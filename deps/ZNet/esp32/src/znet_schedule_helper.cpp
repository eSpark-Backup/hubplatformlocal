#include "znet_schedule_helper.hpp"

#include <znet/znet_utils.hpp>
#include <cassert>

extern "C" {
#include "znet_log.h"
}

namespace znet {

namespace {
using MinutesDuration = int;

constexpr uint8_t DaysInWeek{ 7 };

constexpr MinutesDuration MinutesInHour{ 60 };
constexpr MinutesDuration MinutesInDay = MinutesInHour * 24;
constexpr MinutesDuration MinutesInWeek = MinutesInDay * DaysInWeek;

constexpr uint8_t HoursInDay{ 24 };

}  // namespace

znet_year_day_schedule_t ScheduleHelper::adjustSchedule( const znet_year_day_schedule_t& schedule,
                                                         std::chrono::minutes timeDifference ) {
    znet_year_day_schedule_t result{ .schedule_id = schedule.schedule_id };

    znet_date_time_t tempDateTime{};

    tempDateTime.date.year = schedule.start_year;
    tempDateTime.date.month = schedule.start_month;
    tempDateTime.date.day = schedule.start_day;
    tempDateTime.time.hour = schedule.start_hour;
    tempDateTime.time.minute = schedule.start_minute;
    tempDateTime.time.second = 0;

    tempDateTime = utils::addTimeDifference( tempDateTime, timeDifference );
    result.start_year = tempDateTime.date.year;
    result.start_month = tempDateTime.date.month;
    result.start_day = tempDateTime.date.day;
    result.start_hour = tempDateTime.time.hour;
    result.start_minute = tempDateTime.time.minute;

    tempDateTime.date.year = schedule.stop_year;
    tempDateTime.date.month = schedule.stop_month;
    tempDateTime.date.day = schedule.stop_day;
    tempDateTime.time.hour = schedule.stop_hour;
    tempDateTime.time.minute = schedule.stop_minute;
    tempDateTime.time.second = 0;

    tempDateTime = utils::addTimeDifference( tempDateTime, timeDifference );
    result.stop_year = tempDateTime.date.year;
    result.stop_month = tempDateTime.date.month;
    result.stop_day = tempDateTime.date.day;
    result.stop_hour = tempDateTime.time.hour;
    result.stop_minute = tempDateTime.time.minute;

    return result;
}

znet_daily_repeating_schedule_t ScheduleHelper::adjustSchedule( const znet_daily_repeating_schedule_t& schedule,
                                                                std::chrono::minutes timeDifference ) {
    using Days = std::chrono::duration<int64_t, std::ratio<60 * MinutesInHour * HoursInDay>>;
    using Weeks = std::chrono::duration<int64_t, std::ratio<60 * MinutesInWeek>>;

    const auto startTime = std::chrono::minutes( schedule.start_min ) + std::chrono::hours( schedule.start_hour );
    auto adjustedStartTime = startTime + timeDifference;
    adjustedStartTime %= MinutesInWeek;
    if( adjustedStartTime.count() < 0 )
        adjustedStartTime += Weeks( 1 );

    assert( adjustedStartTime.count() >= 0 );

    auto daysOffset = std::chrono::duration_cast<Days>( adjustedStartTime );
    adjustedStartTime -= daysOffset;

    uint32_t adjustedDayBitmask = schedule.week_day_bitmask & ZNET_CMD_SCHEDULE_ENTRY_LOCK_ALL_WEEK_DAYS_MASK;

    if( daysOffset.count() ) {
        adjustedDayBitmask <<= daysOffset.count();
        adjustedDayBitmask |= adjustedDayBitmask >> DaysInWeek;
        adjustedDayBitmask &= ZNET_CMD_SCHEDULE_ENTRY_LOCK_ALL_WEEK_DAYS_MASK;
    }

    return { .schedule_id = schedule.schedule_id,
             .week_day_bitmask = static_cast<uint8_t>( adjustedDayBitmask ),
             .start_hour = static_cast<uint8_t>( adjustedStartTime.count() / MinutesInHour ),
             .start_min = static_cast<uint8_t>( adjustedStartTime.count() % MinutesInHour ),
             .duration_hour = schedule.duration_hour,
             .duration_min = schedule.duration_min };
}

}  // namespace znet
