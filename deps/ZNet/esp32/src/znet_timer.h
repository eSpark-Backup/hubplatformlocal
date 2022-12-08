/**
 * @file znet_timer.h
 * @date 27 Jun 2018
 * @brief TBD.
 */

#ifndef ZNET_TIMER_H
#define ZNET_TIMER_H

#ifndef BUILD_ZNET_LIBRARY
#error "For internal use only!"
#endif

#include <znet/znet.h>

/**
 * @brief Type of timer
 */
typedef void znet_timer_t;

/**
 * @brief Function prototype for timer callback
 *
 * @param timer Timer handler
 * @param data Data passed to setup timer fucntion
 * @param destroy Zero on timeout. On destroy, Non zero is set
 */
typedef void ( *ZNET_TIMER_CALLBACK )( znet_timer_t* timer, void* data, int destroy );

/**
 * @brief Setup timer
 *
 * After calling callback function timer handler is not valid
 *
 * @param timeout Timeout in ms
 * @param func Function called after timeout, if 0 then call on next iteration
 * loop
 * @param data Data to be transferred to the callback function
 * @return Return timer handler on success. On error, NULL is returned and
 * callback called
 */
znet_timer_t* znet_timer_shoot( uint64_t timeout, ZNET_TIMER_CALLBACK func, void* data );

/**
 * @brief Destroy timer before timed out
 *
 * @param timer Timer handler
 */
void znet_timer_kill( znet_timer_t* timer );

#endif  // ZNET_TIMER_H
