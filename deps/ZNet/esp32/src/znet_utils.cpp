#include <znet/znet_utils.hpp>

#include <znet/znet.h>

#include <cmath>
#include <array>
#include <time.h>
#include <numeric>

#include "znet/znet.h"

extern "C" {
#include "znet_log.h"
}

namespace znet {
namespace utils {

namespace {

using TimePoint = uint64_t;

constexpr const TimePoint SecondsInDay = 24 * 60 * 60;
constexpr const TimePoint SecondsInYear = 365 * SecondsInDay;
constexpr const znet_cmd_time_parameters_month_t LeapMonth = 2;

constexpr const auto SecondsIn4Years = SecondsInYear * 4 + SecondsInDay;
constexpr const auto SecondsIn100Years = SecondsIn4Years * ( 100 / 4 ) - SecondsInDay;
constexpr const auto SecondsIn400Years = SecondsIn100Years * ( 400 / 100 ) + SecondsInDay;
constexpr const auto MaxTimePoint =
    SecondsIn400Years * ( std::numeric_limits<znet_cmd_time_parameters_year_t>::max() / 400 );
constexpr const auto InvalidTimePoint = std::numeric_limits<TimePoint>::max();

constexpr const uint32_t DaysOffsets[12 + 1] = {
    0,
    31,
    31 + 28,
    31 + 28 + 31,
    31 + 28 + 31 + 30,
    31 + 28 + 31 + 30 + 31,
    31 + 28 + 31 + 30 + 31 + 30,
    31 + 28 + 31 + 30 + 31 + 30 + 31,
    31 + 28 + 31 + 30 + 31 + 30 + 31 + 31,
    31 + 28 + 31 + 30 + 31 + 30 + 31 + 31 + 30,
    31 + 28 + 31 + 30 + 31 + 30 + 31 + 31 + 30 + 31,
    31 + 28 + 31 + 30 + 31 + 30 + 31 + 31 + 30 + 31 + 30,
    31 + 28 + 31 + 30 + 31 + 30 + 31 + 31 + 30 + 31 + 30 + 31,
};

static_assert( DaysOffsets[12] == 365, "Check days offsets" );

znet_cmd_time_parameters_day_t getDaysCount( znet_cmd_time_parameters_month_t month, bool isLeapYear ) {
    assert( month >= 1 );
    assert( month <= 12 );

    return static_cast<znet_cmd_time_parameters_day_t>( DaysOffsets[month] - DaysOffsets[month - 1] ) +
           ( isLeapYear && month == LeapMonth );
}

uint32_t getDaysOffset( znet_cmd_time_parameters_month_t month, bool isLeapYear ) {
    assert( month >= 1 );
    assert( month <= 12 );

    return DaysOffsets[month - 1] + ( isLeapYear && month > LeapMonth );
}

znet_cmd_time_parameters_year_t getLeapYears( znet_cmd_time_parameters_year_t year ) {
    const auto isZero = year == 0;
    year -= !isZero;
    return !isZero + year / 4 + year / 400 - year / 100;
}

TimePoint fromDateTime( const znet_date_time_t& dateTime ) {
    const auto leapYears = getLeapYears( dateTime.date.year );
    const auto isLeapYear = getLeapYears( dateTime.date.year + 1 ) > leapYears;

    if( !is_date_valid( &dateTime.date ) ) {
        ZNET_LOGD(
            "fromDateTime: date %d:%d:%d is invalid\n", dateTime.date.day, dateTime.date.month, dateTime.date.year );
        return InvalidTimePoint;
    }

    if( !is_time_valid( &dateTime.time ) ) {
        ZNET_LOGD( "fromDateTime: time %d:%d:%d is invalid\n",
                   dateTime.time.hour,
                   dateTime.time.minute,
                   dateTime.time.second );
        return InvalidTimePoint;
    }

    if( dateTime.date.day > getDaysCount( dateTime.date.month, isLeapYear ) ) {
        ZNET_LOGD( "Invalid day %d in month %d year %d\n", dateTime.date.day, dateTime.date.month, dateTime.date.year );
        return InvalidTimePoint;
    }

    const auto secondsInYears = SecondsInYear * dateTime.date.year + leapYears * SecondsInDay;
    const auto secondsInMonths = getDaysOffset( dateTime.date.month, isLeapYear ) * SecondsInDay;
    const auto secondsInDays = ( dateTime.date.day - 1 ) * SecondsInDay;

    return secondsInYears + secondsInMonths + secondsInDays + dateTime.time.hour * 3600 + dateTime.time.minute * 60 +
           dateTime.time.second;
}

template <typename Value, typename TimePointGetter>
void fixTimeValue( TimePoint& timePoint, Value& value, TimePointGetter getter ) {
    for( int i = 0;; ++i, --value ) {
        const auto tempTimePoint = getter( value );
        if( timePoint >= tempTimePoint ) {
            timePoint -= tempTimePoint;
            return;
        }

        assert( i == 0 );
        assert( value );
    }
}

znet_date_time_t toDateTime( TimePoint timePoint, const znet_date_time_t* defaultTimePoint ) {
    if( timePoint > MaxTimePoint ) {
        ZNET_LOGD( "Invalid time point %lu\n", timePoint );
        return defaultTimePoint ? *defaultTimePoint : znet_date_time_t{};
    }

    znet_date_time_t result;

    result.date.year = static_cast<znet_cmd_time_parameters_year_t>(
        ( timePoint / SecondsIn400Years ) * 400 + ( timePoint % SecondsIn400Years ) / SecondsInYear );
    fixTimeValue( timePoint, result.date.year, []( znet_cmd_time_parameters_year_t year ) {
        return SecondsInDay * getLeapYears( year ) + SecondsInYear * year;
    } );

    const auto isLeapYear = getLeapYears( result.date.year + 1 ) > getLeapYears( result.date.year );

    result.date.month =
        std::min( static_cast<znet_cmd_time_parameters_month_t>( timePoint / ( SecondsInDay * 28 ) + 1 ),
                  znet_cmd_time_parameters_month_t( 12 ) );
    fixTimeValue( timePoint, result.date.month, [isLeapYear]( uint8_t month ) {
        return getDaysOffset( month, isLeapYear ) * SecondsInDay;
    } );

    result.date.day = static_cast<znet_cmd_time_parameters_day_t>( timePoint / SecondsInDay + 1 );
    timePoint %= SecondsInDay;

    result.time.second = static_cast<znet_cmd_time_parameters_second_t>( timePoint % 60 );
    timePoint /= 60;

    result.time.minute = static_cast<znet_cmd_time_parameters_minute_t>( timePoint % 60 );
    timePoint /= 60;

    result.time.hour = static_cast<znet_cmd_time_parameters_hour_t>( timePoint );
    assert( result.time.hour < 24 );

    return result;
}
}  // namespace

float valueToFloat( int32_t value, uint8_t precision ) {
    return value / std::pow( 10.0f, precision );
}

int32_t valueFromFloat( float value, uint8_t precision ) {
    return static_cast<int32_t>( value * std::pow( 10.0f, precision ) );
}

std::uint8_t getBitsCount( const std::uint8_t* data, std::uint8_t size ) {
    // TODO: optimize via table

    std::uint8_t result = 0;
    forEachBit( data, size, [&result]( std::uint8_t ) { ++result; } );

    return result;
}

std::string bitmaskToString( const std::uint8_t* data, std::uint8_t size ) {
    std::string result( size * 8, '0' );

    forEachBit( data, size, [&result]( uint8_t index ) { result[index] = '1'; } );

    return result;
}

const char* boolToYesNo( bool value ) {
    return value ? "yes" : "no";
}

const char* progressToStringStatus( bool isOk, uint8_t progress ) {
    return !isOk ? "failed" : progress == 0 ? "started" : progress >= 100 ? "finished" : "in_progress";
}

SecondsOpt actuatorReportTimeToSeconds( uint8_t time ) {
    return time < 0x80 ? SecondsOpt( std::chrono::seconds( time ) ) :
                         time < 0xFE ? SecondsOpt( std::chrono::minutes( time - 0x7F ) ) : SecondsOpt();
}

uint8_t actuatorSetTimeFromSeconds( const SecondsOpt& seconds ) {
    return !seconds ? 0xFF :
                      *seconds <= Seconds( 0x7F ) ?
                      static_cast<uint8_t>( seconds->count() ) :
                      static_cast<uint8_t>( std::min( std::chrono::duration_cast<std::chrono::minutes>( *seconds ),
                                                      std::chrono::minutes( 127 ) )
                                                .count() +
                                            0x7F );
}

utils::optional<std::uint8_t> findFirstBit( const std::uint8_t* data, std::uint8_t size, std::uint8_t bitShift ) {
    return findBit( data, size, [&]( std::uint8_t ) { return true; }, bitShift );
}

znet_clock_weekday_value_t getClockWeekDay( const znet_date_t& date ) {
    const auto tp = fromDateTime( { date, znet_time_t{ 0, 0, 0 } } );

    return tp == InvalidTimePoint ? 0 : ( tp / SecondsInDay + 5 ) % 7 + 1;
}

std::chrono::seconds getTimeDifference( const znet_date_time_t& end, const znet_date_time_t& begin ) {
    const auto tpEnd = fromDateTime( end );
    const auto tpBegin = fromDateTime( begin );

    return tpEnd != InvalidTimePoint && tpBegin != InvalidTimePoint ? std::chrono::seconds( tpEnd - tpBegin ) :
                                                                      std::chrono::seconds( 0 );
}

znet_date_time_t addTimeDifference( const znet_date_time_t& dateTime, const std::chrono::seconds& timeDiff ) {
    const auto tp = fromDateTime( dateTime );
    return toDateTime( tp + timeDiff.count() * ( tp != InvalidTimePoint ), &dateTime );
}

utils::optional<std::vector<uint8_t> > hexToBytes( const std::string& hex ) {
    if( hex.size() & 1 ) {
        return {};
    }

    auto toByte = []( char c, int& byte ) {
        if( c >= '0' && c <= '9' )
            byte = c - '0';
        else if( c >= 'A' && c <= 'F' )
            byte = 0x0A + ( c - 'A' );
        else if( c >= 'a' && c <= 'f' )
            byte = 0x0A + ( c - 'a' );
        else
            return false;

        return true;
    };

    std::vector<uint8_t> result;
    result.reserve( hex.size() / 2 );

    for( size_t i = 0; i < hex.size(); i += 2 ) {
        int left = 0, right = 0;
        if( !toByte( hex[i], left ) || !toByte( hex[i + 1], right ) ) {
            return {};
        }

        result.push_back( static_cast<uint8_t>( right | ( left << 4 ) ) );
    }

    return { std::move( result ) };
}

std::string bytesToHex( const std::vector<uint8_t>& bytes ) {
    std::string result;
    result.reserve( bytes.size() * 2 );

    auto toSymbol = []( uint8_t byte ) {
        return static_cast<char>( byte >= 0x0A ? 'A' + ( byte - 0x0A ) : '0' + byte );
    };

    for( auto byte : bytes ) {
        result.push_back( toSymbol( byte >> 4 ) );
        result.push_back( toSymbol( byte & 0x0F ) );
    }

    return result;
}

template <>
std::string stringify<err::znet_error_message_t>( const err::znet_error_message_t& err ) {
    std::stringstream ss;
    ss << "{ \"code\": " << err.m_code << ", \"data\": \"" << err.m_data << "\" ";
    if( !err.m_details.empty() ) {
        ss << ", \"details\": { ";
        auto iter = err.m_details.begin(), end = err.m_details.end();
        ss << "\"" << iter->first << "\": \"" << iter->second << "\"";
        while( ++iter != end ) {
            ss << ", \"" << iter->first << "\": \"" << iter->second << "\"";
        }
        ss << " } ";
    }
    ss << "}";
    return ss.str();
}

}  // namespace utils
}  // namespace znet

bool operator==( const znet_time_zone_settings_t& a, const znet_time_zone_settings_t& b ) {
    return a.time_zone.is_tzo_negative == b.time_zone.is_tzo_negative && a.time_zone.hour_tzo == b.time_zone.hour_tzo &&
           a.time_zone.minute_tzo == b.time_zone.minute_tzo && a.minute_dst_offset == b.minute_dst_offset &&
           a.start_dst == b.start_dst && a.end_dst == b.end_dst;
}

bool operator==( const znet_dst_time_point_t& a, const znet_dst_time_point_t& b ) {
    return a.month == b.month && a.day == b.day && a.hour == b.hour;
}

bool operator==( const znet_cmd_schedule_entry_lock_time_offset_t& a,
                 const znet_cmd_schedule_entry_lock_time_offset_t& b ) {
    return a.time_zone.is_tzo_negative == b.time_zone.is_tzo_negative && a.time_zone.hour_tzo == b.time_zone.hour_tzo &&
           a.time_zone.minute_tzo == b.time_zone.minute_tzo && a.minute_dst_offset == b.minute_dst_offset;
}

bool operator==( const znet_clock_value_t& a, const znet_clock_value_t& b ) {
    return a.hour == b.hour && a.minute == b.minute && a.weekday == b.weekday;
}
