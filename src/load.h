#pragma once

#define MODE_NONE    (0x00)
#define MODE_INSTALL (0x01)
#define MODE_REMOVE  (0x02)
#define MODE_START   (0x03)
#define MODE_STOP    (0x04)
#define MODE_CHECK   (0x08)
//#define MODE_MAX   (MODE_CHECK)



INT ManageNtLoader(
    _In_ PCHAR ServiceNameA,
    _In_ PCHAR DriverFullPathA,
    _In_ ULONG StartType,
    _In_opt_ PWCHAR Dependencies,
    _In_ ULONG DependenciesSize,
    _In_ USHORT Mode
);
