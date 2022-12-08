#include "znet_time_manager.hpp"

#include "znet/iznet_date_time_provider.hpp"
#include "znet/iznet_nodes_provider.hpp"
#include "znet/znet_node.hpp"
#include "znet/cc_0x8a_time.hpp"
#include "znet/cc_0x8b_time_parameters.hpp"
#include "znet/cc_0x4e_schedule_entry_lock.hpp"
#include "znet/cc_0x81_clock.hpp"

#include "znet/converters/time_zones_converter.hpp"

#include "znet/znet_utils.hpp"

#include <algorithm>
#include <cmath>

extern "C" {
#include "znet_log.h"
}

namespace znet {

namespace {

constexpr std::chrono::seconds DateTimeWaitingInterval( 100 );

}  // namespace

TimeManager::TimeManager( IDateTimeProvider& dateTimeProvider,
                          INodesProvider& nodesProvider,
                          ITimerProvider& timerProvider )
: NodesProcessor( nodesProvider, timerProvider, DateTimeWaitingInterval )
, m_dateTimeProvider( dateTimeProvider ) {}

void TimeManager::onTimeRequested( znet_node_id_t nodeId, znet_node_channel_id_t channelId, uint8_t encap ) {
    ZCc0x8ATime::sendTimeRespond( nodeId, channelId, m_dateTimeProvider.getLocalDateTime().time, encap );
}

void TimeManager::onDateRequested( znet_node_id_t nodeId, znet_node_channel_id_t channelId, uint8_t encap ) {
    ZCc0x8ATime::sendDateRespond( nodeId, channelId, m_dateTimeProvider.getLocalDateTime().date, encap );
}

void TimeManager::onTimeZoneRequested( znet_node_id_t nodeId, znet_node_channel_id_t channelId, uint8_t encap ) {
    const auto timeZoneSettings = m_setedTimeZone ? *m_setedTimeZone : m_dateTimeProvider.getTimeZoneSettings();

    ZCc0x8ATime::sendTimeZoneRespond( nodeId, channelId, timeZoneSettings, encap );
}

void TimeManager::onTimeZoneSet( znet_node_id_t nodeId,
                                 znet_node_channel_id_t channelId,
                                 const znet_time_zone_settings_t& timeZoneSettings ) {
    m_setedTimeZone.reset( new znet_time_zone_settings_t( timeZoneSettings ) );

    auto hoursTzo = time_zones_converter::getTimeZone( timeZoneSettings.time_zone.is_tzo_negative ?
                                                           -timeZoneSettings.time_zone.hour_tzo :
                                                           timeZoneSettings.time_zone.hour_tzo );

    if( strlen( hoursTzo.first ) != 0 ) {
        m_dateTimeProvider.setTimeZoneSettings( hoursTzo.first, hoursTzo.second );
    }
}

bool TimeManager::processNode( ZNode& ) {
    m_atteptsCount = 0;
    return requestClock();
}

bool TimeManager::requestClock( znet_node_channel_id_t prevChannelId ) {
    const auto clockCc = getCurrentNode()->getNextCommandClass<ZCc0x81Clock>( prevChannelId );

    if( !clockCc ) {
        return requestTimeParameters();
    }

    const auto roundedLocalDateTime =
        utils::addTimeDifference( m_dateTimeProvider.getLocalDateTime(), std::chrono::seconds( 30 ) );

    const znet_clock_value_t clockValue{ .hour = roundedLocalDateTime.time.hour,
                                         .minute = roundedLocalDateTime.time.minute,
                                         .weekday = utils::getClockWeekDay( roundedLocalDateTime.date ) };

    const auto channelId = clockCc->getParentChannel().getId();

    ZNET_LOGI( "Set clock value, channel %u. clock value: %d week, %d:%d\n",
               channelId,
               clockValue.weekday,
               clockValue.hour,
               clockValue.minute );

    clockCc->setTime( clockValue );

    return clockCc->requestTime(
        [=]( const znet_clock_value_t& reportedClockValue ) {
            if( reportedClockValue == clockValue ) {
                ZNET_LOGI( "Clock value was setted correctly, channel %u\n", channelId );
            }
            else {
                ZNET_LOGW( "Reported clock value differs, channel %u. clock value: %d week, %d:%d\n",
                           channelId,
                           reportedClockValue.weekday,
                           reportedClockValue.hour,
                           reportedClockValue.minute );
            }

            requestClock( channelId );
        },
        [this]( RequestFailType failType ) { onFail( "Clock", failType ); },
        getJobContext() );
}

bool TimeManager::requestTimeParameters() {
    const auto timePrmCc = getCurrentNode()->getNextCommandClass<ZCc0x8BTimeParameters>();
    if( !timePrmCc ) {
        return requestScheduleTimeZone();
    }

    if( getCurrentNode()->getNextCommandClass<ZCc0x8BTimeParameters>( timePrmCc ) ) {
        ZNET_LOGE( "There are more one channel with time parameters cc in node %d. How to handle it???\n",
                   getCurrentNodeId() );
    }

    const auto utcDateTime = m_dateTimeProvider.getUtcDateTime();

    ZNET_LOGI( "Set time params %d:%d:%d %d:%d:%d\n",
               utcDateTime.date.year,
               utcDateTime.date.month,
               utcDateTime.date.day,
               utcDateTime.time.hour,
               utcDateTime.time.minute,
               utcDateTime.time.second );

    timePrmCc->setValue( utcDateTime.date, utcDateTime.time );

    return timePrmCc->requestTimeParameters(
        [utcDateTime, this]( const znet_date_time_t& reportedDateTime ) {
            const auto timeDifference = utils::getTimeDifference( reportedDateTime, utcDateTime );
            if( timeDifference.count() == 0 ) {
                ZNET_LOGI( "Time parameters are installed correctly\n" );
            }
            else {
                ZNET_LOGE( "Reported time parameters differs: %d sec\n", timeDifference.count() );
            }

            requestScheduleTimeZone();
        },
        [=]( RequestFailType failType ) { onFail( "Time parameters", failType ); },
        getJobContext() );
}

bool TimeManager::requestScheduleTimeZone() {
    const auto scheduleEntryLockCc = getCurrentNode()->getNextCommandClass<ZCc0x4EScheduleEntryLock>();
    if( !scheduleEntryLockCc || !scheduleEntryLockCc->isTimeZoneSupported() ) {
        return requestTimeZone();
    }

    const auto timeZoneSettings = m_dateTimeProvider.getTimeZoneSettings();

    ZNET_LOGI( "Set schedule time zone. tz: %c%d:%d  dst: %dmin\n",
               timeZoneSettings.time_zone.is_tzo_negative ? '-' : '+',
               timeZoneSettings.time_zone.hour_tzo,
               timeZoneSettings.time_zone.minute_tzo,

               static_cast<int>( timeZoneSettings.minute_dst_offset ) );

    const znet_cmd_schedule_entry_lock_time_offset_t timeOffset{ timeZoneSettings.time_zone,
                                                                 timeZoneSettings.minute_dst_offset };

    scheduleEntryLockCc->setTimeOffset( timeOffset );

    return scheduleEntryLockCc->requestTimeZone(
        [this, timeOffset]( const znet_cmd_schedule_entry_lock_time_offset_t& reportedTimeOffset ) {
            if( timeOffset == reportedTimeOffset ) {
                ZNET_LOGI( "Schedule time zone settings are installed correctly\n" );
            }
            else {
                ZNET_LOGW( "Reported schedule time zone differs. tz: %c%d:%d  dst: %dmin\n",
                           reportedTimeOffset.time_zone.is_tzo_negative ? '-' : '+',
                           reportedTimeOffset.time_zone.hour_tzo,
                           reportedTimeOffset.time_zone.minute_tzo,

                           static_cast<int>( reportedTimeOffset.minute_dst_offset ) );
            }

            requestTimeZone();
        },
        [this]( RequestFailType failType ) { onFail( "Schedule time zone settings", failType ); },
        getJobContext() );
}

bool TimeManager::requestTimeZone() {
    const auto timeCc = getCurrentNode()->getNextCommandClass<ZCc0x8ATime>();
    if( !timeCc || !timeCc->isTimeZoneSupported() ) {
        return requestDateTime();
    }

    const auto timeZoneSettings = m_dateTimeProvider.getTimeZoneSettings();

    ZNET_LOGI( "Set time zone. tz: %c%d:%d  dst: %dmin [%d:%d:%d %d:%d:%d]\n",
               timeZoneSettings.time_zone.is_tzo_negative ? '-' : '+',
               timeZoneSettings.time_zone.hour_tzo,
               timeZoneSettings.time_zone.minute_tzo,

               static_cast<int>( timeZoneSettings.minute_dst_offset ),

               timeZoneSettings.start_dst.month,
               timeZoneSettings.start_dst.hour,
               timeZoneSettings.start_dst.day,

               timeZoneSettings.end_dst.month,
               timeZoneSettings.end_dst.day,
               timeZoneSettings.end_dst.hour );

    timeCc->setTimeZone( timeZoneSettings );

    return timeCc->requestTimeZone(
        [timeZoneSettings, this]( const znet_time_zone_settings_t& reportedTimeZoneSettings ) {
            if( timeZoneSettings == reportedTimeZoneSettings ) {
                ZNET_LOGI( "Time zone settings are installed correctly\n" );
            }
            else {
                ZNET_LOGW( "Reported time zone differs. tz: %c%d:%d  dst: %dmin [%d:%d:%d %d:%d:%d]\n",
                           reportedTimeZoneSettings.time_zone.is_tzo_negative ? '-' : '+',
                           reportedTimeZoneSettings.time_zone.hour_tzo,
                           reportedTimeZoneSettings.time_zone.minute_tzo,

                           static_cast<int>( timeZoneSettings.minute_dst_offset ),

                           reportedTimeZoneSettings.start_dst.month,
                           reportedTimeZoneSettings.start_dst.hour,
                           reportedTimeZoneSettings.start_dst.day,

                           reportedTimeZoneSettings.end_dst.month,
                           reportedTimeZoneSettings.end_dst.day,
                           reportedTimeZoneSettings.end_dst.hour );
            }

            requestDateTime();
        },
        [this]( RequestFailType failType ) { onFail( "Time zone settings", failType ); },
        getJobContext() );
}

bool TimeManager::requestDateTime() {
    const auto timeCc = getCurrentNode()->getNextCommandClass<ZCc0x8ATime>();
    if( !timeCc ) {
        onNodeProcessed( {} );
        return false;
    }

    if( getCurrentNode()->getNextCommandClass<ZCc0x8ATime>( timeCc ) ) {
        ZNET_LOGE( "There are more one channel with time cc in node %d. How to handle it?\n", getCurrentNodeId() );
    }

    return timeCc->requestDateAndTime(
        [=]( const znet_date_time_t& dateTime ) {
            const auto localDateTime = m_dateTimeProvider.getLocalDateTime();
            const auto timeDiff = utils::getTimeDifference<std::chrono::seconds>( localDateTime, dateTime );

            const auto roundedTimeDiffMinutes =
                static_cast<std::chrono::minutes::rep>( round( timeDiff.count() / 60. ) );

            if( roundedTimeDiffMinutes > 0 && ++m_atteptsCount != ms_maxAttemptsCount ) {
                // Another attempt to synchronize time
                requestClock();
                return;
            }

            ZNET_LOGI( "Date and time received, setup node %d time diff: %lld min\n",
                       getCurrentNodeId(),
                       roundedTimeDiffMinutes );
            getCurrentNode()->setTimeOffset( std::chrono::minutes{ roundedTimeDiffMinutes } );

            onNodeProcessed( {} );
        },
        [this]( RequestFailType failType ) { onFail( "Date and time", failType ); },
        getJobContext() );
}

void TimeManager::onFail( const char* requestName, RequestFailType failType ) {
    ZNET_LOGE( "%s request failed, setup default node %d time diff\n", requestName, getCurrentNodeId() );
    getCurrentNode()->setTimeOffset( ZNode::TimeDifference{ 0 } );
    onNodeProcessed( err::time::time_request_failed( getCurrentNodeId(), requestName, failType ) );
}

}  // namespace znet
