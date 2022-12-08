#ifndef __LISTS_CFG_H__
#define __LISTS_CFG_H__

//----------------------------------------------------------------------------

#include "heap.h"

//----------------------------------------------------------------------------

#ifdef USE_ORIGINAL_HEAP_MANAGER

#define LIST_ELIGNMENT Heap_Alignment_Default
#define LISTENTRY_ELIGNMENT Heap_Alignment_Default

#endif  // USE_ORIGINAL_HEAP_MANAGER

//#define USE_LISTS_POOLS
#ifdef USE_LISTS_POOLS

#define LIST_POOL_START_COUNT 32
#define LISTENTRY_POOL_START_COUNT 64

#endif  // USE_ORIGINAL_HEAP_MANAGER

#endif  // #ifndef __LISTS_CFG_H__
