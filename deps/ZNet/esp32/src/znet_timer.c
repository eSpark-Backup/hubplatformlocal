/**
 * @file znet_timer.c
 * @date 27 Jul 2018
 * @brief TBD.
 */

/// INFO: pair
#include "znet_timer.h"

/// INFO: crt & system
#include <assert.h>

/// INFO: public
#include <znet/znet.h>

/// INFO: private
#include "znet_main.h"
#include "znet_chrono.h"
#include "znet_log.h"

/// INFO: internal

extern ZNet_t znet_instance;

znet_timer_t* znet_timer_shoot( uint64_t timeout, ZNET_TIMER_CALLBACK func, void* data ) {
    ZTimer timer = ZTimer_Create( SystemTime_ms( timeout ), (ZTimer_Callback)func, data );
    if( timer ) {
        if( !ZNet_AddTimer( timer ) ) {
            ZTimer_Free( timer );
            timer = NULL;
        }
    }
    return (znet_timer_t*)timer;
}

void znet_timer_kill( znet_timer_t* timer ) {
    if( ZNet_RemoveTimer( (ZTimer)timer ) ) {
        ZTimer_Free( (void*)timer );
    }
}
