#include "inc/nt.h"

#include "ut.h"

#include "print.h"


NTSTATUS setPrivileges(
    _In_ ULONG *Privileges,
    _In_ ULONG PrivilegesCount,
    _In_ INT Enable
)
{
    FEnter();

    ULONG retLength;
    NTSTATUS status = 0;
    HANDLE token = NULL;
    LUID luid;

    status = NtOpenProcessToken(
        NtCurrentProcess(),
        TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY,
        &token
    );
    if ( status != 0 )
    {
        EPrint("NtOpenProcessToken failed! (0x%x)\n", status);
        return status;
    }

    ULONG tokePrivsSize = (ULONG)(sizeof(TOKEN_PRIVILEGES) + (PrivilegesCount-1) * sizeof(LUID_AND_ATTRIBUTES));
    TOKEN_PRIVILEGES* tokePrivs = (PTOKEN_PRIVILEGES)malloc(tokePrivsSize);
    if ( !tokePrivs )
        return STATUS_NO_MEMORY;
    
    tokePrivs->PrivilegeCount = PrivilegesCount;
    for ( ULONG i = 0; i < PrivilegesCount; i++ )
    {
        luid = RtlConvertUlongToLuid(Privileges[i]);

        tokePrivs->Privileges[i].Luid = luid;
        tokePrivs->Privileges[i].Attributes = Enable ? SE_PRIVILEGE_ENABLED : 0;
    }

    status = NtAdjustPrivilegesToken(token,
        FALSE,
        tokePrivs,
        tokePrivsSize,
        NULL,
        &retLength
    );
    if ( status == STATUS_NOT_ALL_ASSIGNED )
    {
        EPrint("NtAdjustPrivilegesToken failed! (0x%x)\n", status);
        status = STATUS_PRIVILEGE_NOT_HELD;
    }

    if ( token )
        NtClose(token);
    if ( tokePrivs )
        free(tokePrivs);
    
    FLeave();
    return status;
}
