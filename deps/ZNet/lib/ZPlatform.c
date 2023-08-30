//#include <time.h>

#include <math.h>

#include "ZPlatform.h"

#define MAX_SYSERR 256

//----------------------------------------------------------------------------
char* ICACHE_FLASH_ATTR sys_last_err_string() {
    char* result = NULL; /*(char*)zmalloc( MAX_SYSERR * sizeof(char) );*/
    if( result == NULL )
        return NULL;

#if( defined( _WINDOWS ) )
    uint32_t err = GetLastError();
    if( err == 0 )
        return NULL;

    uint32_t r = FormatMessage( FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                                NULL,
                                err,
                                MAKELANGID( LANG_NEUTRAL, SUBLANG_DEFAULT ),
                                result,
                                MAX_SYSERR,
                                NULL );
    if( r == 0 ) {
        // sprintf(result, "unknown error %lu", err);
    }
#else
#if( defined( __APPLE__ ) || defined( __linux__ ) )
    int err = errno;
    if( err == 0 )
        return NULL;

    int r = strerror_r( err, result, MAX_SYSERR );
    if( r ) {
        // sprintf(result, "unknown error %d", err);
    }
#else
#endif
#endif

    return result;
}

//----------------------------------------------------------------------------
int ICACHE_FLASH_ATTR remove_recursive( const char* UNUSED( dirname ) ) {
    /*
if( dirname == NULL ) { errno = EINVAL; return -1; }
int len = str_length(dirname); if( len == 0 ) { errno = EINVAL; return -1; }

char abs_filename[FILENAME_MAX];
struct dirent* ep;

DIR *dp = opendir(dirname); if( dp == NULL ) return -1;

str_copy(abs_filename, dirname);
char* name = abs_filename + len;
if( abs_filename[len - 1] != '/' ) {
    str_cat(abs_filename, "/");
    len++;
}

while( (ep = readdir(dp)) ) {
    struct stat stFileInfo;

    strncpy(name, ep->d_name, sizeof(abs_filename) - len - 1);

    //if( lstat(abs_filename, &stFileInfo) < 0 ) continue;
    if( stat(abs_filename, &stFileInfo) < 0 ) continue;

    if( S_ISDIR(stFileInfo.st_mode) ) {
        if( str_compare(ep->d_name, ".") && str_compare(ep->d_name, "..") ) {
            (void)remove_recursive(abs_filename); // if error occurs, last
remove() will throw error
        }
    }
    else
    {
        (void)remove(abs_filename); // if error occurs, last remove() will throw
error
    }
}

(void)closedir(dp);

return remove(dirname);
*/
    return ( -1 );
}

#ifdef _WINDOWS
/*
//----------------------------------------------------------------------------
BOOL
WINAPI
DllMain(HINSTANCE hinstDLL, uint32_t fdwReason, LPVOID lpvReserved)
{
    return TRUE;
}
*/
/*
//----------------------------------------------------------------------------
int
ICACHE_FLASH_ATTR
asprintf(char** buffer, const char* format, ...)
{
        va_list args;
        va_start(args, format);

        int r = vasprintf(buffer, format, args);

        va_end(args);

        return r;
}
*/
/*
//----------------------------------------------------------------------------
int
ICACHE_FLASH_ATTR
vasprintf(char** buffer, const char* format, va_list args)
{
        int len = _vscprintf(format, args) + 1; // extra room for terminating
'\0' *buffer = (char*)malloc(len * sizeof(char)); if( *buffer == NULL ) return
-1; return vsprintf(*buffer, format, args);
}
*/

/*
//----------------------------------------------------------------------------
float
ICACHE_FLASH_ATTR
roundf(float num) /// TODO ICACHE_FLASH_ATTR ???
{
        float integer = ceilf(num);
    if( num > 0 )return integer - num > 0.5f ? integer - 1.0f : integer;
    return integer - num >= 0.5f ? integer - 1.0f : integer;
}

//----------------------------------------------------------------------------
double
ICACHE_FLASH_ATTR
round(double num) /// TODO ICACHE_FLASH_ATTR ???
{
    double integer = ceil(num);
    if( num > 0 ) return integer - num > 0.5 ? integer - 1.0 : integer;
    return integer - num >= 0.5 ? integer - 1.0 : integer;
}
*/
/*
//----------------------------------------------------------------------------
void
ICACHE_FLASH_ATTR
flockfile( FILE *filehandle )
{
    HANDLE file = (HANDLE)_get_osfhandle(_fileno(filehandle));
    LockFile(file, 0, 0, 0xffffffff, 0xffffffff);
}
*/
/*
void
ICACHE_FLASH_ATTR
funlockfile( FILE *filehandle )
{
    HANDLE file = (HANDLE)_get_osfhandle(_fileno(filehandle));
    UnlockFile(file, 0, 0, 0xffffffff, 0xffffffff);
}
*/
#endif
