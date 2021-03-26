#include <windows.h>
#include <winioctl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strsafe.h>

#include "install.h"



#define PARAM_IDENTIFIER ('/')
#define BINARY_NAME ("drivstaller")
#define VERSION ("1.1.1")
#define LAST_CHANGED ("26.03.2021")



typedef struct CmdParams {
	CHAR path[MAX_PATH];
	SIZE_T path_size;
	CHAR* service_name;
	SIZE_T service_name_size;
	INT mode;
	DWORD start_type;
} CmdParams, * PCmdParams;



VOID
printUsage();

VOID
printHelp();

BOOL
parseArgs(
	_In_ INT argc, 
	_In_reads_(argc) CHAR** argv, 
	_Out_ CmdParams* params);

BOOL
isArgOfType(
	_In_ CHAR* arg,
	_In_ CHAR* type
);

BOOL hasValue(
	char* type, 
	int i, 
	int end_i
);

BOOL
isAskForHelp(
	_In_ INT argc,
	_In_reads_(argc) CHAR** argv
);



INT __cdecl main(_In_ ULONG argc, _In_reads_(argc) PCHAR argv[])
{
	CmdParams params = { 0 };
	BOOL s = TRUE;

	if ( argc < 2 )
	{
		printUsage();
		return 0;
	}

	if ( isAskForHelp(argc, argv) )
	{
		printHelp();
		return TRUE;
	}

	if ( !parseArgs(argc, argv, &params) != 0 )
	{
		printUsage();
		return 0;
	}

	if ( !checkDriverPath(params.path) )
		return 1;

	switch ( params.mode )
	{
		case MODE_INSTALL:
			s = ManageDriver(params.service_name, params.path, params.start_type, MODE_INSTALL);
			if ( !s )
			{
				printf("ERROR: Unable to install driver.\n");

				ManageDriver(params.service_name, params.path, params.start_type, MODE_REMOVE);
			}
			break;

		case MODE_REMOVE:
			s = ManageDriver(params.service_name, params.path, params.start_type, MODE_REMOVE);
			break;

		case MODE_START:
			s = ManageDriver(params.service_name, params.path, params.start_type, MODE_START);
			break;

		case MODE_STOP:
			s = ManageDriver(params.service_name, params.path, params.start_type, MODE_STOP);
			break;

		default:
			break;
	}

	if ( s )
		printf("SUCCESS!\n");

	return 0;
}

VOID printUsage()
{
	printf("Usage: %s [options] path\\to\\driver.sys [options]\n\n", BINARY_NAME);
	printf("Version: %s\n", VERSION);
	printf("Last changed: %s\n", LAST_CHANGED);
}

VOID printHelp()
{
	printUsage();
	printf(
		"\n"
		"Options:\n"
		" * /h Print this.\n"
		" * /i Install and start the driver.\n"
		" * /u Unistall and stop the driver.\n"
		" * /o Start the driver.\n"
		" * /x Stop the driver.\n"
		" * /s Service start type. 0: Boot (started by the system loader), 1: System (started by the IoInitSystem), 2: Auto (started automatically by the SCM), 3: Demand (Default) (started by the SCM with a call to StartService, i.e. the /o parameter), 4: Disabled.\n"
		" * /v Verbose output.\n"
		"\n"
		"The /i, /u, /o, /x options are exclusive.\n"
		"\n"
		"Example: %s path\\to\\drv.sys /i /s 3\n", BINARY_NAME
	);
}

BOOL parseArgs(_In_ INT argc, _In_reads_(argc) CHAR** argv, _Out_ CmdParams* params)
{
	INT start_i = 1;
	INT end_i = argc - 1;
	INT i;
	INT mode_count = 0;
	DWORD fpl = 0;
	DWORD le = 0;
	BOOL error = FALSE;
	BOOL verbose = FALSE;

	// defaults
	params->start_type = SERVICE_DEMAND_START;

	// if first argument is the input file
	if ( argv[1][0] != PARAM_IDENTIFIER )
	{
		fpl = GetFullPathNameA(argv[1], MAX_PATH, params->path, &params->service_name);
		le = GetLastError();
		params->path_size = fpl;
		params->path[MAX_PATH - 1] = 0;
		start_i = 2;
		end_i = argc;
	}

	for ( i = start_i; i < end_i; i++ )
	{
		if ( argv[i][0] != PARAM_IDENTIFIER )
			continue;

		if ( isArgOfType(argv[i], "i") )
		{
			params->mode = MODE_INSTALL;
			mode_count++;
		}
		else if ( isArgOfType(argv[i], "u") )
		{
			params->mode = MODE_REMOVE;
			mode_count++;
		}
		else if ( isArgOfType(argv[i], "o") )
		{
			params->mode = MODE_START;
			mode_count++;
		}
		else if ( isArgOfType(argv[i], "x") )
		{
			params->mode = MODE_STOP;
			mode_count++;
		}
		else if ( isArgOfType(argv[i], "s") )
		{
			if (hasValue("s", i, end_i))
			{
				params->start_type = (DWORD)strtoul(argv[i+1], NULL, 0);
				i++;
			}
		}
		else if (isArgOfType(argv[i], "v"))
		{
			verbose = TRUE;
		}
		else
		{
			printf("INFO: Unknown arg type \"%s\"\n", argv[i]);
		}
	}

	if ( start_i == 1 )
	{
		fpl = GetFullPathNameA(argv[i], MAX_PATH, params->path, &params->service_name);
		le = GetLastError();
		params->path_size = fpl;
		params->path[MAX_PATH - 1] = 0;
	}
	
	CHAR* stream = strrchr(params->service_name, ':');
	if ( stream )
		params->service_name = ++stream;

	if ( mode_count > 1 )
	{
		printf("ERROR: Selected more than 1 mode!\n");
		error = TRUE;
	}
	if ( mode_count == 0 )
	{
		printf("ERROR: No mode selected!\n");
		error = TRUE;
	}

	if ( params->path == 0 || fpl == 0 )
	{
		printf("ERROR (0x%x): No driver name passed!\n", le);
		error = TRUE;
	}
	if ( params->service_name == 0 )
	{
		printf("ERROR (0x%x): No service name found!\n", le);
		error = TRUE;
	}

	if (params->start_type != SERVICE_AUTO_START && 
		params->start_type != SERVICE_BOOT_START && 
		params->start_type != SERVICE_DEMAND_START && 
		params->start_type != SERVICE_DISABLED && 
		params->start_type != SERVICE_SYSTEM_START)
	{
		printf("ERROR: Unknown Service start type!\n");
		error = TRUE;
	}

	if (error)
		return FALSE;

	params->service_name_size = strnlen(params->service_name, MAX_PATH);

	if (verbose)
	{
		printf("path: %s (%zu)\n", params->path, params->path_size);
		printf("name: %s (%zu)\n", params->service_name, params->service_name_size);

		if (params->mode == MODE_INSTALL) printf("mode: %s\n", "MODE_INSTALL");
		else if (params->mode == MODE_REMOVE) printf("mode: %s\n", "MODE_REMOVE");
		else if (params->mode == MODE_START) printf("mode: %s\n", "MODE_START");
		else if (params->mode == MODE_STOP) printf("mode: %s\n", "MODE_STOP");

		if (params->start_type == SERVICE_AUTO_START) printf("start type: SERVICE_AUTO_START\n");
		else if (params->start_type == SERVICE_BOOT_START) printf("start type: SERVICE_BOOT_START\n");
		else if (params->start_type == SERVICE_DEMAND_START) printf("start type: SERVICE_DEMAND_START\n");
		else if (params->start_type == SERVICE_DISABLED) printf("start type: SERVICE_DISABLED\n");
		else if (params->start_type == SERVICE_SYSTEM_START) printf("start type: SERVICE_SYSTEM_START\n");

		printf("\n");
	}

	return TRUE;
}

BOOL isAskForHelp(_In_ INT argc, _In_reads_(argc) CHAR** argv)
{
	if ( argc < 2 )
		return FALSE;

	return isArgOfType(argv[1], "h") || isArgOfType(argv[1], "?");
}

BOOL isArgOfType(_In_ CHAR* arg, _In_ CHAR* type)
{
	size_t type_ln;

	type_ln = strlen(type);

	return strnlen(&arg[1], 10) == type_ln && strncmp(&arg[1], type, type_ln) == 0;
}

BOOL hasValue(char* type, int i, int end_i)
{
	if (i >= end_i)
	{
		printf("INFO: Arg \"%c%s\" has no value! Skipped!\n", PARAM_IDENTIFIER, type);
		return FALSE;
	}

	return TRUE;
}
