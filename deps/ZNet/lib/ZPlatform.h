
#ifndef __ZNET_PLATFORM_H__
#define __ZNET_PLATFORM_H__

#include "typedefs.h"

#if( defined( _WINDOWS ) || defined( __APPLE__ ) || defined( __linux__ ) )
#include <stdio.h>
#include <errno.h>
#include <dirent.h>
//#include <time.h>
#endif
#include "ZErrors.h"

#ifdef _WINDOWS
#define ZWINLINE __inline
#else
#define ZWINLINE inline
#endif

// wrapper for unused variables
#ifdef __GNUC__
#define UNUSED( x ) UNUSED_##x __attribute__( ( __unused__ ) )
#else
#define UNUSED( x ) UNUSED_##x
#endif

#ifdef _WINDOWS

#include <Windows.h>
#include <shlwapi.h>
#include <io.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <WinSock.h>
#include <stdint.h>
//#include <time.h>
#include <tchar.h>

#define sleep( sec ) Sleep( (sec)*1000 )
#define sleep_ms( ms ) Sleep( ms )

struct timezone {
    int tz_minuteswest; /* minutes W of Greenwich */
    int tz_dsttime;     /* type of dst correction */
};

#define O_NOCTTY 0

#define str_copy( s1, s2 ) strcpy( s1, s2 )
#define str_duplicate( s ) strdup( s )
#define str_compare( s1, s2 ) strcmp( s1, s2 )
#define str_length( s ) strlen( s )
#define str_cat( s1, s2 ) strcat( s1, s2 )
#define str_ncat( s1, s2, n ) strncat( s1, s2, n )

#else

//#include <sys/time.h>

#if( defined( __APPLE__ ) || defined( __linux__ ) )

#if( defined( __i386__ ) || defined( __x86_64__ ) )
#define __sd_linux_use_directIO__ 1
#ifdef __APPLE__
#include <sys/uio.h>
#else
#include <sys/io.h>
#endif
#endif

#include <fcntl.h>
#include <sys/socket.h>
#include <netdb.h>
#include <sys/errno.h>
#include <termios.h>
#include <memory.h>

#include <unistd.h>
#include <pthread.h>
#include <sys/stat.h>

#ifndef __USE_UNIX98
#define __USE_UNIX98  // to workaround PTHREAD_MUTEX_RECURSIVE and
                      // PTHREAD_MUTEX_RECURSIVE_NP
#endif

//#ifndef O_BINARY
//	#define O_BINARY 0
//#endif

#define SOCKET_ERROR -1

#define MAX_PATH FILENAME_MAX
#define closesocket( x ) close / ( x )
#define _sleep( x ) usleep( (x)*1000 )

#define str_copy( s1, s2 ) strcpy( s1, s2 )
#define str_duplicate( s ) strdup( s )
#define str_compare( s1, s2 ) strcmp( s1, s2 )
#define str_length( s ) strlen( s )
#define str_cat( s1, s2 ) strcat( s1, s2 )
#define str_ncat( s1, s2, n ) strncat( s1, s2, n )

#define sleep_ms( ms )                          \
    do {                                        \
        struct timeval wait = { 0, ms * 1000 }; \
        select( 0, NULL, NULL, NULL, &wait );   \
    } while( 0 )
#else
#if defined( ESP_PLATFORM )
#include <string.h>
/// TODO: investigate

#define str_length( s ) strlen( s )

#else
// ESP8266 ?
#include <string.h>
#include <osapi.h>
#include "espmissingincludes.h"

//#define copy_str(s)            strdup(s)
#define str_copy( s1, s2 ) strcpy( s1, s2 )
#define str_duplicate( s ) strdup( s )
#define str_compare( s1, s2 ) strcmp( s1, s2 )
#define str_length( s ) strlen( s )
#define str_cat( s1, s2 ) strcat( s1, s2 )
#define str_ncat( s1, s2, n ) strncat( s1, s2, n )

//#define sleep_m s(ms) os_delay_us( ms * 1000 )

#endif
#endif
#endif

#ifdef __cplusplus
extern "C" {
#endif

char* ICACHE_FLASH_ATTR sys_last_err_string();

//__attribute__((visibility("default")))
int ICACHE_FLASH_ATTR remove_recursive( const char* dirname );

#ifdef __cplusplus
}
#endif

#endif  // #ifndef __ZNET_PLATFORM_H__
