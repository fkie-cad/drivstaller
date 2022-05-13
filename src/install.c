#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strsafe.h>

#include "install.h"

#pragma warning( disable : 4995 )



BOOL
InstallDriver(
    _In_ SC_HANDLE SchSCManager,
    _In_ LPCTSTR Name,
    _In_ LPCTSTR ServiceExe,
    _In_ DWORD StartType,
    _In_ PCHAR Dependencies
);


BOOL
RemoveDriver(
    _In_ SC_HANDLE SchSCManager,
    _In_ LPCTSTR Name
);

BOOL
StartDriver(
    _In_ SC_HANDLE chSCManager,
    _In_ LPCTSTR Name
);

BOOL
StopDriver(
    _In_ SC_HANDLE SchSCManager,
    _In_ LPCTSTR Name
);



BOOL ManageDriver(_In_ LPCTSTR Name, _In_ LPCTSTR ServiceExe, _In_ DWORD StartType, _In_ PCHAR Dependencies, _In_ USHORT Mode)
{
    SC_HANDLE   schSCManager;
    BOOL rCode = TRUE;

    if ( !Name || !ServiceExe )
    {
        printf("ERROR: Invalid Driver or Service provided to ManageDriver() \n");

        return FALSE;
    }

    schSCManager = OpenSCManager(
        NULL,                   // machine name: local
        NULL,                   // database name: local
        SC_MANAGER_ALL_ACCESS   // access 
    );

    if ( !schSCManager )
    {
        printf("ERROR (0x%x): Open SC Manager failed!\n", GetLastError());

        return FALSE;
    }

    switch ( Mode )
    {
        case MODE_INSTALL:
            if ( InstallDriver(schSCManager, Name, ServiceExe, StartType, Dependencies) )
            {
                //if ( StartType == SERVICE_AUTO_START ||
                //     StartType == SERVICE_BOOT_START ||
                //     StartType == SERVICE_SYSTEM_START )
                {
                    rCode = StartDriver(schSCManager, Name);
                }
            }
            else
                rCode = FALSE;

            break;

        case MODE_REMOVE:
            rCode = StopDriver(schSCManager, Name);
            if ( !RemoveDriver(schSCManager, Name) )
                rCode = FALSE;

            break;

        case MODE_START:
            rCode = StartDriver(schSCManager, Name);

            break;

        case MODE_STOP:
            rCode = StopDriver(schSCManager, Name);

            break;

        default:
            printf("ERROR: Unknown ManageDriver() function. \n");

            rCode = FALSE;
            break;
    }

    if ( schSCManager )
        CloseServiceHandle(schSCManager);

    return rCode;
}

/**
 * Install the driver
 */
BOOL InstallDriver(_In_ SC_HANDLE SchSCManager, _In_ LPCTSTR Name, _In_ LPCTSTR ServiceExe, _In_ DWORD StartType, _In_ PCHAR Dependencies)
{
    SC_HANDLE schService;
    DWORD err;

    printf("Creating driver service %s\n", Name);

    // Creates an entry for a standalone driver. 
    // May be extended later.
    schService = CreateServiceA(
        SchSCManager,           // handle of service control manager database
        Name,             // address of name of service to start
        Name,             // address of display name
        SERVICE_ALL_ACCESS,     // type of access to service
        SERVICE_KERNEL_DRIVER,  // type of service
        StartType,              // when to start service
        SERVICE_ERROR_NORMAL,   // severity if service fails to start
        ServiceExe,             // address of name of binary file
        NULL,                   // service does not belong to a group
        NULL,                   // no tag requested
        Dependencies,           // A pointer to a double null-terminated array of null-separated names of services or load ordering groups that the system must start before this service. Specify NULL or an empty string if the service has no dependencies. Dependency on a group means that this service can run if at least one member of the group is running after an attempt to start all members of the group.
                                // You must prefix group names with SC_GROUP_IDENTIFIER so that they can be distinguished from a service name, because services and service groups share the same name space.
        NULL,                   // use LocalSystem account
        NULL                    // no password for service account
    );

    if ( schService == NULL )
    {
        err = GetLastError();

        if ( err == ERROR_SERVICE_EXISTS )
        {
            printf("INFO: Service already exists!\n");
            return TRUE;
        }
        else
        {
            printf("ERROR (0x%x): CreateService failed! \n", err);
            return  FALSE;
        }
    }

    if ( schService )
        CloseServiceHandle(schService);

    return TRUE;
}

BOOL RemoveDriver(_In_ SC_HANDLE SchSCManager, _In_ LPCTSTR Name)
{
    SC_HANDLE schService;
    BOOL rCode;
    
    printf("Removing driver service %s\n", Name);

    schService = OpenService(SchSCManager, Name, SERVICE_ALL_ACCESS);

    if ( schService == NULL )
    {
        printf("ERROR (0x%x): OpenService failed! \n", GetLastError());
        return FALSE;
    }

    if ( DeleteService(schService) )
    {
        rCode = TRUE;
    }
    else
    {
        printf("ERROR (0x%x): DeleteService failed! \n", GetLastError());
        rCode = FALSE;
    }

    if ( schService )
        CloseServiceHandle(schService);

    return rCode;
}

BOOL StartDriver(_In_ SC_HANDLE SchSCManager, _In_ LPCTSTR Name)
{
    SC_HANDLE schService;
    DWORD err;
    
    printf("Starting driver %s\n", Name);

    schService = OpenService(SchSCManager, Name, SERVICE_ALL_ACCESS);

    if ( schService == NULL )
    {
        printf("ERROR (0x%x): OpenService failed!\n", GetLastError());
        return FALSE;
    }

    if ( !StartService(schService, 0, NULL) )
    {
        err = GetLastError();

        if ( err == ERROR_SERVICE_ALREADY_RUNNING )
        {
            printf("INFO: Service already running!\n");
            return TRUE;
        }
        else
        {
            printf("ERROR (0x%x): StartService failure!\n", err);
            return FALSE;
        }
    }

    if ( schService )
        CloseServiceHandle(schService);

    return TRUE;

}

BOOL StopDriver(_In_ SC_HANDLE SchSCManager, _In_ LPCTSTR Name)
{
    BOOL rCode = TRUE;
    SC_HANDLE schService;
    SERVICE_STATUS serviceStatus;
    ULONG le;
    
    printf("Stopping driver %s\n", Name);

    schService = OpenService(SchSCManager, Name, SERVICE_ALL_ACCESS);

    if ( schService == NULL )
    {
        le = GetLastError();
        if ( le == ERROR_SERVICE_DOES_NOT_EXIST )
        {
            printf("Service does not exist.\n");
            return TRUE;
        }
        else
        {
            printf("ERROR (0x%x): OpenService failed!\n", le);
            return FALSE;
        }
    }

    if ( ControlService(schService, SERVICE_CONTROL_STOP, &serviceStatus) )
    {
        rCode = TRUE;
    }
    else
    {
        le = GetLastError();
        if ( le == ERROR_SERVICE_NOT_ACTIVE )
        {
            printf("Service not active.\n");
            rCode = TRUE;
        }
        else
        {
            printf("ERROR (0x%x): ControlService (stop) failed!\n", le);
            rCode = FALSE;
        }
    }

    if ( schService )
        CloseServiceHandle(schService);

    return rCode;

}
