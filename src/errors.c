#include "errors.h"


PCHAR getErrorString(INT ErrorCode)
{
    switch ( ErrorCode )
    {
        case ERROR_INVALID_IMAGE_HASH:
            return "ERROR_INVALID_IMAGE_HASH";
        default:
            return "unknown";
    }
}
