#include "registry.h"
#include "print.h"

#define RING3
#ifdef RING3
#include "strsafe.h"

#define ExAllocatePoolWithTag(_pt_, _n_, _t_) malloc(_n_)
#define ExFreePool(_p_) free(_p_)
#define RtlStringCbPrintfW StringCbPrintfW
#endif



NTSTATUS CreateRegistryKey(_In_ CONST PWCHAR Path, _Out_ HANDLE* Key, _In_ ULONG DesiredAccess, _In_ ULONG CreateOptions)
{
    NTSTATUS status = STATUS_SUCCESS;
    UNICODE_STRING regPath;
    ULONG oaflags = OBJ_CASE_INSENSITIVE;
    #ifndef RING3
    oaflags |= OBJ_KERNEL_HANDLE;
    #endif

    RtlInitUnicodeString(&regPath, Path);

    // open key
    OBJECT_ATTRIBUTES oa;
    RtlZeroMemory(&oa, sizeof(OBJECT_ATTRIBUTES));
    InitializeObjectAttributes(&oa, &regPath, oaflags, NULL, NULL); 

    status = ZwCreateKey(
                Key, 
                DesiredAccess, 
                &oa, 
                0, 
                NULL, 
                CreateOptions, 
                NULL
            );
    if ( !NT_SUCCESS(status) ) 
    {
        EPrint("ERROR (0x%x): Can't open or create key!", status);
        return status;
    }
    
    return status;
}

NTSTATUS OpenRegistryKey(_In_ CONST PWCHAR Path, _Out_ HANDLE* Key, _In_ ULONG DesiredAccess)
{
    NTSTATUS status = STATUS_SUCCESS;
    UNICODE_STRING RegPath;
    ULONG oaflags = OBJ_CASE_INSENSITIVE;
    #ifndef RING3
    oaflags |= OBJ_KERNEL_HANDLE;
    #endif

    RtlInitUnicodeString(&RegPath, Path);

    // open key
    OBJECT_ATTRIBUTES oa;
    RtlZeroMemory(&oa, sizeof(OBJECT_ATTRIBUTES));
    InitializeObjectAttributes(&oa, &RegPath, oaflags, NULL, NULL); 

    status = ZwOpenKeyEx(
                Key, 
                DesiredAccess, 
                &oa, 
                0
            );
    if ( status != 0 ) 
    {
        EPrint("Can't open key \"%ws\"! (0x%x)\n", Path, status);
        return status;
    }

    return status;
}

NTSTATUS WriteRegDWORD(_In_ HANDLE Key, _In_ PWCHAR valueName, _In_ ULONG data)
{
    NTSTATUS status = STATUS_SUCCESS;
    UNICODE_STRING Name; 

    RtlInitUnicodeString(&Name, valueName); 
    
    status = ZwSetValueKey(
                Key, 
                &Name, 
                0, 
                REG_DWORD, 
                &data, 
                sizeof(ULONG)
            ); 

    return status;
}

NTSTATUS ReadRegDWORD(_In_ HANDLE Key, _In_ PWCHAR valueName, _Out_ ULONG* Data)
{
    NTSTATUS status = STATUS_SUCCESS;
    ULONG Size = REG_KEY_PART_INFO_DWORD_SIZE;
    UNICODE_STRING Name; 
    UCHAR buffer[REG_KEY_PART_INFO_DWORD_SIZE];

    RtlInitUnicodeString(&Name, valueName); 
    
    RtlZeroMemory(buffer, Size);
    PKEY_VALUE_PARTIAL_INFORMATION pi = (PKEY_VALUE_PARTIAL_INFORMATION)buffer;
    
    status = ZwQueryValueKey(
        Key, 
        &Name, 
        KeyValuePartialInformation, 
        pi, 
        Size, 
        &Size
    ); 
    if ( !NT_SUCCESS(status) ) 
    {
        EPrint("ERROR (0x%x): Could not read key!\n", status);
        return status;
    }
    if ( pi->Type != REG_DWORD )
    {
        status = STATUS_OBJECT_TYPE_MISMATCH;
        DPrint("ERROR (0x%x): ZwQueryValueKey DWORD failed!\n", status);
        return status;
    }

    *Data = *(ULONG*)&pi->Data[0];

    return status;
}

NTSTATUS WriteRegQWORD(_In_ HANDLE Key, _In_ PWCHAR valueName, _In_ UINT64 data)
{
    NTSTATUS status = STATUS_SUCCESS;
    UNICODE_STRING Name; 

    RtlInitUnicodeString(&Name, valueName); 
    
    status = ZwSetValueKey(
                Key, 
                &Name, 
                0, 
                REG_QWORD, 
                &data, 
                sizeof(UINT64)
            ); 

    return status;
}

NTSTATUS ReadRegQWORD(_In_ HANDLE Key, _In_ PWCHAR valueName, _Out_ UINT64* Data)
{
    NTSTATUS status = STATUS_SUCCESS;
    ULONG Size = REG_KEY_PART_INFO_QWORD_SIZE;
    UNICODE_STRING Name; 
    UCHAR buffer[REG_KEY_PART_INFO_QWORD_SIZE];

    RtlInitUnicodeString(&Name, valueName); 
    
    RtlZeroMemory(buffer, Size);
    PKEY_VALUE_PARTIAL_INFORMATION pi = (PKEY_VALUE_PARTIAL_INFORMATION)buffer;
    
    status = ZwQueryValueKey(
        Key, 
        &Name, 
        KeyValuePartialInformation, 
        pi, 
        Size, 
        &Size
    ); 
    if ( !NT_SUCCESS(status) ) 
    {
        EPrint("ERROR (0x%x): ZwQueryValueKey QWORD failed!\n", status);
        return status;
    }
    if ( pi->Type != REG_QWORD )
    {
        status = STATUS_OBJECT_TYPE_MISMATCH;
        EPrint("ERROR (0x%x): ZwQueryValueKey QWORD failed!\n", status);
        return status;
    }

    *Data = *(UINT64*)&pi->Data[0];

    return status;
}

NTSTATUS WriteRegSZ(_In_ HANDLE Key, _In_ PWCHAR valueName, _In_ PWCHAR Sz, _In_ ULONG  SzCb)
{
    return WriteRegValue(Key, REG_SZ, valueName, Sz, SzCb);
}

//NTSTATUS ReadRegSZ(_In_ HANDLE Key, _In_ PWCHAR valueName, _Out_ PCHAR* data, _Inout_ PULONG size)
//{
//    return ReadRegData(Key, valueName, data, size);
//}

NTSTATUS WriteRegBinary(_In_ HANDLE Key, _In_ PWCHAR valueName, _In_ PVOID Data, _In_ ULONG Size)
{
    return WriteRegValue(Key, REG_BINARY, valueName, Data, Size);
}

NTSTATUS WriteRegValue(_In_ HANDLE Key, _In_ ULONG Type, _In_ PWCHAR valueName, _In_ PVOID Data, _In_ ULONG Size)
{
    NTSTATUS status = STATUS_SUCCESS;
    UNICODE_STRING Name; 

    RtlInitUnicodeString(&Name, valueName); 
    
    status = ZwSetValueKey(
                Key, 
                &Name, 
                0, 
                Type, 
                Data, 
                Size
            ); 

    return status;
}

NTSTATUS ReadRegData(
    _In_ HANDLE Key, 
    _In_ PWCHAR valueName, 
    _Inout_ PKEY_VALUE_PARTIAL_INFORMATION* pdata, 
    _Inout_ PULONG size
)
{
    NTSTATUS status = STATUS_SUCCESS;
    ULONG RequiredSize;
    UNICODE_STRING Name; 
    RtlInitUnicodeString(&Name, valueName); 
    PKEY_VALUE_PARTIAL_INFORMATION data;
    
    if ( pdata == NULL )
    {
        status = STATUS_INVALID_PARAMETER_3;
        EPrint("ReadRegData failed! (0x%x)\n", status);
        return status;
    }
    else  data = *pdata;


    status = ZwQueryValueKey(
        Key, 
        &Name, 
        KeyValuePartialInformation, 
        NULL, 
        0, 
        &RequiredSize
    ); 
    if ( !NT_SUCCESS(status) && status != STATUS_BUFFER_TOO_SMALL ) 
    {
        EPrint("ERROR (0x%x): Could not query key!\n", status);
        return status;
    }

    if ( data == NULL )
    {
        data = (PKEY_VALUE_PARTIAL_INFORMATION)ExAllocatePoolWithTag(PagedPool, RequiredSize, 'ygeR');
        if ( data == NULL )
        {
            status = STATUS_INSUFFICIENT_RESOURCES;
            EPrint("Error (0x%x): allocate reg query buffer\n", status);
            data = NULL;
            *size = 0;
            return status;
        }
    }
    else
    {
        if ( *size < RequiredSize )
        {
            if ( data == NULL )
            {
                status = STATUS_BUFFER_TOO_SMALL;
                EPrint("ERROR (0x%x): data[0x%x] too small, 0x%x needed\n", status, *size, RequiredSize);
                return status;
            }
        }
    }
    
    status = ZwQueryValueKey(
        Key, 
        &Name, 
        KeyValuePartialInformation, 
        data, 
        RequiredSize, 
        &RequiredSize
    ); 
    if ( !NT_SUCCESS(status) ) 
    {
        EPrint("ERROR (0x%x): Could not query key!", status);
        return status;
    }

    *size = RequiredSize;
    *pdata = data;

    return status;
}
 
NTSTATUS DeleteRegistryKey(
    _In_ PWCHAR KeyPath
)
{
    FEnter();
    NTSTATUS status = STATUS_SUCCESS;
    
    QUERY_KEY_FLAGS flags = { 0 };
    status = QueryKeys(
        KeyPath,
        KEY_ALL_ACCESS,
        deleteKeyCb,
        deleteValueCb,
        flags
    );

    FLeave();
    return status;
}

/**
 * Query all keys and subkey and call callbacks on each value and subkey
 */
NTSTATUS
QueryKeys(
    _In_ PWCHAR KeyPath,
    _In_ ULONG DesiredAccess,
    _In_opt_ REG_KEY_CALLBACK RegKeyCallback,
    _In_opt_ REG_VALUE_CALLBACK RegValueCallback,
    _In_ QUERY_KEY_FLAGS Flags
)
{
    FEnter();

    NTSTATUS status = STATUS_SUCCESS;

    HANDLE key = NULL;
    PUINT8 szBuffer = NULL;
    ULONG szBufferSize = PAGE_SIZE;

    LIST_ENTRY keyObjHead = {0};
    PKEY_OBJ firstKeyObj = NULL;
    SIZE_T firstKeyNameCb = 0;
    PKEY_OBJ actKeyObj = NULL;
    PLIST_ENTRY actKeyLink = NULL;

    DPrint("  KeyPath: %ws\n", KeyPath);

    //
    // init list
    //
    InitializeListHead(&keyObjHead);

    firstKeyNameCb = wcslen(KeyPath) * 2;
    firstKeyObj = ExAllocatePoolWithTag(PagedPool, firstKeyNameCb+sizeof(KEY_OBJ), 0);
    if ( !firstKeyObj )
    {
        status = STATUS_INSUFFICIENT_RESOURCES;
        goto clean;
    }
    RtlStringCbPrintfW(firstKeyObj->Name, firstKeyNameCb+2, L"%s", KeyPath);
    firstKeyObj->NameSize = (ULONG)firstKeyNameCb+2;
    firstKeyObj->Link.Flink = NULL;
    firstKeyObj->Link.Blink = NULL;
    *(PUINT32)&firstKeyObj->Flags = 0;
    
    InsertTailList(&keyObjHead, &firstKeyObj->Link);

    //
    // preallocate registry key info buffer
    //
    szBuffer = ExAllocatePoolWithTag(PagedPool, szBufferSize, 0);
    if ( !szBuffer )
    {
        status = STATUS_INSUFFICIENT_RESOURCES;
        ExFreePool(firstKeyObj);
        firstKeyObj = NULL;
        goto clean;
    }
    
    //
    // iterate list
    //
    while ( !IsListEmpty(&keyObjHead) )
    {
        // first peek the tail but don't remove it
        actKeyLink = PeekTailList(&keyObjHead);
        if ( actKeyLink == NULL )
            break;
        actKeyObj = CONTAINING_RECORD(actKeyLink, KEY_OBJ, Link);
        
        //
        // open key
        //
        if ( key != NULL )
        {
            ZwClose(key);
            key = NULL;
        }
        status = OpenRegistryKey(actKeyObj->Name, &key, DesiredAccess);
        if ( !NT_SUCCESS(status) )
        {
            EPrint("OpenRegistryKey \"%ws\" failed! (0x%x)\n", actKeyObj->Name, status);
            
            // if it can't be opened remove it
            actKeyLink = RemoveTailList(&keyObjHead);
            ExFreePool(actKeyObj);
            actKeyObj = NULL;

            // and continue
            continue;
        }

        //
        // check the status of act list obj
        //
        if ( actKeyObj->Flags.Visited )
        {
            // if it already has been visited
            // there should not be any subkeys any more
            // and it can be handled
            
            //
            // iterate values
            //
            if ( RegValueCallback )
            {
                status = QueryValues(key, actKeyObj->Name, &szBuffer, &szBufferSize, RegValueCallback, Flags);
            }

            //
            // when done
            // call callback on this key
            if ( RegKeyCallback )
            {
                status = RegKeyCallback(key, actKeyObj->Name);
            }
            
            // and remove from list
            //
            actKeyLink = RemoveTailList(&keyObjHead);
            ExFreePool(actKeyObj);
            actKeyObj = NULL;
        }
        else
        {
            //
            // if it's the first time this key is visited

            // write subkeys into list
            //
            status = QuerySubKeys(actKeyObj->Name, key, &szBuffer, &szBufferSize, &keyObjHead, Flags);
            //if ( !NT_SUCCESS(status) || szBuffer == NULL )
            //    goto clean;
            
            // mark as visited
            actKeyObj->Flags.Visited = 1;
        }
    }

clean:
    if ( szBuffer != NULL )
        ExFreePool(szBuffer);
    if ( key != NULL )
        ZwClose(key);

    FLeave();
    return status;
}

NTSTATUS
QuerySubKeys(
    _In_ PWCHAR KeyName,
    _In_ HANDLE Key,
    _Inout_ PUINT8 *Buffer,
    _Inout_ PULONG BufferSize,
    _In_ PLIST_ENTRY keyNamesHead,
    _In_ QUERY_KEY_FLAGS Flags
)
{
    FEnter();

    NTSTATUS status = STATUS_SUCCESS;
    
    ULONG written = 0;

    PKEY_OBJ subKeyObj = NULL;
    SIZE_T subKeyNameCb = 0;

    //ASSERT(Buffer != NULL);
    //ASSERT(BufferSize != NULL);
    if ( *Buffer == NULL || *BufferSize == sizeof(KEY_BASIC_INFORMATION) )
    {
        status = STATUS_INVALID_PARAMETER;
        EPrint("Buffer is NULL or too small! (0x%x)\n", status);
        goto clean;
    }
    

    DPrint("Query sub keys:\n");

    ULONG i = 0;
    PKEY_BASIC_INFORMATION keyInfo = (PKEY_BASIC_INFORMATION)(*Buffer);
    while ( i < MAX_KEY_ITERATION_COUNT )
    {
        // happy prefast check
        if ( *Buffer == NULL )
            break;

        RtlZeroMemory((*Buffer), (*BufferSize));
        status = ZwEnumerateKey(
                    Key,
                    i,
                    //KeyFullInformation,
                    KeyBasicInformation,
                    keyInfo,
                    (*BufferSize),
                    &written
                );
        
        if ( status == STATUS_NO_MORE_ENTRIES )
        {
            status = STATUS_SUCCESS;
            break;
        }
        if ( status == STATUS_BUFFER_TOO_SMALL || status == STATUS_BUFFER_OVERFLOW )
        {
            DPrint("buffer too small: 0x%x < 0x%x\n", (*BufferSize), keyInfo->NameLength);
            (*BufferSize) = keyInfo->NameLength + sizeof(KEY_BASIC_INFORMATION);
            ExFreePool((*Buffer));
            (*Buffer) = ExAllocatePoolWithTag(PagedPool, (*BufferSize), 0);
            if ( !(*Buffer) )
            {
                status = STATUS_INSUFFICIENT_RESOURCES;
                goto clean;
            }
            RtlZeroMemory((*Buffer), (*BufferSize));
            keyInfo = (PKEY_BASIC_INFORMATION)(*Buffer);
            status = ZwEnumerateKey(
                    Key,
                    i,
                    //KeyFullInformation,
                    KeyBasicInformation,
                    keyInfo,
                    (*BufferSize),
                    &written
                );
        }

        if ( !NT_SUCCESS(status) || keyInfo == NULL )
        {
            i++;
            continue;
        }

        DPrint("[Key %u]: %.*ws\n", i, (USHORT)(keyInfo->NameLength/2), keyInfo->Name);

        subKeyNameCb = wcslen(KeyName)*2;
        subKeyNameCb += 4 + keyInfo->NameLength; // + L"\\" + L\0
        subKeyObj = ExAllocatePoolWithTag(PagedPool, subKeyNameCb+sizeof(KEY_OBJ), 0);
        if ( !subKeyObj )
        {
            status = STATUS_INSUFFICIENT_RESOURCES;
            goto clean;
        }
        RtlStringCbPrintfW(subKeyObj->Name, subKeyNameCb, L"%ws\\%.*ws", KeyName, keyInfo->NameLength/2, keyInfo->Name);
        subKeyObj->NameSize = (ULONG)subKeyNameCb;
        subKeyObj->Link.Flink = NULL;
        subKeyObj->Link.Blink = NULL;
        *(PUINT32)&subKeyObj->Flags = 0;
    
        InsertTailList(keyNamesHead, &(subKeyObj->Link));

        i++;
    }

clean:
    
    FLeave();
    return status;
}

NTSTATUS
QueryValues(
    _In_ HANDLE Key,
    _In_ PWCHAR KeyPath,
    _Inout_ PVOID *Buffer,
    _Inout_ PULONG BufferSize,
    _In_ REG_VALUE_CALLBACK ValueCallback,
    _In_ QUERY_KEY_FLAGS Flags
)
{
    FEnter();

    NTSTATUS status = STATUS_SUCCESS;
    
    ULONG i;
    ULONG written = 0;

    DPrint("KeyName: %ws\n", KeyPath);

    if ( *Buffer == NULL || *BufferSize == sizeof(KEY_VALUE_FULL_INFORMATION) )
    {
        status = STATUS_INVALID_PARAMETER;
        EPrint("Buffer is NULL or too small! (0x%x)\n", status);
        goto clean;
    }

    //
    // iterate all values
    //

    if ( Flags.PrintQueryValuesParent )
    {
        printf("Key: %ws\n", KeyPath);
        printf("-----");
        for ( size_t kpi = 0; kpi < wcslen(KeyPath); kpi++ )
            printf("-");
        printf("\n");
    }

    i = 0;
    PKEY_VALUE_FULL_INFORMATION valueInfo = (PKEY_VALUE_FULL_INFORMATION)(*Buffer);
    while ( i < MAX_VALUE_ITERATION_COUNT && (*Buffer) )
    {
        RtlZeroMemory((*Buffer), (*BufferSize));
        status = ZwEnumerateValueKey(Key, i, KeyValueFullInformation, valueInfo, (*BufferSize), &written);
        if ( status == STATUS_NO_MORE_ENTRIES )
        {
            DPrint("STATUS_NO_MORE_ENTRIES\n");
            status = 0;
            break;
        }
        if ( status == STATUS_BUFFER_TOO_SMALL || status == STATUS_BUFFER_OVERFLOW )
        {
            while ( status == STATUS_BUFFER_TOO_SMALL || status == STATUS_BUFFER_OVERFLOW )
            {
                DPrint("buffer too small: 0x%x < 0x%x\n", (*BufferSize), valueInfo->DataLength);
                ExFreePool((*Buffer));
                *Buffer = NULL;
                //(*BufferSize) = valueInfo->DataLength + sizeof(KEY_VALUE_FULL_INFORMATION);
                (*BufferSize) += PAGE_SIZE;
                DPrint("new buffer size: 0x%x\n", (*BufferSize));
                (*Buffer) = ExAllocatePoolWithTag(PagedPool, (*BufferSize), 0);
                if ( !(*Buffer) )
                {
                    status = STATUS_INSUFFICIENT_RESOURCES;
                    goto clean;
                }
                RtlZeroMemory((*Buffer), (*BufferSize));
                valueInfo = (PKEY_VALUE_FULL_INFORMATION)(*Buffer);
                status = ZwEnumerateValueKey(Key, i, KeyValueFullInformation, valueInfo, (*BufferSize), &written);
            }
        }
        
        if ( !NT_SUCCESS(status) || valueInfo == NULL )
        {
            EPrint("ZwEnumerateValueKey failed! (0x%x)\n", status);
            i++;
            continue;
        }
        
        ValueCallback(Key, KeyPath, valueInfo);

        i++;
    }

    if ( Flags.PrintQueryValuesParent )
    {
        printf("\n");
    }

clean:

    FLeave();
    return status;
}

NTSTATUS deleteKeyCb(
    _In_ HANDLE Key,
    _In_ PWCHAR Name
)
{
    FEnter();

    NTSTATUS status = STATUS_SUCCESS;

    DPrint("KeyName: %ws\n", Name);
    status = ZwDeleteKey(Key);

    FLeave();
    return status;
}

NTSTATUS deleteValueCb(
    _In_ HANDLE Key,
    _In_ PWCHAR Name, 
    _In_ PKEY_VALUE_FULL_INFORMATION Info
)
{
    FEnter();

    NTSTATUS status = STATUS_SUCCESS;

    if ( Info->Name[0] == 0 || Info->NameLength == 0 )
        return status;

    UNICODE_STRING keyName = { 0 };
    RtlInitUnicodeString(&keyName, &Info->Name[0]);
    
    DPrint("KeyName: %ws\n", Name);
    status = ZwDeleteValueKey(Key, &keyName);

    FLeave();
    return status;
}
