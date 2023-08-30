/**
 * @file znet_uart.h
 * @date 27 Mar 2018
 * @brief TBD.
 */

#ifndef ZNET_UART_H
#define ZNET_UART_H

#ifndef BUILD_ZNET_LIBRARY
#error "For internal use only!"
#endif

#include <znet/znet.h>

/**
 * @brief Check: uart reset is available
 *
 * @return Return zero on success. On error, error code is returned
 */
int znet_uart_reset_available( void );

/**
 * @brief Reset zwave module and uart connected to it
 *
 * Before call check possible to hard reset the module ()
 *
 * @param arg Parameter for callback functions
 * @return Return zero on success. On error, error code is returned
 */
int znet_uart_reset( void );

/**
 * @brief Read data from uart (from zwave module connected to uart)
 *
 * Use cases:
 * 1. Get free space in uart rx buffer - data == NULL, size == 0,
 * ret_size = <ptr>
 * 2. Read all data (limit by size) from uart rx buffer - data = <ptr>,
 * size != 0, ret_size = <ptr> For read call is blocked if all data can
 * not be readed from uart buffer
 *
 * @param data Pointer to buffer for data
 * @param size The size of the data for read
 * @param ret_size Return size of available data in uart buffer
 * @return Return zero on success. On error, error code is returned
 */
int znet_uart_read( void* data, size_t size, size_t* ret_size );

/**
 * @brief Write data to uart (to zwave module connected to uart)
 *
 * Use cases:
 * 1. Get free space in uart tx buffer - data == NULL, size == 0,
 * ret_size = <ptr>
 * 2. Write all data (limit by size) to uart tx buffer - data = <ptr>,
 * size != 0, ret_size = <ptr> For write call is blocked if all data can
 * not be written to uart buffer
 *
 * @param data Pointer to buffer with data
 * @param size The size of the data for write
 * @param ret_size Return free size in uart buffer
 * @return Return zero on success. On error, error code is returned
 */
int znet_uart_write( const void* data, size_t size, size_t* ret_size );

#endif  // ZNET_UART_H
