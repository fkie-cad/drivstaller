#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strsafe.h>

#include "inc/nt.h"

#include "load.h"

#include "print.h"
#include "registry.h"
#include "ut.h"



#define NT_REG_LM L"\\Registry\\Machine"
#define REG_SERVICE_KEY L"System\\CurrentControlSet\\Services"
//#define REG_KEY_FRMT L"%wS\\" REG_SERVICE_KEY "\\%wS"


NTSTATUS createRegistryKeyPath(
    _In_ LPCWSTR ServiceName,
    _Out_ PWCHAR *RegistryPath,
    _Out_ PULONG RegistryPathSize
);

NTSTATUS startDriver(
    _In_ PWCHAR RegistryPath,
    _In_ LPCWSTR DriverPath,
    _In_ BOOLEAN Reload
);

NTSTATUS createServiceEntry(
    _In_ LPCWSTR DriverPath,
    _In_ LPCWSTR KeyPath,
    _In_ ULONG StartType,
    _In_opt_ PWCHAR Dependencies,
    _In_ ULONG DependenciesSize
);

/**
 * Unload driver and delete registry key
 */
NTSTATUS stopDriver(
    _In_ PWCHAR RegistryKeyPath
);

NTSTATUS deleteServiceKey(
    _In_ LPCWSTR KeyPath,
    _In_ LPCWSTR ServiceName
);

NTSTATUS checkServiceKey(
    _In_ LPCWSTR KeyPath,
    _In_ LPCWSTR ServiceName
);

NTSTATUS printValueCb(
    _In_ HANDLE Key,
    _In_ PWCHAR Name, 
    _In_ PKEY_VALUE_FULL_INFORMATION Info
);



INT ManageNtLoader(
    _In_ PCHAR ServiceNameA,
    _In_ PCHAR DriverFullPathA,
    _In_ ULONG StartType,
    _In_opt_ PWCHAR Dependencies,
    _In_ ULONG DependenciesSize,
    _In_ USHORT Mode
)
{
    FEnter();
    NTSTATUS status = 0;

    PWCHAR driverPathW = NULL;
    size_t driverPathCch = strlen(DriverFullPathA);
    size_t driverPathWCb = driverPathCch*2;

    PWCHAR serviceNameW = NULL;
    size_t serviceNameCch = strlen(ServiceNameA);
    size_t serviceNameWCb = serviceNameCch*2;

    PWCHAR registryKeyPath = NULL;
    ULONG registryKeyPathSize = 0;
    
    BOOLEAN reload = TRUE; // Flags.Reload
    BOOLEAN privsSet = FALSE;

    if ( !ServiceNameA || !DriverFullPathA )
    {
        EPrint("Invalid DriverFullPath or ServiceName provided!\n");

        return ERROR_INVALID_PARAMETER;
    }
    
    //
    // Convert path to wide string
    //
    //if ( Mode != MODE_CHECK )
    {
        driverPathW = (PWCHAR)malloc(driverPathWCb+2);
        if ( !driverPathW )
        {
            status = ERROR_NO_SYSTEM_RESOURCES;
            goto clean;
        }
        //RtlSecureZeroMemory(driverPathW, driverPathWCb+2);
    
        status = StringCchPrintfW(driverPathW, driverPathCch+1, L"%hs", DriverFullPathA);
        if ( status != 0 )
        {
            EPrint("Converting driver path failed! (0x%x)\n", status);
            status = STATUS_UNSUCCESSFUL;
        }
    }
    
    //
    // Convert service name to wide string
    //
    serviceNameW = (PWCHAR)malloc(serviceNameWCb+2);
    if ( !serviceNameW )
    {
        status = ERROR_NO_SYSTEM_RESOURCES;
        goto clean;
    }
    RtlSecureZeroMemory(serviceNameW, serviceNameWCb+2);
    
    status = StringCchPrintfW(serviceNameW, serviceNameCch+1, L"%hs", ServiceNameA);
    if ( status != 0 )
    {
        EPrint("Converting service name failed! (0x%x)\n", status);
        status = STATUS_UNSUCCESSFUL;
    }

    DPrint("ServiceNameA: %s\n", ServiceNameA);
    DPrint("DriverFullPathA: %s\n", DriverFullPathA);
    DPrint("driverPathW: %ws\n", driverPathW);
    DPrint("serviceNameW: %ws\n", serviceNameW);
    
    //
    // Adjust privileges for some modes
    //
    if ( Mode == MODE_INSTALL 
      || Mode == MODE_START 
      || Mode == MODE_REMOVE 
      || Mode == MODE_STOP )
    {
        //
        // Enable required privileges
        //
        ULONG privs[] = {
            SE_DEBUG_PRIVILEGE, 
            SE_LOAD_DRIVER_PRIVILEGE
        };
        status = setPrivileges(privs, ARRAYSIZE(privs), TRUE);
        if ( !NT_SUCCESS(status) )
        {
            EPrint("Requested privileges could not be assigned! (0x%x)\n", status);
            goto clean;
        }
        DPrint("SE_LOAD_DRIVER_PRIVILEGE enabled\n");
        DPrint("SE_DEBUG_PRIVILEGE enabled\n");

        privsSet = TRUE;
    }
    
    //
    // Create registry path.
    // Required for all modes.
    //
    status = createRegistryKeyPath(
                serviceNameW,
                &registryKeyPath,
                &registryKeyPathSize
            );
    if ( status != 0 )
    {
        EPrint("createRegistryKeyPath failed! (0x%x)\n", status);
        goto clean;
    }

    switch ( Mode )
    {
        case MODE_CHECK:
            status = checkServiceKey(registryKeyPath, serviceNameW);
            if ( status == STATUS_OBJECT_NAME_NOT_FOUND )
            {
                printf("Service not found!\n");
                status = 0;
            }

            break;

        case MODE_INSTALL:
            status = createServiceEntry(
                driverPathW, 
                registryKeyPath,
                StartType,
                Dependencies,
                DependenciesSize
            );
            if ( status != 0 )
            {
                EPrint("Creating service entry failed! (0x%x)\n", status);
                goto clean;
            }
            printf("Service registry key created.\n");

            status = startDriver(
                    registryKeyPath,
                    driverPathW,
                    reload
                );
            if ( status != 0 )
            {
                deleteServiceKey(registryKeyPath, serviceNameW);

                EPrint("Starting driver failed! (0x%x)\n", status);
                switch ( status )
                {
                    case STATUS_IMAGE_CERT_REVOKED: printf("    STATUS_IMAGE_CERT_REVOKED\n");break;
                    default: break;
                }
                goto clean;
            }
            printf("Driver started.\n");

            break;

        case MODE_REMOVE:
            DPrint("Unloading driver...\n");
            status = stopDriver(registryKeyPath);
            if ( status != 0 )
            {
                EPrint("Stopping driver failed! (0x%x)\n", status);
            }
            printf("Driver stopped.\n");

            DPrint("Deleting service registry key...\n");
            status = deleteServiceKey(registryKeyPath, serviceNameW);
            if ( status != 0 )
            {
                EPrint("Deleting service registry key failed! (0x%x)\n", status);
                goto clean;
            }
            printf("Service registry key deleted!\n");

            break;

        case MODE_START:
            DPrint("Loadin driver...\n");
            status = startDriver(
                    registryKeyPath,
                    driverPathW,
                    reload
                );
            if ( status != 0 )
            {
                EPrint("Starting driver failed! (0x%x)\n", status);
                switch ( status )
                {
                    case STATUS_IMAGE_CERT_REVOKED: printf("    STATUS_IMAGE_CERT_REVOKED\n");break;
                    default: break;
                }
                goto clean;
            }
            printf("Driver started.\n");

            break;

        case MODE_STOP:
            DPrint("Unloading driver...\n");
            status = stopDriver(registryKeyPath);
            if ( status != 0 )
            {
                EPrint("Stopping driver failed! (0x%x)\n", status);
                break;
            }
            printf("Driver stopped.\n");

            break;

        default:
            EPrint("Unknown manage mode.\n");

            status = STATUS_INVALID_PARAMETER;
            break;
    }

clean:
    if ( privsSet )
    {
        //
        // Disable required privileges
        //
        ULONG privs[] = {
            SE_DEBUG_PRIVILEGE, 
            SE_LOAD_DRIVER_PRIVILEGE
        };
        status = setPrivileges(privs, ARRAYSIZE(privs), FALSE);
        if ( !NT_SUCCESS(status) )
        {
            EPrint("Requested privileges could not be disabled! (0x%x)\n", status);
        }
        DPrint("SE_LOAD_DRIVER_PRIVILEGE disabled\n");
        DPrint("SE_DEBUG_PRIVILEGE disabled\n");
    }
    if ( driverPathW )
        free(driverPathW);
    if ( serviceNameW )
        free(serviceNameW);
    if ( registryKeyPath )
        free(registryKeyPath);

    int e = RtlNtStatusToDosError(status);
    SetLastError(e);
    DPrint("status: 0x%x\n", status);
    DPrint("error: 0x%x\n", e);
    FLeave();
    return e;
}

/**
 * Create service registry entry path
 */
NTSTATUS createRegistryKeyPath(
    _In_ LPCWSTR ServiceName,
    _Out_ PWCHAR *RegistryPath,
    _Out_ PULONG RegistryPathSize
)
{
    FEnter();
    NTSTATUS status = 0;
    PWCHAR regKeyBuffer = NULL;
    // include 0 of first two parts is used to count up for "\" separator
    ULONG regKeyBufferCch = RTL_NUMBER_OF(NT_REG_LM) + RTL_NUMBER_OF(REG_SERVICE_KEY) + (ULONG)wcslen(ServiceName);
    ULONG regKeyBufferCb = regKeyBufferCch * 2;

    *RegistryPath = NULL;
    *RegistryPathSize = 0;

    regKeyBuffer = (PWCHAR)malloc(regKeyBufferCb+2);
    if ( !regKeyBuffer )
    {
        status = ERROR_NO_SYSTEM_RESOURCES;
        goto clean;
    }
    RtlSecureZeroMemory(regKeyBuffer, regKeyBufferCb+2);

    status = StringCbPrintfW(regKeyBuffer, regKeyBufferCb+2,
        L"%s\\%s\\%s",
        NT_REG_LM, 
        REG_SERVICE_KEY,
        ServiceName);
    if ( status != 0 )
    {
        EPrint("Creating registry path failed! (0x%x)\n", status);
        goto clean;
    };
    DPrint("regKeyBuffer: %ws\n", regKeyBuffer)
    
    *RegistryPath = regKeyBuffer;
    *RegistryPathSize = regKeyBufferCb+2;

clean:
    FLeave();
    return status;
}

/**
 * Create a driver service key
 */
NTSTATUS createServiceEntry(
    _In_ LPCWSTR DriverPath,
    _In_ LPCWSTR KeyPath,
    _In_ ULONG StartType,
    _In_opt_ PWCHAR Dependencies,
    _In_ ULONG DependenciesSize
)
{
    FEnter();

    NTSTATUS status = STATUS_UNSUCCESSFUL;
    HANDLE key = NULL;
    UNICODE_STRING driverImagePath;

    RtlInitEmptyUnicodeString(&driverImagePath, NULL, 0);

    //
    // Path needs to be in nt style
    //
    status = RtlDosPathNameToNtPathName_U_WithStatus(
                DriverPath,
                &driverImagePath,
                NULL,
                NULL
            );
    if ( status != 0 )
    {
        EPrint("RtlDosPathNameToNtPathName_U failed! (0x%x)\n", status);
        return status;
    }

    //
    // create the key
    //
    status = CreateRegistryKey(
        (const PWCHAR)KeyPath,
        &key,
        KEY_ALL_ACCESS,
        REG_OPTION_NON_VOLATILE
    );
    if ( status != 0 )
    {
        EPrint("CreateRegistryKey failed! (0x%x)\n", status);
        goto clean;
    }

    //
    // fill the values
    //

    typedef struct _REG_DWORD_VALUE_DATA {
        PWCHAR Name;
        ULONG Value;
    } REG_DWORD_VALUE_DATA, *PREG_DWORD_VALUE_DATA;

    REG_DWORD_VALUE_DATA dwordData[] = {
        { L"ErrorControl", SERVICE_ERROR_NORMAL },
        { L"Type", SERVICE_KERNEL_DRIVER },
        { L"Start", StartType },
    };
    ULONG nrDwordData = RTL_NUMBER_OF(dwordData);

    // control values
    for ( ULONG i = 0; i < nrDwordData; i++ )
    {
        PREG_DWORD_VALUE_DATA d = &dwordData[i];

        status = WriteRegDWORD(key,  d->Name, d->Value);
        if ( status != 0 )
        {
            EPrint("Creating reg value \"%ws\" failed! (0x%x)", d->Name, status);
            goto clean;
        }
    }

    // path
    status = WriteRegValue(
        key, 
        REG_EXPAND_SZ,
        L"ImagePath",
        driverImagePath.Buffer,
        driverImagePath.Length + sizeof(UNICODE_NULL)
    );
    if ( status != 0 )
    {
        EPrint("Creating reg value \"ImagePath\" failed! (0x%x)", status);
        goto clean;
    }

    // display name
    //status = WriteRegValue(
    //    key, 
    //    REG_SZ,
    //    L"DisplayName",
    //    ServiceName,
    //    wcslen(ServiceName) + sizeof(UNICODE_NULL)
    //);

    if ( Dependencies )
    {
        status = WriteRegValue(
            key, 
            REG_MULTI_SZ,
            L"DependOnService",
            Dependencies,
            DependenciesSize
        );
        if ( status != 0 )
        {
            EPrint("Creating reg value \"DependOnService\" failed! (0x%x)", status);
            goto clean;
        }
    }

clean:
    if ( key )
        ZwClose(key);

    if ( driverImagePath.Buffer )
        RtlFreeUnicodeString(&driverImagePath);
    
    FLeave();
    return status;
}

/**
 * Create service registry entry
 * Load the driver
 */
NTSTATUS startDriver(
    _In_ PWCHAR RegistryPath,
    _In_ LPCWSTR DriverPath,
    _In_ BOOLEAN Reload
)
{
    FEnter();
    NTSTATUS status = 0;
    UNICODE_STRING driverServiceName;

    if ( RegistryPath == NULL )
        return STATUS_INVALID_PARAMETER_1;
    if ( DriverPath == NULL )
        return STATUS_INVALID_PARAMETER_2;

    RtlInitUnicodeString(&driverServiceName, RegistryPath);
    status = NtLoadDriver(&driverServiceName);
    DPrint("status: 0x%x\n", status);
    if ( Reload )
    {
        //printf("checking to unload\n");
        if ((status == STATUS_IMAGE_ALREADY_LOADED) ||
            (status == STATUS_OBJECT_NAME_COLLISION) ||
            (status == STATUS_OBJECT_NAME_EXISTS))
        {
            DPrint("unload\n");
            //printf("Trying to unload\n");
            status = NtUnloadDriver(&driverServiceName);
            DPrint("  status: 0x%x\n", status);
            if ( NT_SUCCESS(status) )
            {
                DPrint("load\n");
                status = NtLoadDriver(&driverServiceName);
                DPrint("  status: 0x%x\n", status);
                if ( status != 0 )
                {
                    EPrint("NtLoadDriver failed! (0x%x)\n", status);
                    //return status;
                }
            }
            else
            {
                EPrint("NtUnloadDriver failed! (0x%x)\n", status);
            }
        }
    }
    else
    {
        EPrint("NtLoadDriver failed! (0x%x)\n", status);
        if (status == STATUS_OBJECT_NAME_EXISTS)
            status = STATUS_SUCCESS;
    }

//clean:

    FLeave();
    return status;
}

NTSTATUS stopDriver(
    _In_ PWCHAR RegistryKeyPath
)
{
    FEnter();

    NTSTATUS status = STATUS_SUCCESS;
    UNICODE_STRING driverServiceName;
    
    RtlInitUnicodeString(&driverServiceName, RegistryKeyPath);
    DPrint("driverServiceName: %.*ws\n", driverServiceName.Length/2, driverServiceName.Buffer);
    status = NtUnloadDriver(&driverServiceName);
    if ( status != 0 )
    {
        EPrint("NtUnloadDriver failed! (0x%x)\n", status);
        goto clean;
    }
    DPrint("Driver unloaded!\n");
    
clean:

    FLeave();
    return status;
}

/**
 * Delete the driver's service key
 */
NTSTATUS deleteServiceKey(
    _In_ LPCWSTR KeyPath,
    _In_ LPCWSTR ServiceName
)
{
    FEnter();

    NTSTATUS status = STATUS_UNSUCCESSFUL;

    status = DeleteRegistryKey((PWCHAR)KeyPath);

    FLeave();
    return status;
}

/**
 * Check the driver's service key
 */
NTSTATUS checkServiceKey(
    _In_ LPCWSTR KeyPath,
    _In_ LPCWSTR ServiceName
)
{
    FEnter();

    NTSTATUS status = STATUS_UNSUCCESSFUL;

    DPrint("KeyPath: %ws\n", KeyPath);
    DPrint("ServiceName: %ws\n", ServiceName);

    printf("ServiceName: %ws\n", ServiceName);
    printf("-------------");
    for ( size_t i = 0; i < wcslen(ServiceName); i++ )
        printf("-");
    printf("\n");

    QUERY_KEY_FLAGS flags = {
        .PrintQueryValuesParent = 1,
        .Reserved = 0
    };
    status = QueryKeys(
                (PWCHAR)KeyPath,
                KEY_READ,
                NULL,
                printValueCb,
                flags
            );

//clean:

    FLeave();
    return status;
}

NTSTATUS printValueCb(
    _In_ HANDLE Key,
    _In_ PWCHAR Name, 
    _In_ PKEY_VALUE_FULL_INFORMATION Info
)
{
    FEnter();

    NTSTATUS status = 0;

    SIZE_T offset;
    PWCHAR str = NULL;
    SIZE_T strCb;
    ULONG infoSize = Info->DataLength + Info->DataOffset;

    DPrint("KeyName: %ws\n", Name);
    switch ( Info->Type )
    {
        case REG_SZ:
        {
            printf("  %.*ws: %.*ws\n",
                Info->NameLength/2, Info->Name,
                Info->DataLength/2, (PWCHAR)&((PUINT8)Info)[Info->DataOffset]
            );
            break;
        }
        case REG_EXPAND_SZ:
        {
            printf("  %.*ws: %.*ws\n",
                Info->NameLength/2, Info->Name,
                Info->DataLength/2, (PWCHAR)&((PUINT8)Info)[Info->DataOffset]
            );
            break;
        }
        case REG_MULTI_SZ:
        {
            offset = Info->DataOffset;
            str = (PWCHAR)&((PUINT8)Info)[offset];
            strCb = 0;
            
            printf("  %.*ws: ", Info->NameLength/2, Info->Name);

            while ( str != NULL && str[0] != 0 )
            {
                strCb = wcslen(str) * 2;
                if ( offset + strCb > infoSize )
                    break;

                printf("%ws", str);

                offset += strCb + 2;
                if ( offset > infoSize )
                    break;
                str = (PWCHAR)&((PUINT8)Info)[offset];
                
                if ( str[0] != 0 )
                    printf(", ");
            }
            printf("\n");

            break;
        }
        case REG_DWORD:
        {
            printf("  %.*ws: 0x%x\r\n",
                Info->NameLength/2, Info->Name,
                *(ULONG*)&((PUINT8)Info)[Info->DataOffset]
            );
            break;
        }
        //case REG_FULL_RESOURCE_DESCRIPTOR:
            // ...
            //break;
        //case REG_RESOURCE_LIST:
        //        printResourceList(Info);
        //    break;
        case REG_BINARY:
        default:
        {
            if ( Info->NameLength > 0 )
            {
                printf("  %.*ws (type: 0x%x): ", Info->NameLength/2, Info->Name, Info->Type);
            }
            else
            {
                printf("  (Default: 0x%x): ", Info->Type);
            }

            PUINT8 data = &((PUINT8)Info)[Info->DataOffset];
            ULONG diln = (Info->DataLength>0x100u)?0x100u:Info->DataLength;

            PrintMemCol8(data, diln, 0);

            break;
        }
    }

//clean:

    FLeave();
    return status;
}
