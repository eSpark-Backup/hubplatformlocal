#ifndef __TYPEDEFS_H__
#define __TYPEDEFS_H__

#include <limits.h>

#if defined( _WINDOWS )
#include <windows.h>
//	#include <windef.h>
#endif

#if( defined( __APPLE__ ) || defined( __linux__ ) || defined( ESP_PLATFORM ) )
#include <stdbool.h>
#endif

#if( defined( _WINDOWS ) || defined( __APPLE__ ) || defined( __linux__ ) || defined( ESP_PLATFORM ) )
#include <stdlib.h>
#include <stdint.h>
#endif

typedef enum {
    Encapsulation_None = 0,
    Encapsulation_Sec_UNAUTHENTICATED = 0x01,
    Encapsulation_Sec_AUTHENTICATED = 0x02,
    Encapsulation_Sec_2 = 0x04,
    //    Encapsulation_MuCh = 0x20, /// INFO: deprecated by refactoring
    Encapsulation_CRC16 = 0x40,
    Encapsulation_Sec_0 = 0x80,
} encap_type_t;

typedef enum {
    Message_Not_Sending = 0x00,
    Message_Sending = 0x01,
} message_send_type_t;
extern message_send_type_t message_sending;

#define ALL_SECURITY \
    ( Encapsulation_Sec_UNAUTHENTICATED | Encapsulation_Sec_AUTHENTICATED | Encapsulation_Sec_2 | Encapsulation_Sec_0 )

/*
/// Exact integral types.

/// int8_t & uint8_t:
#if (SCHAR_MAX == 127)
        typedef signed   char int8_t ;
        #define INT8_T_DEFINED
        typedef unsigned char uint8_t;
        #define UINT8_T_DEFINED
#else
        #if (SHRT_MAX == 127)
                typedef signed   short int8_t ;
                #define INT8_T_DEFINED
                typedef unsigned short uint8_t;
                #define UINT8_T_DEFINED
        #else
                #if (INT_MAX == 127)
                        typedef signed   int int8_t ;
                        #define INT8_T_DEFINED
                        typedef unsigned int uint8_t;
                        #define UINT8_T_DEFINED
                #else
                        #if (LONG_MAX == 127)
                                typedef signed   long int8_t ;
                                #define INT8_T_DEFINED
                                typedef unsigned long uint8_t;
                                #define UINT8_T_DEFINED
                        #else
                                #if (LLONG_MAX == 127)
                                        typedef signed   long long int8_t ;
                                        #define INT8_T_DEFINED
                                        typedef unsigned long long uint8_t;
                                        #define UINT8_T_DEFINED
                                #else
                                        #error "Can not typedef int8_t &
uint8_t" #endif #endif #endif #endif #endif

/// int16_t & uint16_t:
#if (SHRT_MAX == 32767)
        typedef signed   short int16_t ;
        #define INT16_T_DEFINED
        typedef unsigned short uint16_t;
        #define UINT16_T_DEFINED
#else
        #if (INT_MAX == 32767)
                typedef signed   int int16_t ;
                #define INT16_T_DEFINED
                typedef unsigned int uint16_t;
                #define UINT16_T_DEFINED
        #else
                #if (LONG_MAX == 32767)
                        typedef signed   long int16_t ;
                        #define INT16_T_DEFINED
                        typedef unsigned long uint16_t;
                        #define UINT16_T_DEFINED
                #else
                        #if (LLONG_MAX == 32767)
                                typedef signed   long long int16_t ;
                                #define INT16_T_DEFINED
                                typedef unsigned long long uint16_t;
                                #define UINT16_T_DEFINED
                        #else
                                #error "Can not typedef int16_t & uint16_t"
                        #endif
                #endif
        #endif
#endif

/// int32_t & uint32_t:
#if (INT_MAX == 2147483647)
        typedef signed   int int32_t ;
        #define INT32_T_DEFINED
        typedef unsigned int uint32_t;
        #define UINT32_T_DEFINED
#else
        #if (LONG_MAX == 2147483647)
                typedef signed   long int32_t ;
                #define INT32_T_DEFINED
                typedef unsigned long uint32_t;
                #define UINT32_T_DEFINED
        #else
                #if (LLONG_MAX == 2147483647)
                        typedef signed   long long int32_t ;
                        #define INT32_T_DEFINED
                        typedef unsigned long long uint32_t;
                        #define UINT32_T_DEFINED
                #else
                        #error "Can not typedef int32_t & uint32_t"
                #endif
        #endif
#endif


/// int64_t & uint64_t:
#if (LONG_MAX == 9223372036854775807)
        typedef signed   long int64_t ;
        #define INT64_T_DEFINED
        typedef unsigned long uint64_t;
        #define UINT64_T_DEFINED
#else
        #if (LLONG_MAX == 9223372036854775807)
                typedef signed   long long int64_t ;
                #define INT64_T_DEFINED
                typedef unsigned long long uint64_t;
                #define UINT64_T_DEFINED
        #else
                #error "Can not typedef int64_t & uint64_t"
        #endif
#endif


/// Small types.
/// Signed.
#ifdef INT8_T_DEFINED
        typedef int8_t int_least8_t;
#else
        #error "Can not typedef int_least8_t"
#endif
#ifdef INT16_T_DEFINED
        typedef int16_t int_least16_t;
#else
        #error "Can not typedef int_least16_t"
#endif
#ifdef INT32_T_DEFINED
        typedef int32_t int_least32_t;
#else
        #error "Can not typedef int_least32_t"
#endif
#ifdef INT64_T_DEFINED
        typedef int64_t int_least64_t;
#else
        #error "Can not typedef int_least64_t"
#endif
/// Unsigned.
#ifdef UINT8_T_DEFINED
        typedef uint8_t uint_least8_t;
#else
        #error "Can not typedef uint_least8_t"
#endif
#ifdef UINT16_T_DEFINED
        typedef uint16_t uint_least16_t;
#else
        #error "Can not typedef uint_least16_t"
#endif
#ifdef UINT32_T_DEFINED
        typedef uint32_t uint_least32_t;
#else
        #error "Can not typedef uint_least32_t"
#endif
#ifdef UINT64_T_DEFINED
        typedef uint64_t uint_least64_t;
#else
        #error "Can not typedef uint_least64_t"
#endif


/// Fast types.
/// Signed.
#ifdef INT8_T_DEFINED
        typedef int8_t int_fast8_t;
#else
        #error "Can not typedef int_fast8_t"
#endif
#ifdef INT16_T_DEFINED
        typedef int16_t int_fast16_t;
#else
        #error "Can not typedef int_fast16_t"
#endif
#ifdef INT32_T_DEFINED
        typedef int32_t int_fast32_t;
#else
        #error "Can not typedef int_fast32_t"
#endif
#ifdef INT64_T_DEFINED
        typedef int64_t int_fast64_t;
#else
        #error "Can not typedef int_fast64_t"
#endif
/// Unsigned.
#ifdef UINT8_T_DEFINED
        typedef uint8_t uint_fast8_t;
#else
        #error "Can not typedef uint_fast8_t"
#endif
#ifdef UINT16_T_DEFINED
        typedef uint16_t uint_fast16_t;
#else
        #error "Can not typedef uint_fast16_t"
#endif
#ifdef UINT32_T_DEFINED
        typedef uint32_t uint_fast32_t;
#else
        #error "Can not typedef uint_fast32_t"
#endif
#ifdef UINT64_T_DEFINED
        typedef uint64_t uint_fast64_t;
#else
        #error "Can not typedef uint_fast64_t"
#endif
*/

/// BOOLs

#ifdef _WINDOWS
//#error "WINDOWS!!!!!!!!!!!!!!!!!"
#define false_value ( 0 != 0 )
#define true_value ( !false_value )
typedef BOOL bool;
#define false FALSE
#define true TRUE
#else
#if( defined( __linux__ ) || defined( __APPLE__ ) )
//#error "LINUX!!!!!!!!!!!!!!!!!"
#if( __bool_true_false_are_defined < 1 )
#define false_value ( 0 != 0 )
#define true_value ( !false_value )
typedef enum { false = false_value, true = true_value } bool;
#endif
#define FALSE false
#define TRUE true
#define BOOL bool
#else
#ifdef ESP_PLATFORM
//#error "ESP32!!!!!!!!!!!!!!!!!"
#if( __bool_true_false_are_defined < 1 )
#define false_value ( 0 != 0 )
#define true_value ( !false_value )
typedef enum { false = false_value, true = true_value } bool;
#endif
#define FALSE false
#define TRUE true
#define BOOL bool
#else
// ESP8266 ?
#include <c_types.h>
//#error "uC!!!!!!!!!!!!!!!!!"
#define false_value ( 0 != 0 )
#define true_value ( !false_value )
// typedef enum { FALSE = false_value, TRUE = true_value } bool;
//#define false  FALSE
//#define true   TRUE
//#define BOOL   bool
#endif
#endif
#endif

#if( defined( _WINDOWS ) || defined( __APPLE__ ) || defined( __linux__ ) || defined( ESP_PLATFORM ) )
#define ICACHE_FLASH_ATTR
#define ICACHE_RODATA_ATTR
#define STORE_ATTR
#endif

#endif  // #ifndef __TYPEDEFS_H__
