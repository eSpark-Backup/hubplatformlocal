#ifndef __HEAP_H__
#define __HEAP_H__

//----------------------------------------------------------------------------

#include "heap_cfg.h"

#include "typedefs.h"

#if( defined( _WINDOWS ) )
#include <stdlib.h>
#elif( defined( ESP_PLATFORM ) || defined( __APPLE__ ) || defined( __linux__ ) )
#include "znet_mem.h"
#else
#include "espmissingincludes.h"
#include <mem.h>
#endif  // ( defined( _WINDOWS ) || defined(__APPLE__) || defined(__linux__) || defined( ESP_PLATFORM ) )

//----------------------------------------------------------------------------

#ifdef USE_ORIGINAL_HEAP_MANAGER

void* ICACHE_FLASH_ATTR Heap_Alloc( int size );  // returns NULL if fault
void* ICACHE_FLASH_ATTR Heap_ReAlloc( void* ptr,
                                      int size );  // returns NULL if fault
void ICACHE_FLASH_ATTR Heap_Free( void* ptr );     // returns FALSE if fault

bool ICACHE_FLASH_ATTR Heap_Init( void );

#if( defined( _WINDOWS ) || defined( __APPLE__ ) || defined( __linux__ ) )
void ICACHE_FLASH_ATTR Heap_Show( void );
void ICACHE_FLASH_ATTR Heap_Test( void );
#endif

#else

#define Heap_Init() ( TRUE )

#if( defined( _WINDOWS ) )
#define Heap_Alloc( size ) malloc( size )
#define Heap_ReAlloc( ptr, size ) realloc( ptr, size )
#define Heap_Free( ptr ) free( ptr )
#elif( defined( ESP_PLATFORM ) || defined( __APPLE__ ) || defined( __linux__ ) )
#define Heap_Alloc( size ) znet_malloc( size )
#define Heap_ReAlloc( ptr, size ) znet_realloc( ptr, size )
#define Heap_Free( ptr ) znet_free( ptr )
#else
#define Heap_Alloc( size ) os_malloc( size )
#define Heap_ReAlloc( ptr, size ) os_realloc( ptr, size )
#define Heap_Free( ptr ) os_free( ptr )
#endif  // ( defined( _WINDOWS) || defined(__APPLE__) || defined(__linux__) || defined( ESP_PLATFORM ) )

#endif  // USE_ORIGINAL_HEAP_MANAGER

#endif  // #ifndef __HEAP_H__
