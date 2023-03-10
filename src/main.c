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
#define VERSION ("1.1.6")
#define LAST_CHANGED ("02.09.2022")


#define MAX_DEPENDENCIES (0x10)



typedef struct _DEPENDENCIES {
    PCHAR Buffer;
    ULONG Size; // buffer size
    ULONG Count; // string count
} DEPENDENCIES, *PDEPENDENCIES;

typedef struct _CMD_PARAMS {
    CHAR Path[MAX_PATH];
    SIZE_T PathSize;
    CHAR ServiceName[MAX_PATH];
    SIZE_T ServiceNameSize;
    DEPENDENCIES Dependencies;
    INT Mode;
    DWORD StartType;
} CMD_PARAMS, *PCMD_PARAMS;



VOID
printUsage();

VOID
printHelp();

BOOL
parseArgs(
    _In_ INT argc, 
    _In_reads_(argc) CHAR** argv, 
    _Out_ CMD_PARAMS* Params);

INT 
parseDependencies(
    _In_ INT argc, 
    _In_reads_(argc) CHAR** argv, 
    _Inout_ PDEPENDENCIES Dependencies, 
    _In_ PINT DependencyIds, 
    _In_ INT DependencyIdsCount
);

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

BOOL
IsProcessElevated();



INT __cdecl main(_In_ ULONG argc, _In_reads_(argc) PCHAR argv[])
{
    CMD_PARAMS params = { 0 };
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

    if ( params.Mode == MODE_INSTALL && !FileExists(params.Path) )
        return 1;

    if ( !IsProcessElevated() )
    {
        printf("ERROR (0x%x): Not elevated! Please run as Admin.\n", -1);
        return -1;
    }

    switch ( params.Mode )
    {
        case MODE_CHECK:
            //printf("Stopping %s\n", params.Path);
            s = ManageDriver(params.ServiceName, params.Path, params.StartType, NULL, MODE_CHECK);
            break;

        case MODE_INSTALL:
            //printf("Installing %s\n", params.Path);
            s = ManageDriver(params.ServiceName, params.Path, params.StartType, params.Dependencies.Buffer, MODE_INSTALL);
            if ( !s )
            {
                printf("ERROR: Unable to install driver.\n");

                ManageDriver(params.ServiceName, params.Path, params.StartType, NULL, MODE_REMOVE);
            }
            break;

        case MODE_REMOVE:
            //printf("Removing %s\n", params.Path);
            s = ManageDriver(params.ServiceName, params.Path, params.StartType, NULL, MODE_REMOVE);
            break;

        case MODE_START:
            //printf("Starting %s\n", params.Path);
            s = ManageDriver(params.ServiceName, params.Path, params.StartType, NULL, MODE_START);
            break;

        case MODE_STOP:
            //printf("Stopping %s\n", params.Path);
            s = ManageDriver(params.ServiceName, params.Path, params.StartType, NULL, MODE_STOP);
            break;

        default:
            break;
    }

    if ( s )
        printf("SUCCESS!\n");

    if ( params.Dependencies.Buffer )
        free(params.Dependencies.Buffer);

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
        " * /c Check, if the service already exists.\n"
        " * /s Service start type:\n\t0: Boot (started by the system loader)\n"
        "      1: System (started by the IoInitSystem)\n"
        "      2: Auto (started automatically by the SCM)\n"
        "      3: Demand (Default) (started by the SCM with a call to StartService, i.e. the /o parameter)\n"
        "      4: Disabled.\n"
        " * /d A driver dependency. If more dependencies are needed, pass more /d options (<= 0x%x) in the required order.\n"
        " * /v Verbose output.\n"
        " * /h Print this.\n"
        "\n"
        "The /i, /u, /o, /x options are exclusive.\n"
        "\n"
        "Example: %s path\\to\\drv.sys /i /s 3\n", 
        MAX_DEPENDENCIES,
        BINARY_NAME
    );
}

#define NOT_A_VALUE(__val__) (__val__ == NULL || __val__[0] == LIN_PARAM_IDENTIFIER || __val__[0] == WIN_PARAM_IDENTIFIER)
//#define IS_VALUE(__val__) (__val__ != NULL || __val__[0] != LIN_PARAM_IDENTIFIER || __val__[0] != WIN_PARAM_IDENTIFIER)
#define IS_1C_ARG(_a_, _v_) ( ( _a_[0] == LIN_PARAM_IDENTIFIER || _a_[0] == WIN_PARAM_IDENTIFIER ) && _a_[1] == _v_ && _a_[2] == 0 )
BOOL parseArgs(_In_ INT argc, _In_reads_(argc) CHAR** argv, _Out_ CMD_PARAMS* Params)
{
    INT start_i = 1;
    INT i;
    INT mode_count = 0;
    DWORD fpl = 0;
    DWORD le = 0;
    BOOL error = FALSE;
    BOOL verbose = FALSE;
    PCHAR path = NULL;
    INT depIds[MAX_DEPENDENCIES] = {0};
    INT depIdsCount = 0;

    char* arg;
    char *val0;

    // defaults
    Params->StartType = SERVICE_DEMAND_START;

    for ( i = start_i; i < argc; i++ )
    {
        arg = argv[i];
        val0 = ( i < argc - 1 ) ? argv[i+1] : NULL;

        if ( !arg )
            break;
        
        if ( IS_1C_ARG(arg, 'c') )
        {
            Params->Mode = MODE_CHECK;
            mode_count++;
        }
        else if ( IS_1C_ARG(arg, 'd') )
        {
            if ( NOT_A_VALUE(val0) )
            {
                printf("No dependency name!\n");
                continue;
            }
            if ( depIdsCount >= MAX_DEPENDENCIES )
            {
                printf("Maximum number of dependencies reached!");
                continue;
            }

            depIds[depIdsCount] = i+1;
            depIdsCount++;

            i++;
        }
        else if ( IS_1C_ARG(arg, 'i') )
        {
            Params->Mode = MODE_INSTALL;
            mode_count++;
        }
        else if ( IS_1C_ARG(arg, 'n') )
        {
            if ( NOT_A_VALUE(val0) )
            {
                printf("No service name!\n");
                continue;
            }
            strcpy(Params->ServiceName, argv[i+1]);
            i++;
        }
        else if ( IS_1C_ARG(arg, 'o') )
        {
            Params->Mode = MODE_START;
            mode_count++;
        }
        else if ( IS_1C_ARG(arg, 's') )
        {
            if ( NOT_A_VALUE(val0) )
            {
                printf("No start type value!\n");
                continue;
            }
            Params->StartType = (DWORD)strtoul(argv[i+1], NULL, 0);
            i++;
        }
        else if ( IS_1C_ARG(arg, 'u') )
        {
            Params->Mode = MODE_REMOVE;
            mode_count++;
        }
        else if ( IS_1C_ARG(arg, 'v') )
        {
            verbose = TRUE;
        }
        else if ( IS_1C_ARG(arg, 'x') )
        {
            Params->Mode = MODE_STOP;
            mode_count++;
        }
        else
        {
            if ( NOT_A_VALUE(arg) )
            {
                printf("Unknown arg!\n");
                continue;
            }
            path = arg;
        }
    }

    parseDependencies(argc, argv, &Params->Dependencies, depIds, depIdsCount);
    
    PCHAR bname = NULL;
    if ( path )
    {
        if ( Params->ServiceName[0] == 0 )
        {
            fpl = GetFullPathNameA(path, MAX_PATH, Params->Path, &bname);
            strcpy(Params->ServiceName, bname);
        }
        else
        {
            fpl = GetFullPathNameA(path, MAX_PATH, Params->Path, NULL);
        }
        le = GetLastError();
        if ( le == ERROR_SUCCESS )
        {
            Params->PathSize = fpl;
            Params->Path[MAX_PATH - 1] = 0;
        }
        else
        {
            Params->PathSize = 0;
            Params->Path[0] = 0;
            Params->ServiceName[0] = 0;
            Params->ServiceNameSize = 0;
        }
    }

    if ( Params->ServiceName[0] != 0 )
    {
        CHAR* stream = NULL;
        if (bname) 
            stream = strrchr(bname, ':');

        if ( stream )
        {
            strcpy(Params->ServiceName, ++stream);
            Params->ServiceNameSize = strlen(Params->ServiceName);
        }
        else
        {
            Params->ServiceNameSize = strlen(Params->ServiceName);
            // remove ".sys"
            if ( Params->ServiceNameSize > 4 )
            {
                if ( *(ULONG*)&Params->ServiceName[Params->ServiceNameSize-4] == 0x7379732e )
                {
                    Params->ServiceNameSize -= 4;
                    Params->ServiceName[Params->ServiceNameSize] = 0;
                }
            }
        }
    }

    //if ( (Params->Mode & (Params->Mode-1)) != 0 )
    if ( mode_count > 1 )
    {
        printf("ERROR: Selected more than 1 mode!\n");
        error = TRUE;
    }
    //if ( Params->Mode == 0 )
    if ( mode_count == 0 )
    {
        printf("ERROR: No mode selected!\n");
        error = TRUE;
    }

    if ( Params->Mode == MODE_INSTALL && ( Params->Path == 0 || fpl == 0 ) )
    {
        printf("ERROR (0x%x): No driver name passed!\n", le);
        error = TRUE;
    }
    if ( Params->ServiceName[0] == 0 )
    {
        printf("ERROR (0x%x): No service name found!\n", le);
        error = TRUE;
    }

    if (Params->StartType != SERVICE_AUTO_START && 
        Params->StartType != SERVICE_BOOT_START && 
        Params->StartType != SERVICE_DEMAND_START && 
        Params->StartType != SERVICE_DISABLED && 
        Params->StartType != SERVICE_SYSTEM_START)
    {
        printf("ERROR: Unknown Service start type!\n");
        error = TRUE;
    }

    if (error)
        return FALSE;

    if (verbose)
    {
        printf("path: %s (%zu)\n", Params->Path, Params->PathSize);
        printf("name: %s (%zu)\n", Params->ServiceName, Params->ServiceNameSize);

        if (Params->Mode == MODE_INSTALL) printf("mode: %s\n", "MODE_INSTALL");
        else if (Params->Mode == MODE_REMOVE) printf("mode: %s\n", "MODE_REMOVE");
        else if (Params->Mode == MODE_START) printf("mode: %s\n", "MODE_START");
        else if (Params->Mode == MODE_STOP) printf("mode: %s\n", "MODE_STOP");
        else printf("mode: %s\n", "UNKNOWN");
        
        if ( Params->Mode == MODE_INSTALL )
        {
            if (Params->StartType == SERVICE_BOOT_START) printf("start type: SERVICE_BOOT_START\n");
            else if (Params->StartType == SERVICE_SYSTEM_START) printf("start type: SERVICE_SYSTEM_START\n");
            else if (Params->StartType == SERVICE_AUTO_START) printf("start type: SERVICE_AUTO_START\n");
            else if (Params->StartType == SERVICE_DEMAND_START) printf("start type: SERVICE_DEMAND_START\n");
            else if (Params->StartType == SERVICE_DISABLED) printf("start type: SERVICE_DISABLED\n");
            else printf("start type: %s\n", "UNKNOWN");
        }

        if ( Params->Dependencies.Count > 0 )
        {
            printf("Dependencies (%u):\n", Params->Dependencies.Count );
            ULONG offset = 0;
            for ( i = 0; i < (INT)Params->Dependencies.Count; i++ )
            {
                printf(" [%d] %s\n", (i+1), &Params->Dependencies.Buffer[offset]);
                offset += (ULONG)strlen(&Params->Dependencies.Buffer[offset]) + 1;
            }
        }

        printf("\n");
    }

    return TRUE;
}

INT parseDependencies(_In_ INT argc, _In_reads_(argc) CHAR** argv, _Inout_ PDEPENDENCIES Dependencies, _In_ PINT DependencyIds, _In_ INT DependencyIdsCount)
{
    INT i;
    PCHAR arg = NULL;
    SIZE_T reqSize = 0;
    SIZE_T offset;
    ULONG count = 0;

    //
    // get required size of flat string array

    for ( i = 0; i < DependencyIdsCount; i++ )
    {
        if ( DependencyIds[i] >= argc )
        {
            DependencyIds[i] = -1;
            continue;
        }

        arg = argv[DependencyIds[i]];

        if ( arg == NULL || arg[0] == 0 || arg[0] == LIN_PARAM_IDENTIFIER || arg[0] == WIN_PARAM_IDENTIFIER )
        {
            DependencyIds[i] = -1;
            continue;
        }

        reqSize += strlen(arg) + 1; // string + terminating 0
        count++;
    }

    if ( reqSize == 0 )
        return -1;
    
    // add dependencies array terminating 0
    // + one extra 0 for wrong implementations
    reqSize += 2;

    if ( reqSize > ULONG_MAX )
        return -1;


    //
    // alloc buffer
    
    Dependencies->Buffer = (PCHAR)malloc(reqSize);
    if ( !Dependencies->Buffer )
        return -1;


    //
    // fill array buffer with strings

    Dependencies->Count = count;
    Dependencies->Size = (ULONG)reqSize;

    offset = 0;
    for ( i = 0; i < DependencyIdsCount; i++ )
    {
        if ( DependencyIds[i] == -1)
        {
            continue;
        }
        
        arg = argv[DependencyIds[i]];

        reqSize = strlen(arg);

        memcpy(&Dependencies->Buffer[offset], arg, reqSize);
        offset += reqSize;
        Dependencies->Buffer[offset] = 0; // terminate string
        offset++;
    }
    Dependencies->Buffer[offset] = 0; // terminate array
    offset++;
    Dependencies->Buffer[offset] = 0; // extra termination

    return 0;
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

BOOL IsProcessElevated()
{
	BOOL fIsElevated = FALSE;
	HANDLE hToken = NULL;
	TOKEN_ELEVATION elevation;
	DWORD dwSize;

	if ( !OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &hToken) )
	{
        printf("(0x%x): OpenProcessToken failed\n", GetLastError());
		goto clean;  // if Failed, we treat as False
	}

	if ( !GetTokenInformation(hToken, TokenElevation, &elevation, sizeof(elevation), &dwSize) )
	{	
        printf("ERROR (0x%x): GetTokenInformation failed\n", GetLastError());
		goto clean;// if Failed, we treat as False
	}

	fIsElevated = elevation.TokenIsElevated;

clean:
	if (hToken)
	{
		CloseHandle(hToken);
		hToken = NULL;
	}
	return fIsElevated; 
}
