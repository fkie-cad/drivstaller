#pragma once

NTSTATUS setPrivileges(
    _In_ ULONG *Privileges,
    _In_ ULONG PrivilegesCount,
    _In_ INT Enable
);
