#include <windows.h>
#include <winioctl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strsafe.h>

#pragma warning ( disable : 4996 )

#include "install.h"
#include "Files.h"



#define LIN_PARAM_IDENTIFIER ('/')
#define WIN_PARAM_IDENTIFIER ('-')
#define PARAM_IDENTIFIER WIN_PARAM_IDENTIFIER

#define BINARY_NAME ("drivstaller")
#define VERSION ("1.1.3")
#define LAST_CHANGED ("29.09.2021")



typedef struct CmdParams {
    CHAR path[MAX_PATH];
    SIZE_T path_size;
    CHAR service_name[MAX_PATH];
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

    printf("%s - %s\n\n", BINARY_NAME, VERSION);

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

    if ( !parseArgs(argc, argv, &params) )
    {
        printUsage();
        return 0;
    }

    if ( params.mode == MODE_INSTALL && !FileExists(params.path) )
        return 1;

    switch ( params.mode )
    {
        case MODE_INSTALL:
            //printf("Installing %s\n", params.path);
            s = ManageDriver(params.service_name, params.path, params.start_type, MODE_INSTALL);
            if ( !s )
            {
                printf("ERROR: Unable to install driver.\n");

                ManageDriver(params.service_name, params.path, params.start_type, MODE_REMOVE);
            }
            break;

        case MODE_REMOVE:
            //printf("Removing %s\n", params.path);
            s = ManageDriver(params.service_name, params.path, params.start_type, MODE_REMOVE);
            break;

        case MODE_START:
            //printf("Starting %s\n", params.path);
            s = ManageDriver(params.service_name, params.path, params.start_type, MODE_START);
            break;

        case MODE_STOP:
            //printf("Stopping %s\n", params.path);
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
    printf("Compiled: %s - %s\n", __DATE__, __TIME__);
}

VOID printHelp()
{
    printUsage();
    printf(
        "\n"
        "Options:\n"
        " * /n Name of service. If not set, it will be derived of the driver path.\n"
        " * /i Install and start the driver.\n"
        " * /u Unistall and stop the driver.\n"
        " * /o Start the driver.\n"
        " * /x Stop the driver.\n"
        " * /s Service start type:\n\t0: Boot (started by the system loader)\n\t1: System (started by the IoInitSystem)\n\t2: Auto (started automatically by the SCM)\n\t3: Demand (Default) (started by the SCM with a call to StartService, i.e. the /o parameter)\n\t4: Disabled.\n"
        " * /v Verbose output.\n"
        " * /h Print this.\n"
        "\n"
        "The /i, /u, /o, /x options are exclusive.\n"
        "\n"
        "Example: %s path\\to\\drv.sys /i /s 3\n", BINARY_NAME
    );
}

BOOL parseArgs(_In_ INT argc, _In_reads_(argc) CHAR** argv, _Out_ CmdParams* params)
{
    INT start_i = 1;
    INT i;
    INT mode_count = 0;
    DWORD fpl = 0;
    DWORD le = 0;
    BOOL error = FALSE;
    BOOL verbose = FALSE;
    PCHAR path = NULL;

    // defaults
    params->start_type = SERVICE_DEMAND_START;

    for ( i = start_i; i < argc; i++ )
    {
        //if ( argv[i][0] != PARAM_IDENTIFIER )
        //    continue;

        if ( isArgOfType(argv[i], "i") )
        {
            params->mode = MODE_INSTALL;
            mode_count++;
        }
        else if ( isArgOfType(argv[i], "n") )
        {
            if ( hasValue("n", i, argc) )
            {
                strcpy(params->service_name, argv[i+1]);
                i++;
            }
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
            if (hasValue("s", i, argc))
            {
                params->start_type = (DWORD)strtoul(argv[i+1], NULL, 0);
                i++;
            }
        }
        else if ( isArgOfType(argv[i], "v") )
        {
            verbose = TRUE;
        }
        else
        {
            path = argv[i];
        }
    }
    
    PCHAR bname = NULL;
    if ( path )
    {
        if ( params->service_name[0] == 0 )
        {
            fpl = GetFullPathNameA(path, MAX_PATH, params->path, &bname);
            strcpy(params->service_name, bname);
        }
        else
        {
            fpl = GetFullPathNameA(path, MAX_PATH, params->path, NULL);
        }
        le = GetLastError();
        if ( le == ERROR_SUCCESS )
        {
            params->path_size = fpl;
            params->path[MAX_PATH - 1] = 0;
        }
        else
        {
            params->path_size = 0;
            params->path[0] = 0;
            params->service_name[0] = 0;
            params->service_name_size = 0;
        }
    }

    if ( params->service_name[0] != 0 )
    {
        CHAR* stream = NULL;
        if (bname) 
            stream = strrchr(bname, ':');

        if ( stream )
        {
            strcpy(params->service_name, ++stream);
            params->service_name_size = strlen(params->service_name);
        }
        else
        {
            params->service_name_size = strlen(params->service_name);
            // remove ".sys"
            if ( params->service_name_size > 4 )
            {
                if ( *(ULONG*)&params->service_name[params->service_name_size-4] == 0x7379732e )
                {
                    params->service_name_size -= 4;
                    params->service_name[params->service_name_size] = 0;
                }
            }
        }
    }

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

    if ( params->mode == MODE_INSTALL && ( params->path == 0 || fpl == 0 ) )
    {
        printf("ERROR (0x%x): No driver name passed!\n", le);
        error = TRUE;
    }
    if ( params->service_name[0] == 0 )
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

    if (verbose)
    {
        printf("path: %s (%zu)\n", params->path, params->path_size);
        printf("name: %s (%zu)\n", params->service_name, params->service_name_size);

        if (params->mode == MODE_INSTALL) printf("mode: %s\n", "MODE_INSTALL");
        else if (params->mode == MODE_REMOVE) printf("mode: %s\n", "MODE_REMOVE");
        else if (params->mode == MODE_START) printf("mode: %s\n", "MODE_START");
        else if (params->mode == MODE_STOP) printf("mode: %s\n", "MODE_STOP");
        else printf("mode: %s\n", "UNKNOWN");
        
        if ( params->mode == MODE_INSTALL )
        {
            if (params->start_type == SERVICE_AUTO_START) printf("start type: SERVICE_AUTO_START\n");
            else if (params->start_type == SERVICE_BOOT_START) printf("start type: SERVICE_BOOT_START\n");
            else if (params->start_type == SERVICE_DEMAND_START) printf("start type: SERVICE_DEMAND_START\n");
            else if (params->start_type == SERVICE_DISABLED) printf("start type: SERVICE_DISABLED\n");
            else if (params->start_type == SERVICE_SYSTEM_START) printf("start type: SERVICE_SYSTEM_START\n");
            else printf("start type: %s\n", "UNKNOWN");
        }
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
    if ( arg == NULL || ( arg[0] != LIN_PARAM_IDENTIFIER && arg[0] != WIN_PARAM_IDENTIFIER ) || arg[1] == 0 )
        return FALSE;
    return strcmp(&arg[1], type)==0;
    //INT i;
    //if ( arg == NULL || arg[0] == 0 || arg[1] == 0 )
    //	return FALSE;
    //for ( i = 0; i < n ; i++ )
    //{
    //	printf("%02x\n", arg[i+1]);
    //	if ( arg[i+1] != type[i] )
    //		return FALSE;
    //}

    //return TRUE;
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
