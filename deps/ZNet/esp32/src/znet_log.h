/**
 * @file znet_log.h
 * @date 16 Mar 2018
 * @brief TBD.
 */

#ifndef ZNET_LOG_H
#define ZNET_LOG_H

#ifndef BUILD_ZNET_LIBRARY
#error "For internal use only!"
#endif

#include <stdarg.h>

/**
 * @brief Log level
 */
typedef enum {
    ZNET_LOG_NONE = 0, /**< No log */
    ZNET_LOG_ERR,      /**< Error conditions */
    ZNET_LOG_WARNING,  /**< Warning conditions */
    ZNET_LOG_INFO,     /**< Informational conditions */
    ZNET_LOG_DEBUG     /**< Debug-level messages */
} znet_log_level_t;

/**
 * @brief Config log level
 */
#ifndef ZNET_LOG_LEVEL
#if CONFIG_ZNET_LOG_LEVEL
#define ZNET_LOG_LEVEL CONFIG_ZNET_LOG_LEVEL
#endif
#endif

/**
 * @brief Default log level
 */
#ifndef ZNET_LOG_LEVEL
#ifdef NDEBUG
#define ZNET_LOG_LEVEL                                     \
    ZNET_LOG_INFO /**< Default log level for release build \
                   */
#else
#define ZNET_LOG_LEVEL                                    \
    ZNET_LOG_DEBUG /**< Default log level for debug build \
                    */
#endif
#endif

/**
 * @brief Generate a log message using fmt string and option arguments.
 *
 * @param lvl Log message level
 * @param fmt Format string (see printf)
 */
void znet_log( znet_log_level_t lvl, const char* fmt, ... );

/**
 * @brief Generate a log message using fmt and using arguments pointed to by va.
 *
 * @param lvl Log message level
 * @param fmt Format string (see printf)
 */
void znet_vlog( znet_log_level_t lvl, const char* fmt, va_list list );

/**
 * @brief Macro to log with current level
 */
#define ZNET_LOGE( fmt, ... )                             \
    do {                                                  \
        if( ZNET_LOG_LEVEL >= ZNET_LOG_ERR )              \
            znet_log( ZNET_LOG_ERR, fmt, ##__VA_ARGS__ ); \
    } while( 0 )
#define ZNET_LOGW( fmt, ... )                                 \
    do {                                                      \
        if( ZNET_LOG_LEVEL >= ZNET_LOG_WARNING )              \
            znet_log( ZNET_LOG_WARNING, fmt, ##__VA_ARGS__ ); \
    } while( 0 )
#define ZNET_LOGI( fmt, ... )                              \
    do {                                                   \
        if( ZNET_LOG_LEVEL >= ZNET_LOG_INFO )              \
            znet_log( ZNET_LOG_INFO, fmt, ##__VA_ARGS__ ); \
    } while( 0 )
#define ZNET_LOGD( fmt, ... )                               \
    do {                                                    \
        if( ZNET_LOG_LEVEL >= ZNET_LOG_DEBUG )              \
            znet_log( ZNET_LOG_DEBUG, fmt, ##__VA_ARGS__ ); \
    } while( 0 )

#endif  // ZNET_LOG_H
