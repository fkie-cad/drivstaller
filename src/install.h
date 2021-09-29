#ifndef DRIVSTALLER_EXE_INSTALL_H
#define DRIVSTALLER_EXE_INSTALL_H

#include <windows.h>

#define MODE_INSTALL (0x01)
#define MODE_REMOVE  (0x02)
#define MODE_START   (0x03)
#define MODE_STOP    (0x04)

BOOL
ManageDriver(
    _In_ LPCTSTR Name,
    _In_ LPCTSTR ServiceExe,
    _In_ DWORD StartType,
    _In_ USHORT Mode
);

#endif
