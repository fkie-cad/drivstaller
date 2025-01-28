#ifndef DRIVSTALLER_EXE_INSTALL_H
#define DRIVSTALLER_EXE_INSTALL_H

#include <windows.h>

#define MODE_NONE    (0x00)
#define MODE_INSTALL (0x01)
#define MODE_REMOVE  (0x02)
#define MODE_START   (0x03)
#define MODE_STOP    (0x04)
#define MODE_CHECK   (0x08)
#define MODE_MAX   (MODE_CHECK)

BOOL
ManageDriver(
    _In_ LPCTSTR Name,
    _In_ LPCTSTR ServiceExe,
    _In_ DWORD StartType,
    _In_opt_ PCHAR Dependencies,
    _In_ USHORT Mode,
    _In_ ULONG DesiredAccess
);

#endif
