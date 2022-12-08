/**
 * @file znet_uart.c
 * @date 27 Mar 2018
 * @brief TBD.
 */

/// INFO: pair
#include "znet_uart.h"

/// INFO: crt & system
#include <assert.h>

/// INFO: public
#include <znet/znet.h>

/// INFO: private
#include "znet_main.h"
#include "znet_log.h"

/// INFO: internal

int znet_uart_reset_available( void ) {
    if( znet_cb->uart_reset == NULL )
        return ZNET_ERR_INVAL;

    return ZNET_ERR_OK;
}

int znet_uart_reset( void ) {
    assert( znet_cb->uart_reset );

    return znet_cb->uart_reset( znet_cb->arg );
}

int znet_uart_read( void* data, size_t size, size_t* ret_size ) {
    assert( ( data && size ) || ret_size );

    return znet_cb->uart_read( data, size, ret_size, znet_cb->arg );
}

int znet_uart_write( const void* data, size_t size, size_t* ret_size ) {
    assert( ( data && size ) || ret_size );

    return znet_cb->uart_write( data, size, ret_size, znet_cb->arg );
}
