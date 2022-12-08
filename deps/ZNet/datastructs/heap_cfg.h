#ifndef __HEAP_CFG_H__
#define __HEAP_CFG_H__

#ifdef ESP_PLATFORM

#include <sdkconfig.h>

#if CONFIG_ZNET_HEAP
#define USE_ORIGINAL_HEAP_MANAGER

#define HEAP_SIZE CONFIG_ZNET_HEAP_ALLOC
#define HEADER_ALIGNMENT CONFIG_ZNET_HEAP_HEADER_ALIGNMENT
#define DATA_ALIGNMENT CONFIG_ZNET_HEAP_DATA_ALIGNMENT
#endif

#else

//#define USE_ORIGINAL_HEAP_MANAGER

#ifdef USE_ORIGINAL_HEAP_MANAGER

//----------------------------------------------------------------------------
//#define HEAP_SIZE            65536
#define HEAP_SIZE 32768
//#define HEAP_SIZE            24576
//#define HEAP_SIZE            20480

//#define HEAP_SIZE            19456

//#define HEAP_SIZE            19206

//#define HEAP_SIZE            19072

//#define HEAP_SIZE            19008
//#define HEAP_SIZE            18992
//#define HEAP_SIZE            18976

//#define HEAP_SIZE            18956

//#define HEAP_SIZE            18432
//#define HEAP_SIZE            17920
//#define HEAP_SIZE            17792
//#define HEAP_SIZE            17784
//#define HEAP_SIZE            17780
//#define HEAP_SIZE            17778

//#define HEAP_SIZE            17776

//#define HEAP_SIZE            17760
//#define HEAP_SIZE            17728
//#define HEAP_SIZE            17664
//#define HEAP_SIZE            17408
//#define HEAP_SIZE            16384
//#define HEAP_SIZE             10240
//#define HEAP_SIZE             1024
//#define HEAP_SIZE               128
//#define HEAP_SIZE               64

#define HEADER_ALIGNMENT 4
#define DATA_ALIGNMENT 4

#endif  // USE_ORIGINAL_HEAP_MANAGER

#endif  // platform

#endif  // #ifndef __HEAP_CFG_H__
