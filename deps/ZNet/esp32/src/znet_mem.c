/**
 * @file znet_mem.c
 * @date 19 Mar 2018
 * @brief TBD.
 */

/// INFO: pair
#include "znet_mem.h"

/// INFO: crt & system
#include <string.h>

/// INFO: public
#include <znet/znet.h>

/// INFO: private
#include "znet_main.h"

#include <stdio.h>

void* znet_malloc( size_t size ) {
    return znet_cb->alloc( NULL, size, znet_cb->arg );
}

void* znet_realloc( void* ptr, size_t size ) {
    return znet_cb->alloc( ptr, size, znet_cb->arg );
}

void znet_free( void* ptr ) {
    znet_cb->alloc( ptr, 0, znet_cb->arg );
}
