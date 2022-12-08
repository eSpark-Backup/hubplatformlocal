#ifndef __ZNET_TRACKED_TIMES_H__
#define __ZNET_TRACKED_TIMES_H__

//////////////////////////////////////////////////////////////////////////////

#include "system_time.h"

//////////////////////////////////////////////////////////////////////////////

typedef struct TrackedTimes_s {
    SystemTime_t _updated;
    SystemTime_t _invalidated;

} TrackedTimes_t;
typedef TrackedTimes_t* TrackedTimes;

bool ICACHE_FLASH_ATTR TrackedTimes_Init( TrackedTimes times );
void ICACHE_FLASH_ATTR TrackedTimes_Update( TrackedTimes times );
void ICACHE_FLASH_ATTR TrackedTimes_Invalidate( TrackedTimes times );
bool ICACHE_FLASH_ATTR TrackedTimes_IsUpdated( TrackedTimes times );
bool ICACHE_FLASH_ATTR TrackedTimes_IsInvalidated( TrackedTimes times );

//////////////////////////////////////////////////////////////////////////////

#endif  // __ZNET_TRACKED_TIMES_H__
