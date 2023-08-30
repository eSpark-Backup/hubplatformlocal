#include "znet/znet_default_date_time_provider.hpp"

#include <chrono>
#include <utility>
#include <cmath>
#include <cassert>

#include <znet/znet_utils.hpp>

extern "C" {
#include "znet_log.h"
}

namespace znet {

namespace {

struct DstTime {
    time_t time;
    int dstOffset;
};

using DstTimeRange = std::pair<DstTime, DstTime>;

DefaultDateTimeProvider::DateTime timeInfoToDateTime( const tm& timeInfo ) {
    DefaultDateTimeProvider::DateTime result;

    result.date.day = static_cast<uint8_t>( timeInfo.tm_mday );
    result.date.month = static_cast<uint8_t>( timeInfo.tm_mon + 1 );
    result.date.year = static_cast<uint16_t>( timeInfo.tm_year + 1900 );

    result.time.hour = static_cast<uint8_t>( timeInfo.tm_hour );
    result.time.minute = static_cast<uint8_t>( timeInfo.tm_min );
    result.time.second = static_cast<uint8_t>( timeInfo.tm_sec );

    return result;
}

std::chrono::seconds getTmOffset( time_t t, const tm& timeInfo ) {
    DefaultDateTimeProvider::DateTime beginDateTime;
    beginDateTime.date.day = 1;
    beginDateTime.date.month = 1;
    beginDateTime.date.year = 1970;
    beginDateTime.time = {};

    return std::chrono::duration_cast<std::chrono::seconds>(
        utils::getTimeDifference( timeInfoToDateTime( timeInfo ), beginDateTime ) -
        std::chrono::system_clock::from_time_t( t ).time_since_epoch() );
}

DefaultDateTimeProvider::DateTime getDateTime( bool isLocal ) {
    auto now = ::time( nullptr );
    struct tm timeInfo {};
    ( isLocal ? localtime_r : gmtime_r )( &now, &timeInfo );

    return timeInfoToDateTime( timeInfo );
}

bool isDstTimeValid( const DstTime& dstTime ) {
    return dstTime.time != 0;
}

DstTime makeDstTime( time_t time, int dstOffset ) {
    return { time, dstOffset };
}

DstTime makeDstTime( time_t time ) {
    return makeDstTime( time, localtime( &time )->tm_isdst );
}

DstTime makeMiddleDstTime( const DstTime& start, const DstTime& end ) {
    assert( end.time >= start.time );
    return makeDstTime( start.time + ( end.time - start.time ) / 2 );
}

DstTime findTimeShift( DstTime start, DstTime end ) {
    assert( start.dstOffset != end.dstOffset );

    while( end.time - start.time > 1 ) {
        const auto mid = makeMiddleDstTime( start, end );
        if( mid.dstOffset == start.dstOffset ) {
            start.time = mid.time;
        }
        else if( mid.dstOffset == end.dstOffset ) {
            end.time = mid.time;
        }
        else {
            ZNET_LOGE( "DST shift finding error: found offset %d is incorrect\n", mid.dstOffset );
            return DstTime{ 0, 0 };
        }
    }

    return end;
}

DstTime findAnyDstTime( DstTime& start, DstTime& end ) {
    assert( start.dstOffset == end.dstOffset );

    const auto mid = makeMiddleDstTime( start, end );
    if( mid.dstOffset != start.dstOffset ) {
        return mid;
    }

    auto mid2 = makeMiddleDstTime( start, mid );
    if( mid2.dstOffset != start.dstOffset ) {
        end = mid;
        return mid2;
    }

    mid2 = makeMiddleDstTime( mid, end );
    if( mid2.dstOffset != start.dstOffset ) {
        start = mid;
        return mid2;
    }

    return makeDstTime( 0, 0 );
}

DstTimeRange findDstRange( DstTime start, DstTime end ) {
    if( start.dstOffset != end.dstOffset ) {
        ZNET_LOGE( "Dst rand finding error: start.dstOffset != end.dstOffset\n" );
        return std::make_pair( DstTime{ 0, 0 }, DstTime{ 0, 0 } );
    }

    const auto anyDstTime = findAnyDstTime( start, end );
    if( !isDstTimeValid( anyDstTime ) ) {
        // current TZ doesn't have dst offset: it's ok.
        return std::make_pair( DstTime{ 0, 0 }, DstTime{ 0, 0 } );
    }

    const auto result = std::make_pair( findTimeShift( start, anyDstTime ), findTimeShift( anyDstTime, end ) );

    if( !isDstTimeValid( result.first ) || !isDstTimeValid( result.second ) ) {
        std::make_pair( DstTime{ 0, 0 }, DstTime{ 0, 0 } );
    }

    return result;
}

}  // namespace

DefaultDateTimeProvider::DateTime DefaultDateTimeProvider::getUtcDateTime() const {
    return getDateTime( false );
}

DefaultDateTimeProvider::DateTime DefaultDateTimeProvider::getLocalDateTime() const {
    return getDateTime( true );
}

DefaultDateTimeProvider::TimeZoneSettings DefaultDateTimeProvider::getTimeZoneSettings() const {
    auto getYearStart = []( int year ) {
        struct tm timeInfo {};
        timeInfo.tm_year = year;
        timeInfo.tm_mday = 1;
        return mktime( &timeInfo );
    };

    auto getZnetDstOffsetByTime = []( time_t time ) {
        struct tm timeInfo {};
        localtime_r( &time, &timeInfo );

        const auto dateTime =
            utils::addTimeDifference( timeInfoToDateTime( timeInfo ), std::chrono::hours( -timeInfo.tm_isdst ) );

        return znet_dst_time_point_t{
            .month = dateTime.date.month, .day = dateTime.date.day, .hour = dateTime.time.hour
        };
    };

    TimeZoneSettings result = {};

    const auto now = ::time( nullptr );
    struct tm timeInfo {};
    localtime_r( &now, &timeInfo );

    const auto yearStart = getYearStart( timeInfo.tm_year );
    const auto nextYearStart = getYearStart( timeInfo.tm_year + 1 );

    localtime_r( &yearStart, &timeInfo );

    const std::chrono::seconds minutesOffset =
        getTmOffset( yearStart, timeInfo ) - std::chrono::hours( timeInfo.tm_isdst );

    result.time_zone.is_tzo_negative = minutesOffset.count() < 0;
    result.time_zone.hour_tzo =
        std::abs( std::chrono::duration_cast<std::chrono::hours>( minutesOffset ).count() ) & 0x7F;
    result.time_zone.minute_tzo = std::abs( minutesOffset.count() ) % 60;

    const auto dstRange = findDstRange( makeDstTime( yearStart, timeInfo.tm_isdst ), makeDstTime( nextYearStart ) );

    if( isDstTimeValid( dstRange.first ) && isDstTimeValid( dstRange.second ) ) {
        result.start_dst = getZnetDstOffsetByTime( dstRange.first.time );
        result.end_dst = getZnetDstOffsetByTime( dstRange.second.time );
        result.minute_dst_offset = static_cast<int8_t>( dstRange.first.dstOffset * 60 );
    }

    return result;
}

void DefaultDateTimeProvider::setTimeZoneSettings( const char* zoneName, const char* zone ) {
    // Platform specific
    // Nothing to do
}

}  // namespace znet
