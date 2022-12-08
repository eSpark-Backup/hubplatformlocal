/**
 * @file znet_chrono.c
 * @date 27 Mar 2018
 * @brief TBD.
 */

/// INFO: pair
#include "znet_chrono.h"

/// INFO: crt & system
#include <assert.h>

/// INFO: public
#include <znet/znet.h>

/// INFO: private
#include "znet_main.h"
#include "znet_log.h"

/// INFO: internal

uint64_t znet_clock( void ) {
    return znet_cb->clock( znet_cb->arg );
}
