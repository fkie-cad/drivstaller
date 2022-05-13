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
#define VERSION ("1.1.4")
#define LAST_CHANGED ("13.05.2022")



typedef struct _CMD_PARAMS {
    CHAR Path[MAX_PATH];
    SIZE_T PathSize;
    CHAR ServiceName[MAX_PATH];
    SIZE_T ServiceNameSize;
    PCHAR Dependencies;
    ULONG DependenciesSize;
    ULONG NrOfDependencies;
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
    _Out_ CMD_PARAMS* params);

INT 
parseDependencies(
    _In_ INT argc, 
    _In_reads_(argc) CHAR** argv, 
    _Inout_ CMD_PARAMS* params, 
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

    switch ( params.Mode )
    {
        case MODE_INSTALL:
            //printf("Installing %s\n", params.Path);
            s = ManageDriver(params.ServiceName, params.Path, params.StartType, params.Dependencies, MODE_INSTALL);
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

    if ( params.Dependencies )
        free(params.Dependencies);

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
        " * /d A driver dependency. If more dependencies are needed, pass more /d options (<= 0x10) in the required order.\n"
        " * /v Verbose output.\n"
        " * /h Print this.\n"
        "\n"
        "The /i, /u, /o, /x options are exclusive.\n"
        "\n"
        "Example: %s path\\to\\drv.sys /i /s 3\n", BINARY_NAME
    );
}

BOOL parseArgs(_In_ INT argc, _In_reads_(argc) CHAR** argv, _Out_ CMD_PARAMS* params)
{
    INT start_i = 1;
    INT i;
    INT mode_count = 0;
    DWORD fpl = 0;
    DWORD le = 0;
    BOOL error = FALSE;
    BOOL verbose = FALSE;
    PCHAR path = NULL;
    INT depIds[0x10] = {0};
    INT depIdsCount = 0;

    // defaults
    params->StartType = SERVICE_DEMAND_START;

    for ( i = start_i; i < argc; i++ )
    {
        //if ( argv[i][0] != PARAM_IDENTIFIER )
        //    continue;

        if ( isArgOfType(argv[i], "i") )
        {
            params->Mode = MODE_INSTALL;
            mode_count++;
        }
        else if ( isArgOfType(argv[i], "n") )
        {
            if ( hasValue("n", i, argc) )
            {
                strcpy(params->ServiceName, argv[i+1]);
                i++;
            }
        }
        else if ( isArgOfType(argv[i], "u") )
        {
            params->Mode = MODE_REMOVE;
            mode_count++;
        }
        else if ( isArgOfType(argv[i], "o") )
        {
            params->Mode = MODE_START;
            mode_count++;
        }
        else if ( isArgOfType(argv[i], "x") )
        {
            params->Mode = MODE_STOP;
            mode_count++;
        }
        else if ( isArgOfType(argv[i], "s") )
        {
            if (hasValue("s", i, argc))
            {
                params->StartType = (DWORD)strtoul(argv[i+1], NULL, 0);
                i++;
            }
        }
        else if ( isArgOfType(argv[i], "d") )
        {
            if (hasValue("d", i, argc))
            {
                if ( depIdsCount < 0x10 )
                {
                    depIds[depIdsCount] = i+1;
                    depIdsCount++;
                }
                else
                {
                    printf("Maximum number of dependencies reached!");
                }
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

    parseDependencies(argc, argv, params, depIds, depIdsCount);
    
    PCHAR bname = NULL;
    if ( path )
    {
        if ( params->ServiceName[0] == 0 )
        {
            fpl = GetFullPathNameA(path, MAX_PATH, params->Path, &bname);
            strcpy(params->ServiceName, bname);
        }
        else
        {
            fpl = GetFullPathNameA(path, MAX_PATH, params->Path, NULL);
        }
        le = GetLastError();
        if ( le == ERROR_SUCCESS )
        {
            params->PathSize = fpl;
            params->Path[MAX_PATH - 1] = 0;
        }
        else
        {
            params->PathSize = 0;
            params->Path[0] = 0;
            params->ServiceName[0] = 0;
            params->ServiceNameSize = 0;
        }
    }

    if ( params->ServiceName[0] != 0 )
    {
        CHAR* stream = NULL;
        if (bname) 
            stream = strrchr(bname, ':');

        if ( stream )
        {
            strcpy(params->ServiceName, ++stream);
            params->ServiceNameSize = strlen(params->ServiceName);
        }
        else
        {
            params->ServiceNameSize = strlen(params->ServiceName);
            // remove ".sys"
            if ( params->ServiceNameSize > 4 )
            {
                if ( *(ULONG*)&params->ServiceName[params->ServiceNameSize-4] == 0x7379732e )
                {
                    params->ServiceNameSize -= 4;
                    params->ServiceName[params->ServiceNameSize] = 0;
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

    if ( params->Mode == MODE_INSTALL && ( params->Path == 0 || fpl == 0 ) )
    {
        printf("ERROR (0x%x): No driver name passed!\n", le);
        error = TRUE;
    }
    if ( params->ServiceName[0] == 0 )
    {
        printf("ERROR (0x%x): No service name found!\n", le);
        error = TRUE;
    }

    if (params->StartType != SERVICE_AUTO_START && 
        params->StartType != SERVICE_BOOT_START && 
        params->StartType != SERVICE_DEMAND_START && 
        params->StartType != SERVICE_DISABLED && 
        params->StartType != SERVICE_SYSTEM_START)
    {
        printf("ERROR: Unknown Service start type!\n");
        error = TRUE;
    }

    if (error)
        return FALSE;

    if (verbose)
    {
        printf("path: %s (%zu)\n", params->Path, params->PathSize);
        printf("name: %s (%zu)\n", params->ServiceName, params->ServiceNameSize);

        if (params->Mode == MODE_INSTALL) printf("mode: %s\n", "MODE_INSTALL");
        else if (params->Mode == MODE_REMOVE) printf("mode: %s\n", "MODE_REMOVE");
        else if (params->Mode == MODE_START) printf("mode: %s\n", "MODE_START");
        else if (params->Mode == MODE_STOP) printf("mode: %s\n", "MODE_STOP");
        else printf("mode: %s\n", "UNKNOWN");
        
        if ( params->Mode == MODE_INSTALL )
        {
            if (params->StartType == SERVICE_AUTO_START) printf("start type: SERVICE_AUTO_START\n");
            else if (params->StartType == SERVICE_BOOT_START) printf("start type: SERVICE_BOOT_START\n");
            else if (params->StartType == SERVICE_DEMAND_START) printf("start type: SERVICE_DEMAND_START\n");
            else if (params->StartType == SERVICE_DISABLED) printf("start type: SERVICE_DISABLED\n");
            else if (params->StartType == SERVICE_SYSTEM_START) printf("start type: SERVICE_SYSTEM_START\n");
            else printf("start type: %s\n", "UNKNOWN");
        }

        if ( params->NrOfDependencies > 0 )
        {
            printf("Dependencies (%u):\n", params->NrOfDependencies);
            ULONG offset = 0;
            for ( i = 0; i < (INT)params->NrOfDependencies; i++ )
            {
                printf(" [%u] %s\n", (i+1), &params->Dependencies[offset]);
                offset += (ULONG)strlen(&params->Dependencies[offset]) + 1;
            }
        }

        printf("\n");
    }

    return TRUE;
}

INT parseDependencies(_In_ INT argc, _In_reads_(argc) CHAR** argv, _Inout_ CMD_PARAMS* params, _In_ PINT DependencyIds, _In_ INT DependencyIdsCount)
{
    INT i;
    PCHAR arg = NULL;
    SIZE_T reqSize = 0;
    SIZE_T offset;
    ULONG count = 0;

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

        reqSize += strlen(arg) + 1; // string terminating 0
        count++;
    }

    if ( reqSize == 0 )
        return -1;
    
    // dependencies array terminating 0
    // + one extra 0 for wrong implementations
    reqSize += 2;

    if ( reqSize > ULONG_MAX )
        return -1;

    params->Dependencies = (PCHAR)malloc(reqSize);
    if ( !params->Dependencies )
        return -1;

    params->NrOfDependencies = count;
    params->DependenciesSize = (ULONG)reqSize;

    offset = 0;
    for ( i = 0; i < DependencyIdsCount; i++ )
    {
        if ( DependencyIds[i] == -1)
        {
            continue;
        }
        
        arg = argv[DependencyIds[i]];

        reqSize = strlen(arg);

        memcpy(&params->Dependencies[offset], arg, reqSize);
        offset += reqSize;
        params->Dependencies[offset] = 0; // terminate string
        offset++;
    }
    params->Dependencies[offset] = 0; // terminate array
    offset++;
    params->Dependencies[offset] = 0; // extra termination

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
