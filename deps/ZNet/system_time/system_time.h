#ifndef __SYSTEM_TIME_H__
#define __SYSTEM_TIME_H__

//////////////////////////////////////////////////////////////////////////////

#include "typedefs.h"

typedef uint64_t SystemTime_t;
typedef SystemTime_t* SystemTime;

//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////

bool ICACHE_FLASH_ATTR SystemTime_Refresh( void );

//////////////////////////////////////////////////////////////////////////////

/********************************  Timer
 * ***************************************************************/
/*
  void os_timer_arm(uint8_t* timer, uint32_t time, bool repeat_flag);
  void os_timer_disarm(uint8_t* timer);
  void os_timer_setfn(uint8_t* timer, void* pfunction, void* parg);
*/
void znet_timer_process( void );
void ICACHE_FLASH_ATTR znet_timer_set( uint8_t* timer, uint64_t time );
void ICACHE_FLASH_ATTR znet_timer_stop( uint8_t* timer );
void ICACHE_FLASH_ATTR znet_timer_init( uint8_t* timer, void* pfunction, void* parg );
void ICACHE_FLASH_ATTR znet_timer_deinit( uint8_t* timer );
uint64_t ICACHE_FLASH_ATTR znet_timer_get_remaining( uint8_t* timer );

//----------------------------------------------------------------------------
SystemTime_t SystemTime_Now( void );

//----------------------------------------------------------------------------
SystemTime_t ICACHE_FLASH_ATTR SystemTime_us( uint64_t value );
SystemTime_t SystemTime_ms( uint64_t value );
SystemTime_t ICACHE_FLASH_ATTR SystemTime_s( uint64_t value );
SystemTime_t ICACHE_FLASH_ATTR SystemTime_m( uint64_t value );
SystemTime_t ICACHE_FLASH_ATTR SystemTime_h( uint64_t value );
SystemTime_t ICACHE_FLASH_ATTR SystemTime_D( uint64_t value );
SystemTime_t ICACHE_FLASH_ATTR SystemTime_W( uint64_t value );

SystemTime_t ICACHE_FLASH_ATTR SystemTime_us_( uint64_t us, uint64_t ns );
SystemTime_t ICACHE_FLASH_ATTR SystemTime_ms_( uint64_t ms, uint64_t us, uint64_t ns );
SystemTime_t ICACHE_FLASH_ATTR SystemTime_s_( uint64_t s, uint64_t ms, uint64_t us, uint64_t ns );
SystemTime_t ICACHE_FLASH_ATTR SystemTime_m_( uint64_t m, uint64_t s, uint64_t ms, uint64_t us, uint64_t ns );
SystemTime_t ICACHE_FLASH_ATTR
SystemTime_h_( uint64_t h, uint64_t m, uint64_t s, uint64_t ms, uint64_t us, uint64_t ns );
SystemTime_t ICACHE_FLASH_ATTR
SystemTime_D_( uint64_t D, uint64_t h, uint64_t m, uint64_t s, uint64_t ms, uint64_t us, uint64_t ns );
SystemTime_t ICACHE_FLASH_ATTR
SystemTime_W_( uint64_t W, uint64_t D, uint64_t h, uint64_t m, uint64_t s, uint64_t ms, uint64_t us, uint64_t ns );

//----------------------------------------------------------------------------
uint64_t ICACHE_FLASH_ATTR SystemTime_Decode_us( SystemTime_t systemTime );
uint64_t ICACHE_FLASH_ATTR SystemTime_Decode_ms( SystemTime_t systemTime );
uint64_t ICACHE_FLASH_ATTR SystemTime_Decode_s( SystemTime_t systemTime );
uint64_t ICACHE_FLASH_ATTR SystemTime_Decode_m( SystemTime_t systemTime );
uint64_t ICACHE_FLASH_ATTR SystemTime_Decode_h( SystemTime_t systemTime );
uint64_t ICACHE_FLASH_ATTR SystemTime_Decode_D( SystemTime_t systemTime );
uint64_t ICACHE_FLASH_ATTR SystemTime_Decode_W( SystemTime_t systemTime );

uint64_t ICACHE_FLASH_ATTR SystemTime_Decode_us_( SystemTime_t systemTime, uint64_t* ns );
uint64_t ICACHE_FLASH_ATTR SystemTime_Decode_ms_( SystemTime_t systemTime, uint64_t* us, uint64_t* ns );
uint64_t ICACHE_FLASH_ATTR SystemTime_Decode_s_( SystemTime_t systemTime, uint64_t* ms, uint64_t* us, uint64_t* ns );
uint64_t ICACHE_FLASH_ATTR
SystemTime_Decode_m_( SystemTime_t systemTime, uint64_t* s, uint64_t* ms, uint64_t* us, uint64_t* ns );
uint64_t ICACHE_FLASH_ATTR
SystemTime_Decode_h_( SystemTime_t systemTime, uint64_t* m, uint64_t* s, uint64_t* ms, uint64_t* us, uint64_t* ns );
uint64_t ICACHE_FLASH_ATTR SystemTime_Decode_D_( SystemTime_t systemTime,
                                                 uint64_t* h,
                                                 uint64_t* m,
                                                 uint64_t* s,
                                                 uint64_t* ms,
                                                 uint64_t* us,
                                                 uint64_t* ns );
uint64_t ICACHE_FLASH_ATTR SystemTime_Decode_W_( SystemTime_t systemTime,
                                                 uint64_t* D,
                                                 uint64_t* h,
                                                 uint64_t* m,
                                                 uint64_t* s,
                                                 uint64_t* ms,
                                                 uint64_t* us,
                                                 uint64_t* ns );

//----------------------------------------------------------------------------
typedef struct UnixTime_s {
    uint8_t _sec;
    uint8_t _min;
    uint8_t _hour;
    uint8_t _wday;
    uint8_t _mday;
    uint8_t _mon;
    int32_t _year;

} UnixTime_t;
typedef UnixTime_t* UnixTime;

bool ICACHE_FLASH_ATTR SystemTime_ToUNIX( SystemTime_t systemTime, UnixTime unixTime );
SystemTime_t ICACHE_FLASH_ATTR SystemTime_FromUNIX( UnixTime unixTime );

//----------------------------------------------------------------------------
bool ICACHE_FLASH_ATTR SystemTime_Init( void );

//////////////////////////////////////////////////////////////////////////////

#endif  // #ifndef __SYSTEM_TIME_H__
