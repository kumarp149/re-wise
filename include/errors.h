#ifndef __ERRORS__

#define __ERRORS__

typedef enum {
    ERR_NOERROR = 0,
    ERR_ZIP_NOFILE,
    ERR_ZIP_NOT_VALID,
    ERR_ZIP_NOT_ACCESSIBLE,
    ERR_UNKNOWN
} ErrorCode;

static const char* ERROR_MESSAGES[] = {
    "not an error",
    "the archive %s is not found",
    "%s is not a valid archive",
    "unable to access the archive %s",
    "unknown error occurred, %s",
};

static inline const char* error_get_message(ErrorCode code) {
    if (code >= 0 && code < ERR_UNKNOWN)
        return ERROR_MESSAGES[code];
    return ERROR_MESSAGES[ERR_UNKNOWN];
}

#endif