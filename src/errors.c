#include "errors.h"


PCHAR getErrorString(INT ErrorCode)
{
    switch ( ErrorCode )
    {
        case ERROR_BAD_UNIT:
            return "ERROR_BAD_UNIT: The system cannot find the device specified.";
        case ERROR_INVALID_IMAGE_HASH:
            return "ERROR_INVALID_IMAGE_HASH: The hash for the image cannot be found in the system catalogs.";
        default:
            return "unknown";
    }
}
