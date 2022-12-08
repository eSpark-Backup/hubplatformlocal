/**
 * @file znet_chrono.h
 * @date 27 Mar 2018
 * @brief TBD.
 */

#ifndef ZNET_CHRONO_H
#define ZNET_CHRONO_H

#ifndef BUILD_ZNET_LIBRARY
#error "For internal use only!"
#endif

#include <znet/znet.h>

/**
 * @brief Get value from clock that cannot be set and represents monotonic time
 * since some unspecified starting point.
 *
 * @return Time from monotonic timer in ms. On error, 0 is returned
 */
uint64_t znet_clock( void );

#endif  // ZNET_CHRONO_H
