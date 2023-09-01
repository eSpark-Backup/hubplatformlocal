/**
 * @file znet_mem.h
 * @date 16 Mar 2018
 * @brief TBD.
 */

#ifndef ZNET_MEM_H
#define ZNET_MEM_H

#ifndef BUILD_ZNET_LIBRARY
#error "For internal use only!"
#endif

#include <stddef.h>

/**
 * @brief Allocate memory block
 *
 * @param size Size of the requested memory
 * @return Pointer to the allocated memory. On error, these functions return
 * NULL.
 */
void* znet_malloc( size_t size );

/**
 * @brief Reallocate memory
 *
 * @param ptr Pointer to allocated mamory before
 * @param size
 * @return
 */
void* znet_realloc( void* ptr, size_t size );

/**
 * @brief Free memory
 *
 * Frees the memory space pointed to by ptr, which must have been returned by a
 * previous call to znet_malloc() or znet_realloc(). If znet_free( ptr ) has
 * already been called before, undefined behavior occurs. If ptr is NULL, no
 * operation is performed.
 *
 * @param prt Pointer to allocated mamory before
 */
void znet_free( void* ptr );

#endif  // ZNET_MEM_H
