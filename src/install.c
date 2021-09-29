#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strsafe.h>

#include "install.h"



BOOL
InstallDriver(
	_In_ SC_HANDLE SchSCManager,
	_In_ LPCTSTR Name,
	_In_ LPCTSTR ServiceExe,
	_In_ DWORD StartType
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



BOOL ManageDriver(_In_ LPCTSTR Name, _In_ LPCTSTR ServiceExe, _In_ DWORD StartType, _In_ USHORT Mode)
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
			if ( InstallDriver(schSCManager, Name, ServiceExe, StartType) )
				rCode = StartDriver(schSCManager, Name);
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
BOOL InstallDriver(_In_ SC_HANDLE SchSCManager, _In_ LPCTSTR Name, _In_ LPCTSTR ServiceExe, _In_ DWORD StartType)
{
	SC_HANDLE schService;
	DWORD err;

	// Creates an entry for a standalone driver. 
	// May be extended later.
	schService = CreateService(
		SchSCManager,           // handle of service control manager database
		Name,             // address of name of service to start
		Name,             // address of display name
		SERVICE_ALL_ACCESS,     // type of access to service
		SERVICE_KERNEL_DRIVER,  // type of service
		StartType,   // when to start service
		SERVICE_ERROR_NORMAL,   // severity if service fails to start
		ServiceExe,             // address of name of binary file
		NULL,                   // service does not belong to a group
		NULL,                   // no tag requested
		NULL,                   // no dependency names
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

	schService = OpenService(SchSCManager, Name, SERVICE_ALL_ACCESS);

	if ( schService == NULL )
	{
		printf("ERROR (0x%x): OpenService failed!\n", GetLastError());

		return FALSE;
	}

	if ( ControlService(schService, SERVICE_CONTROL_STOP, &serviceStatus) )
	{
		rCode = TRUE;
	}
	else
	{
		printf("ERROR (0x%x): ControlService (stop) failed!\n", GetLastError());

		rCode = FALSE;
	}

	if ( schService )
		CloseServiceHandle(schService);

	return rCode;

}
