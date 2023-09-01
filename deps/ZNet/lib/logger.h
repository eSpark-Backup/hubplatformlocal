
#ifndef __ZNET_LOGGING_H__
#define __ZNET_LOGGING_H__

//#include "typedefs.h"
#include <stdint.h>

#if( defined( _WINDOWS ) )
#include <stdio.h>
#else
#if( defined( __APPLE__ ) || defined( __linux__ ) || defined( ESP_PLATFORM ) )
#include "znet_log.h"
#else
#include <osapi.h>
#include "espmissingincludes.h"
#endif  // ( defined(__APPLE__) ||  defined( __linux__ ) || defined( ESP_PLATFORM ) )
#endif  // ( defined( _WINDOWS) )

//#define ZNET_TRACE_DATA 1

// void ICACHE_FLASH_ATTR znet_log(const  ZWLogLevel level, const
// char* message, ...);

// const char* ICACHE_FLASH_ATTR znet_strerror(ZWError err);

// void ICACHE_FLASH_ATTR znet_log_error(const  ZWLogLevel level,
// const char* message, ZWError err);

// void ICACHE_FLASH_ATTR znet_dump(const  ZWLogLevel level, const
// char* prefix, int length, const uint8_t* buffer);

#ifdef DEBUG
// void ICACHE_FLASH_ATTR znet_debug_log_error(const  ZWError err,
// const char* func, const char* file, int line); #define LOG_ERR(r)
// znet_debug_log_error( znet, r, #r, __FILE__, __LINE__ )
#else
// void ICACHE_FLASH_ATTR znet_debug_log_error(const  ZWError err,
// const char* func); #define LOG_ERR(r)	znet_debug_log_error( znet, r,
// #r
// )
#endif  // DEBUG

typedef struct _Log {
    int _dummy;

} Log_t;
typedef Log_t* Log;

// bool ICACHE_FLASH_ATTR Log_Init( void );

// void ICACHE_FLASH_ATTR Log_Info   ( const Log log, const char* str );
// void ICACHE_FLASH_ATTR Log_Warning( const Log log, const char* str );
// void ICACHE_FLASH_ATTR Log_Error  ( const Log log, const char* str );

// void ICACHE_FLASH_ATTR Log_Write( const char* str, ... );

#if( defined( _WINDOWS ) )

//#define Log_Write( log, fmt, ... )  do{ fprintf( stdout, fmt, ##__VA_ARGS__ );
//}while(0)
#define Log_Write( fmt, ... ) fprintf( stdout, fmt, ##__VA_ARGS__ )

#else
#if( defined( __APPLE__ ) || defined( __linux__ ) || defined( ESP_PLATFORM ) )

#define Log_Write ZNET_LOGD

#else

#define Log_Write( fmt, ... )                                              \
    do {                                                                   \
        static const char flash_str[] ICACHE_RODATA_ATTR STORE_ATTR = fmt; \
        os_printf_plus( flash_str, ##__VA_ARGS__ );                        \
    } while( 0 )
//	#define Log_Write( fmt, ... )  do{ os_printf( fmt, ##__VA_ARGS__ );
//}while(0) #define Log_Write

//#define Log_Write

#endif  // ( defined(__APPLE__) || defined( __linux__ ) || defined( ESP_PLATFORM ) )
#endif  // ( defined( _WINDOWS) )

extern void print_hex_buf( const uint8_t* buf, int len );

#endif  // #ifndef __ZNET_LOGGING_H__
