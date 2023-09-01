//////////////////////////////////////////////////////////////////////////////

#include "system_time.h"
/// INFO: for esp32 and linux use callback functions
#if( defined( __APPLE__ ) || defined( __linux__ ) || defined( ESP_PLATFORM ) )
#include "znet_chrono.h"
#else
#include <sntp.h>
#endif

/// INFO: for esp32 and linux use callback functions
#if( 0 && ( defined( __APPLE__ ) || defined( __linux__ ) || defined( ESP_PLATFORM ) ) )
#include <time.h>
#endif

/// INFO: for esp32 and linux use callback functions
#if( 0 && defined( ESP_PLATFORM ) )
#include <errno.h>
#include <rom/rtc.h>
#include <esp_clk.h>
#include <esp_timer.h>
#include <soc/soc.h>
#include <soc/rtc.h>
#include <soc/rtc_cntl_reg.h>
#include <soc/frc_timer_reg.h>
#endif

#include "logger.h"
#include "ZPlatform.h"

//////////////////////////////////////////////////////////////////////////////

#if( !defined( __linux__ ) && !defined( ESP_PLATFORM ) && !defined( __APPLE__ ) )
#define SYSTEMTIME_TICKS_FREQ 1000000000ULL
#endif

#if( !defined( _WINDOWS ) && !defined( __linux__ ) && !defined( ESP_PLATFORM ) && !defined( __APPLE__ ) )
#define HW_TICKS_FREQ 1000000ULL
#endif

#if( !defined( _WINDOWS ) && !defined( __linux__ ) && !defined( ESP_PLATFORM ) && !defined( __APPLE__ ) )
#define REG_READ_( _r ) ( *(volatile uint32_t*)( _r ) )
#define TIME_NOW() REG_READ_( 0x3ff20c00 )
#endif

//////////////////////////////////////////////////////////////////////////////

#if( !defined( __linux__ ) && !defined( ESP_PLATFORM ) && !defined( __APPLE__ ) )
#if defined( _WINDOWS )
typedef LARGE_INTEGER SystemTicks_t;
#else
typedef uint32_t SystemTicks_t;
#endif
#endif

//----------------------------------------------------------------------------

/// TODO: separate REALTIME and MONOTONIC time
static volatile SystemTime_t systemTime;
static volatile SystemTime_t systemTimePrev;
#if( !defined( __linux__ ) && !defined( ESP_PLATFORM ) && !defined( __APPLE__ ) )
static volatile SystemTime_t systemTimeRem;
#endif

#if( !defined( __linux__ ) && !defined( ESP_PLATFORM ) && !defined( __APPLE__ ) )
static volatile SystemTime_t hwTicksFreq;
static volatile SystemTicks_t hwPrevTicks;
#endif

//----------------------------------------------------------------------------

/// INFO: for esp32 and linux use callback functions
#if( 0 && defined( ESP_PLATFORM ) )
#ifndef CLOCK_MONOTONIC
#define CLOCK_MONOTONIC (clockid_t)4
#endif

/// TODO: how to implement s_microseconds_offset? investigate... for more
/// details see time.c
int ICACHE_FLASH_ATTR clock_gettime( clockid_t clk_id, struct timespec* tp ) {
    uint64_t us;

    if( !tp ) {
        errno = EFAULT;
        return -1;
    }

    switch( clk_id ) {
    case CLOCK_REALTIME:
#if( defined( CONFIG_ESP32_TIME_SYSCALL_USE_RTC ) || defined( CONFIG_ESP32_TIME_SYSCALL_USE_RTC_FRC1 ) )
    {
        us = ( (uint64_t)REG_READ( RTC_BOOT_TIME_LOW_REG ) ) +
             ( ( (uint64_t)REG_READ( RTC_BOOT_TIME_HIGH_REG ) ) << 32 );
        /// INFO: for more details see time.c:get_rtc_time_us
        const uint64_t ticks = rtc_time_get();
        const uint32_t cal = esp_clk_slowclk_cal_get();
        /*  */
        const uint64_t ticks_low = ticks & UINT32_MAX;
        const uint64_t ticks_high = ticks >> 32;
        us += ( ( ticks_low * cal ) >> RTC_CLK_CAL_FRACT ) + ( ( ticks_high * cal ) << ( 32 - RTC_CLK_CAL_FRACT ) );
        ///
        tp->tv_sec = us / 1000000LL;
        tp->tv_nsec = ( us % 1000000LL ) * 1000;
    }
#else
#error \
    "To use this functionality, make sure 'Timers used for gettimeofday function' option in 'ESP32-specific config' menu of menuconfig is set to 'RTC and FRC1'."
        errno = EINVAL;
        return -1;
#endif
    break;

    case CLOCK_MONOTONIC:
#if( defined( CONFIG_ESP32_TIME_SYSCALL_USE_FRC1 ) || defined( CONFIG_ESP32_TIME_SYSCALL_USE_RTC_FRC1 ) )
    {
        us = esp_timer_get_time();
        tp->tv_sec = us / 1000000LL;
        tp->tv_nsec = ( us % 1000000LL ) * 1000;
    }
#else
#error \
    "To use this functionality, make sure 'Timers used for gettimeofday function' option in 'ESP32-specific config' menu of menuconfig is set to 'RTC and FRC1'."
        errno = EINVAL;
        return -1;
#endif
    break;

    default:
        errno = EINVAL;
        return -1;
    }

    return 0;
}
#endif

//----------------------------------------------------------------------------
#if( !defined( __linux__ ) && !defined( ESP_PLATFORM ) && !defined( __APPLE__ ) )
SystemTime_t SystemTime_CalcDeltaOfSystemTicks( SystemTicks_t prev, SystemTicks_t curr )

{
    SystemTime_t p, c;
#if defined( _WINDOWS )
    p = (SystemTime_t)prev.QuadPart;
    c = (SystemTime_t)curr.QuadPart;
#else
    p = (SystemTime_t)prev;
    c = (SystemTime_t)curr;
#endif
    /// return ( ( c >= p ) ? (c - p) : ((SystemTime_t)(-1ULL) - (p - c) +
    /// (SystemTime_t)1ULL) );
    // Log_Write( " (prev: %u %llu curr: %u %llu --> ", prev, prev, curr,
    // (SystemTime_t)curr );  Log_Write( "p: %llu c: %llu)", p, c );

    if( c >= p )
        return ( c - p );

    SystemTime_t delta;
    delta ^= delta;
    --delta;
    delta -= ( p - c );
    ++delta;
    return delta;
}
#endif

// static SystemTime_t prevSec;

//----------------------------------------------------------------------------
bool ICACHE_FLASH_ATTR SystemTime_Refresh( void ) {
    systemTimePrev = systemTime;
/// INFO: for esp32 and linux use callback functions
#if( defined( __APPLE__ ) || defined( __linux__ ) || defined( ESP_PLATFORM ) )
    uint64_t now = znet_clock();
    if( now == 0 )
        return FALSE;

    systemTime = now * 1000000LL;      // convert from ms to ns
    if( systemTime < systemTimePrev )  // process systemTime overflow
    {
        systemTimePrev = 0;
    }
    return TRUE;
#else
/// INFO: for esp32 and linux use callback functions
#if( 0 && ( defined( __APPLE__ ) || defined( __linux__ ) || defined( ESP_PLATFORM ) ) )

    struct timespec ts;
    if( clock_gettime( CLOCK_MONOTONIC, &ts ) )
        return FALSE;

    systemTime = ( (uint64_t)ts.tv_sec ) * 1000000000LL + ts.tv_nsec;

    if( systemTime < systemTimePrev ) {
        systemTimePrev = 0;
        systemTime = systemTimePrev - 1;
    }
    return TRUE;

#else  // ( defined(__APPLE__) || defined( __linux__ ) || defined( ESP_PLATFORM ) )

    SystemTicks_t hwTicks;
#if defined( _WINDOWS )
    if( !QueryPerformanceCounter( &hwTicks ) )
        return FALSE;
#else
    hwTicks = TIME_NOW();
    // Log_Write( "sizeof(uint64_t): %u ||| hwPrevTicks: %u espSysTicks: %u
    // );
#endif
    SystemTime_t delta = SystemTime_CalcDeltaOfSystemTicks( hwPrevTicks, hwTicks );
    hwPrevTicks = hwTicks;
    // Log_Write( " ---> delta: %llu", delta );

    delta *= SYSTEMTIME_TICKS_FREQ;
    // Log_Write( " : delta*: %llu", delta );
    systemTime += delta / hwTicksFreq;
    systemTimeRem += delta % hwTicksFreq;
    // Log_Write( " ---> sysTime: %llu sysTimeRem: %llu", systemTime,
    // systemTimeRem );

    if( systemTimeRem >= hwTicksFreq ) {
        systemTime += systemTimeRem / hwTicksFreq;
        systemTimeRem %= hwTicksFreq;
    }

    /*
    SystemTime_t newSec = systemTime / 1000000000ULL;
    if( newSec != prevSec ) {
            Log_Write( "%llu ", newSec );
            prevSec = newSec;
    }
    */
    if( systemTime < systemTimePrev ) {
        systemTimePrev = 0;
    }
    return TRUE;

#endif  // (defined(__APPLE__) || defined( __linux__ ) || defined( ESP_PLATFORM ) )
#endif
}

//////////////////////////////////////////////////////////////////////////////

//----------------------------------------------------------------------------
SystemTime_t SystemTime_Now( void ) {
    return systemTime;
}

//////////////////////////////////////////////////////////////////////////////

const uint16_t lmos[] = { 0, 31, 60, 91, 121, 152, 182, 213, 244, 274, 305, 335 };
const uint16_t mos[] = { 0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334 };

#define _TBIAS_DAYS ( ( 70ULL * 365ULL ) + 17ULL )
#define _TBIAS_SECS ( _TBIAS_DAYS * 86400ULL )
#define _TBIAS_YEAR 1900ULL
#define MONTAB( year ) ( ( ( (year)&3ULL ) || ( ( year ) == 0ULL ) ) ? mos : lmos )

#define Daysto32( year, mon ) ( ( ( year - 1ULL ) / 4ULL ) + MONTAB( year )[mon] )

//----------------------------------------------------------------------------
bool ICACHE_FLASH_ATTR SystemTime_ToUNIX( SystemTime_t systemTime, UnixTime unixTime ) {
    if( unixTime == NULL )
        return FALSE;

    SystemTime_t secs = SystemTime_Decode_s( systemTime );

    /* days, hour, min, sec */
    SystemTime_t days = _TBIAS_DAYS + ( secs / 86400ULL );
    secs %= 86400ULL;
    unixTime->_hour = ( uint8_t )( secs / 3600ULL );
    secs %= 3600ULL;
    unixTime->_min = ( uint8_t )( secs / 60ULL );
    unixTime->_sec = ( uint8_t )( secs % 60ULL );

    /* determine year */
    SystemTime_t year = days / 365ULL;
    SystemTime_t i;
    for( ; days < ( i = Daysto32( year, 0 ) + ( year * 365ULL ) ); )
        --year;
    days -= i;
    unixTime->_year = ( int32_t )( year + _TBIAS_YEAR );

    /* determine month */
    const uint16_t* pm = MONTAB( year );
    SystemTime_t mon = 12;
    for( ; days < (SystemTime_t)pm[--mon]; )
        ;
    unixTime->_mon = ( uint8_t )( mon + 1 );
    unixTime->_mday = ( uint8_t )( days - pm[mon] + 1 );

    /*week day, result == 0 means Sunday. month and day must be PLUS 1 */
    int m = unixTime->_mon + 1;
    int a = ( 14 - m ) / 12;
    int y = unixTime->_year - a;
    m += 12 * a - 2;
    unixTime->_wday =
        ( uint8_t )( ( 7000 + ( unixTime->_mday + y + y / 4 - y / 100 + y / 400 + ( 31 * m ) / 12 ) ) % 7 );

    return TRUE;
}

//----------------------------------------------------------------------------
SystemTime_t ICACHE_FLASH_ATTR SystemTime_FromUNIX( UnixTime unixTime ) {
    SystemTime_t year = ( SystemTime_t )( unixTime->_year - _TBIAS_YEAR );
    SystemTime_t mon = (SystemTime_t)unixTime->_mon /* - 1*/;
    return SystemTime_s(
        ( ( ( Daysto32( year, mon ) - 1 ) + ( year * 365ULL ) + unixTime->_mday - _TBIAS_DAYS ) * 86400ULL ) +
        ( unixTime->_hour * 3600ULL ) + ( unixTime->_min * 60ULL ) + unixTime->_sec );
}

//////////////////////////////////////////////////////////////////////////////

//----------------------------------------------------------------------------
/* clang-format off */
SystemTime_t ICACHE_FLASH_ATTR SystemTime_us( uint64_t value ) { return ( value *            1000ULL ); }
SystemTime_t SystemTime_ms( uint64_t value ) { return ( value *         1000000ULL ); }
SystemTime_t ICACHE_FLASH_ATTR SystemTime_s ( uint64_t value ) { return ( value *      1000000000ULL ); }
SystemTime_t ICACHE_FLASH_ATTR SystemTime_m ( uint64_t value ) { return ( value *     60000000000ULL ); }
SystemTime_t ICACHE_FLASH_ATTR SystemTime_h ( uint64_t value ) { return ( value *   3600000000000ULL ); }
SystemTime_t ICACHE_FLASH_ATTR SystemTime_D ( uint64_t value ) { return ( value *  86400000000000ULL ); }
SystemTime_t ICACHE_FLASH_ATTR SystemTime_W ( uint64_t value ) { return ( value * 604800000000000ULL ); }
/* clang-format on */

//----------------------------------------------------------------------------
/* clang-format off */
SystemTime_t ICACHE_FLASH_ATTR SystemTime_us_( uint64_t us, uint64_t ns ) 
{ 
	return ( (us * 1000ULL) + ns );
}
SystemTime_t ICACHE_FLASH_ATTR SystemTime_ms_( uint64_t ms, uint64_t us, uint64_t ns ) 
{ 
	return ( (ms * 1000000ULL) + (us * 1000ULL) + ns );
}
SystemTime_t ICACHE_FLASH_ATTR SystemTime_s_( uint64_t s, uint64_t ms, uint64_t us, uint64_t ns ) 
{ 
	return ( (s * 1000000000ULL) + (ms * 1000000ULL) + (us * 1000ULL) + ns );
}
SystemTime_t ICACHE_FLASH_ATTR SystemTime_m_( uint64_t m, uint64_t s, uint64_t ms, uint64_t us, uint64_t ns ) 
{ 
	return ( (m * 60000000000ULL) + (s * 1000000000ULL) + (ms * 1000000ULL) + (us * 1000ULL) + ns );
}
SystemTime_t ICACHE_FLASH_ATTR SystemTime_h_( uint64_t h, uint64_t m, uint64_t s, uint64_t ms, uint64_t us, uint64_t ns ) 
{ 
	return ( (h * 3600000000000ULL) + (m * 60000000000ULL) + (s * 1000000000ULL) + (ms * 1000000ULL) + (us * 1000ULL) + ns );
}
SystemTime_t ICACHE_FLASH_ATTR SystemTime_D_( uint64_t D, uint64_t h, uint64_t m, uint64_t s, uint64_t ms, uint64_t us, uint64_t ns ) 
{ 
	return ( (D * 86400000000000ULL) + (h * 3600000000000ULL) + (m * 60000000000ULL) + (s * 1000000000ULL) + (ms * 1000000ULL) + (us * 1000ULL) + ns );
}
SystemTime_t ICACHE_FLASH_ATTR SystemTime_W_( uint64_t W, uint64_t D, uint64_t h, uint64_t m, uint64_t s, uint64_t ms, uint64_t us, uint64_t ns ) 
{ 
	return ( (W * 604800000000000ULL) + (D * 86400000000000ULL) + (h * 3600000000000ULL) + (m * 60000000000ULL) + (s * 1000000000ULL) + (ms * 1000000ULL) + (us * 1000ULL) + ns );
}
/* clang-format on */

//////////////////////////////////////////////////////////////////////////////

//----------------------------------------------------------------------------
uint64_t ICACHE_FLASH_ATTR SystemTime_Decode_us( SystemTime_t systemTime ) {
    return ( systemTime / 1000ULL );
}
uint64_t ICACHE_FLASH_ATTR SystemTime_Decode_ms( SystemTime_t systemTime ) {
    return ( systemTime / 1000000ULL );
}
uint64_t ICACHE_FLASH_ATTR SystemTime_Decode_s( SystemTime_t systemTime ) {
    return ( systemTime / 1000000000ULL );
}
uint64_t ICACHE_FLASH_ATTR SystemTime_Decode_m( SystemTime_t systemTime ) {
    return ( systemTime / 60000000000ULL );
}
uint64_t ICACHE_FLASH_ATTR SystemTime_Decode_h( SystemTime_t systemTime ) {
    return ( systemTime / 3600000000000ULL );
}
uint64_t ICACHE_FLASH_ATTR SystemTime_Decode_D( SystemTime_t systemTime ) {
    return ( systemTime / 86400000000000ULL );
}
uint64_t ICACHE_FLASH_ATTR SystemTime_Decode_W( SystemTime_t systemTime ) {
    return ( systemTime / 604800000000000ULL );
}

//----------------------------------------------------------------------------
uint64_t ICACHE_FLASH_ATTR SystemTime_Decode_us_( SystemTime_t systemTime, uint64_t* ns ) {
    uint64_t us = systemTime / 1000ULL;
    if( ns )
        *ns = systemTime - us * 1000ULL;
    return us;
}
uint64_t ICACHE_FLASH_ATTR SystemTime_Decode_ms_( SystemTime_t systemTime, uint64_t* us, uint64_t* ns ) {
    uint64_t ms = systemTime / 1000000ULL;
    uint64_t msRem = systemTime - ms * 1000000ULL;
    if( us )
        *us = SystemTime_Decode_us_( msRem, ns );
    else if( ns )
        *ns = msRem;
    return ms;
}
uint64_t ICACHE_FLASH_ATTR SystemTime_Decode_s_( SystemTime_t systemTime, uint64_t* ms, uint64_t* us, uint64_t* ns ) {
    uint64_t s = systemTime / 1000000000ULL;
    uint64_t sRem = systemTime - s * 1000000000ULL;
    if( ms )
        *ms = SystemTime_Decode_ms_( sRem, us, ns );
    else {
        if( us )
            *us = SystemTime_Decode_us_( sRem, ns );
        else if( ns )
            *ns = sRem;
    }
    return s;
}
uint64_t ICACHE_FLASH_ATTR
SystemTime_Decode_m_( SystemTime_t systemTime, uint64_t* s, uint64_t* ms, uint64_t* us, uint64_t* ns ) {
    uint64_t m = systemTime / 60000000000ULL;
    uint64_t mRem = systemTime - m * 60000000000ULL;
    if( s )
        *s = SystemTime_Decode_s_( mRem, ms, us, ns );
    else {
        if( ms )
            *ms = SystemTime_Decode_ms_( mRem, us, ns );
        else {
            if( us )
                *us = SystemTime_Decode_us_( mRem, ns );
            else if( ns )
                *ns = mRem;
        }
    }
    return m;
}
uint64_t ICACHE_FLASH_ATTR
SystemTime_Decode_h_( SystemTime_t systemTime, uint64_t* m, uint64_t* s, uint64_t* ms, uint64_t* us, uint64_t* ns ) {
    uint64_t h = systemTime / 3600000000000ULL;
    uint64_t hRem = systemTime - h * 3600000000000ULL;
    if( m )
        *m = SystemTime_Decode_m_( hRem, s, ms, us, ns );
    else {
        if( s )
            *s = SystemTime_Decode_s_( hRem, ms, us, ns );
        else {
            if( ms )
                *ms = SystemTime_Decode_ms_( hRem, us, ns );
            else {
                if( us )
                    *us = SystemTime_Decode_us_( hRem, ns );
                else if( ns )
                    *ns = hRem;
            }
        }
    }
    return h;
}
uint64_t ICACHE_FLASH_ATTR SystemTime_Decode_D_( SystemTime_t systemTime,
                                                 uint64_t* h,
                                                 uint64_t* m,
                                                 uint64_t* s,
                                                 uint64_t* ms,
                                                 uint64_t* us,
                                                 uint64_t* ns ) {
    uint64_t D = systemTime / 86400000000000ULL;
    uint64_t DRem = systemTime - D * 86400000000000ULL;
    if( h )
        *h = SystemTime_Decode_h_( DRem, m, s, ms, us, ns );
    else {
        if( m )
            *m = SystemTime_Decode_m_( DRem, s, ms, us, ns );
        else {
            if( s )
                *s = SystemTime_Decode_s_( DRem, ms, us, ns );
            else {
                if( ms )
                    *ms = SystemTime_Decode_ms_( DRem, us, ns );
                else {
                    if( us )
                        *us = SystemTime_Decode_us_( DRem, ns );
                    else if( ns )
                        *ns = DRem;
                }
            }
        }
    }
    return D;
}
uint64_t ICACHE_FLASH_ATTR SystemTime_Decode_W_( SystemTime_t systemTime,
                                                 uint64_t* D,
                                                 uint64_t* h,
                                                 uint64_t* m,
                                                 uint64_t* s,
                                                 uint64_t* ms,
                                                 uint64_t* us,
                                                 uint64_t* ns ) {
    uint64_t W = systemTime / 604800000000000ULL;
    uint64_t WRem = systemTime - W * 604800000000000ULL;
    if( D )
        *D = SystemTime_Decode_D_( WRem, h, m, s, ms, us, ns );
    else {
        if( h )
            *h = SystemTime_Decode_h_( WRem, m, s, ms, us, ns );
        else {
            if( m )
                *m = SystemTime_Decode_m_( WRem, s, ms, us, ns );
            else {
                if( s )
                    *s = SystemTime_Decode_s_( WRem, ms, us, ns );
                else {
                    if( ms )
                        *ms = SystemTime_Decode_ms_( WRem, us, ns );
                    else {
                        if( us )
                            *us = SystemTime_Decode_us_( WRem, ns );
                        else if( ns )
                            *ns = WRem;
                    }
                }
            }
        }
    }
    return W;
}

//////////////////////////////////////////////////////////////////////////////

//----------------------------------------------------------------------------
// returns substraction result was without underflow
bool ICACHE_FLASH_ATTR SystemTime_Init( void ) {
#if( defined( __APPLE__ ) || defined( __linux__ ) || defined( ESP_PLATFORM ) )

    systemTime = 0;
    systemTimePrev = 0;
    return SystemTime_Refresh();

#else

#if defined( _WINDOWS )
#if defined( _MSC_VER ) || defined( _MSC_EXTENSIONS )
#define DELTA_EPOCH_IN_MICROSECS 11644473600000000Ui64
#else
#define DELTA_EPOCH_IN_MICROSECS 11644473600000000ULL
#endif

    FILETIME ft;
    GetSystemTimeAsFileTime( &ft );
    // converting file time to unix epoch // convert into microseconds
    systemTime =
        SystemTime_us( ( ( ( ( (SystemTime_t)ft.dwHighDateTime ) << 32 ) | (SystemTime_t)ft.dwLowDateTime ) / 10 ) -
                       DELTA_EPOCH_IN_MICROSECS );

    SystemTicks_t freq;
    if( !QueryPerformanceFrequency( &freq ) )
        return FALSE;
    if( !QueryPerformanceCounter( (SystemTicks_t*)&hwPrevTicks ) )
        return FALSE;
    hwTicksFreq = (SystemTime_t)freq.QuadPart;
#else
    systemTime = SystemTime_s( sntp_get_current_timestamp() );

    hwTicksFreq = HW_TICKS_FREQ;
    hwPrevTicks = 0;
#endif  // defined( _WINDOWS )
    return TRUE;

#endif  // ( !defined( __linux__ ) && !defined( ESP_PLATFORM ) )
}

/********************************  Timer
 * ***************************************************************/
typedef void ( *Timer_Callback )( void* arg );

typedef struct {
    // uint32_t time_out;
    SystemTime_t finish_time;
    Timer_Callback callback;
    void* arg;
    bool work;
} znet_timer_t;

#define MAX_TIMER 20

znet_timer_t znet_timer[MAX_TIMER];
static uint8_t timer_quantity = 0;
SystemTime_t last_time;

void znet_timer_process( void ) {
    SystemTime_t time_now = SystemTime_Now();
    SystemTime_t time_diff = time_now - last_time;
    if( time_diff > SystemTime_ms( 100 ) ) {
        for( uint8_t i = 0; i <= timer_quantity; ++i ) {
            if( znet_timer[i].work ) {
                if( znet_timer[i].finish_time < time_now ) {
                    znet_timer[i].work = false;
                    ( *znet_timer[i].callback )( znet_timer[i].arg );
                }
            }
        }
        last_time = time_now;
    }
}

void ICACHE_FLASH_ATTR znet_timer_set( uint8_t* timer, uint64_t time ) {
    if( *timer != 0 ) {
        uint8_t tim = *timer - 1;
        if( tim <= timer_quantity ) {
            uint64_t time_now = SystemTime_Now();
            uint64_t time_ = SystemTime_ms( time );
            znet_timer[tim].finish_time = time_now + time_;
            /*
                        Log_Write( " time_now: %llu, time_: %llu, "
                                   "znet_timer[tim].finish_time: %llu \r\n",
                                   time_now, time_, znet_timer[tim].finish_time
               );
            */

            // znet_timer[tim].time_out = time / 100;
            znet_timer[tim].work = true;
        }
    }
    else {
        Log_Write( "\r\ntimer_set                U N K N O W N \r\n" );
    }
}

void ICACHE_FLASH_ATTR znet_timer_stop( uint8_t* timer ) {
    if( *timer != 0 ) {
        Log_Write( "timer_stop, timer = %d\r\n", *timer );
        uint8_t tim = *timer - 1;
        if( tim <= timer_quantity ) {
            znet_timer[tim].work = false;
        }
    }
}

void ICACHE_FLASH_ATTR znet_timer_init( uint8_t* timer, void* pfunction, void* parg ) {
    uint8_t i;
    if( timer_quantity == 0 ) {
        for( i = 0; i < MAX_TIMER; ++i )
            znet_timer[i].work = false;
    }

    for( i = 0; i < MAX_TIMER; ++i ) {
        if( ( znet_timer[i].callback == pfunction ) && ( znet_timer[i].arg == parg ) ) {
            *timer = i + 1;
            break;
        }
        if( ( znet_timer[i].callback == NULL ) && ( znet_timer[i].arg == NULL ) ) {
            timer_quantity++;
            znet_timer[i].arg = parg;
            znet_timer[i].callback = pfunction;
            znet_timer[i].work = false;
            *timer = i + 1;
            break;
        }
    }
}

void ICACHE_FLASH_ATTR znet_timer_deinit( uint8_t* timer ) {
    if( *timer != 0 ) {
        uint8_t tim = *timer - 1;

        znet_timer[tim].arg = NULL;
        znet_timer[tim].callback = NULL;
        znet_timer[tim].work = false;
        Log_Write( "Deinit timer, timer = %d\r\n", *timer );
        *timer = 0;
        timer_quantity--;
    }
}

uint64_t ICACHE_FLASH_ATTR znet_timer_get_remaining( uint8_t* timer ) {
    if( *timer == 0 )
        return 0;

    const uint8_t tim = *timer - 1;
    if( tim > timer_quantity )
        return 0;

    const SystemTime_t time_now = SystemTime_Now();
    if( time_now > znet_timer[tim].finish_time )
        return 0;

    return SystemTime_Decode_ms( znet_timer[tim].finish_time - time_now );
}
