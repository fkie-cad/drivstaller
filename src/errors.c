#include "errors.h"


PCHAR getErrorString(INT ErrorCode)
{
    switch ( ErrorCode )
    {
        case ERROR_BAD_UNIT:
            return "ERROR_BAD_UNIT: The system cannot find the device specified.";
        case ERROR_GEN_FAILURE:
            return "ERROR_GEN_FAILURE: A device attached to the system is not functioning.";
        case ERROR_INVALID_IMAGE_HASH:
            return "ERROR_INVALID_IMAGE_HASH: The hash for the image cannot be found in the system catalogs.";
        case CERT_E_REVOKED:
            return "CERT_E_REVOKED: A certificate was explicitly revoked by its issuer.";
        default:
            return "unknown";
    }
}
