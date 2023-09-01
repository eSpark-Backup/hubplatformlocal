/**
 * @file znet_log.c
 * @date 19 Mar 2018
 * @brief TBD.
 */

/// INFO: pair
#include "znet_log.h"

/// INFO: crt & system

/// INFO: public
#include <znet/znet.h>

/// INFO: private
#include "znet_main.h"
#include <stdio.h>

void znet_log( znet_log_level_t lvl, const char* fmt, ... ) {
    va_list args;
    va_start( args, fmt );
    znet_vlog( lvl, fmt, args );
    va_end( args );
}

void znet_vlog( znet_log_level_t lvl, const char* fmt, va_list list ) {
    if( lvl == ZNET_LOG_NONE )
        return;

    if( znet_cb ) {
        znet_cb->log( (int)lvl, fmt, list, znet_cb->arg );
    }
    else {
        vprintf( fmt, list );
    }
}
