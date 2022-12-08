#include "znet_lib_info.h"

#include <stdio.h>

#define ZNET_LIB_VERSION_MAJOR 1
#define ZNET_LIB_VERSION_MINOR 0

const char* znetRevisionTag() {
    return ZNET_REVISION_TAG;
}

const char* znetRevisionId() {
    return ZNET_REVISION_ID;
}

int znetVersion( char* buff, int bufSize ) {
    int len =
        snprintf( buff, bufSize - 1, "%d.%d.%s", ZNET_LIB_VERSION_MAJOR, ZNET_LIB_VERSION_MINOR, ZNET_REVISION_ID );
    buff[len] = '\0';
    return len;
}

const char* znetCommitId() {
    return ZNET_GIT_ID;
}

const char* znetCommitDate() {
    return ZNET_DATE;
}
