#ifndef __FILES_H
#define __FILES_H

#include <windows.h>



BOOL FileExists(
    PCHAR szPath
);

BOOL DirExists(
    PCHAR szPath
);



BOOL FileExists(PCHAR szPath)
{
    DWORD dwAttrib = GetFileAttributesA(szPath);

    return (dwAttrib != INVALID_FILE_ATTRIBUTES && 
            !(dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
}

BOOL DirExists(PCHAR szPath)
{
    DWORD dwAttrib = GetFileAttributesA(szPath);

    return (dwAttrib != INVALID_FILE_ATTRIBUTES && 
            (dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
}

#endif 
