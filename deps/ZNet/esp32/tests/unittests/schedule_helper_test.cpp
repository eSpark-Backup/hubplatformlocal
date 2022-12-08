#include "znet_schedule_helper.hpp"
#include "znet/znet.h"
#include "znet/znet_utils.hpp"

#include <gtest/gtest.h>

static void PrintTo( const znet_daily_repeating_schedule_t& schedule, ::std::ostream* os ) {
    *os << "schedule_id: " << static_cast<int>( schedule.schedule_id.user_id ) << ","
        << static_cast<int>( schedule.schedule_id.slot_id )
        << "; week_day_bitmask: " << static_cast<int>( schedule.week_day_bitmask )
        << "; start_hour: " << static_cast<int>( schedule.start_hour )
        << "; start_min: " << static_cast<int>( schedule.start_min )
        << "; duration_hour: " << static_cast<int>( schedule.duration_hour )
        << "; duration_min: " << static_cast<int>( schedule.duration_min );
}

static bool operator==( const znet_schedule_id_t& a, const znet_schedule_id_t& b ) {
    return a.slot_id == b.slot_id && a.user_id == b.user_id;
}

static bool operator==( const znet_daily_repeating_schedule_t& a, const znet_daily_repeating_schedule_t& b ) {
    return a.schedule_id == b.schedule_id && a.week_day_bitmask == b.week_day_bitmask && a.start_hour == b.start_hour &&
           a.start_min == b.start_min && a.duration_hour == b.duration_hour && a.duration_min == b.duration_min;
}

namespace znet {
namespace ut {

TEST( ScheduleHelperTestSuite, adjustDailyRepeationgSchedule ) {
    const znet_daily_repeating_schedule_t schedule{ .schedule_id = { .user_id = 1, .slot_id = 2 },
                                                    .week_day_bitmask = ( 1 << 3 ) | ( 1 << 6 ),
                                                    .start_hour = 23,
                                                    .start_min = 59,
                                                    .duration_hour = 1,
                                                    .duration_min = 10 };
    EXPECT_EQ( ScheduleHelper::adjustSchedule( schedule, {} ), schedule );
    EXPECT_TRUE( is_daily_repeating_valid( &schedule ) );

    const znet_daily_repeating_schedule_t schedulePlusOffset{ .schedule_id = { .user_id = 1, .slot_id = 2 },
                                                              .week_day_bitmask = ( 1 << 4 ) | ( 1 << 0 ),
                                                              .start_hour = 1,
                                                              .start_min = 2,
                                                              .duration_hour = 1,
                                                              .duration_min = 10 };
    EXPECT_EQ( ScheduleHelper::adjustSchedule( schedule, std::chrono::minutes( 63 ) ), schedulePlusOffset );
    EXPECT_TRUE( is_daily_repeating_valid( &schedulePlusOffset ) );

    const znet_daily_repeating_schedule_t scheduleMinusOffset{ .schedule_id = { .user_id = 1, .slot_id = 2 },
                                                               .week_day_bitmask = ( 1 << 3 ) | ( 1 << 6 ),
                                                               .start_hour = 22,
                                                               .start_min = 57,
                                                               .duration_hour = 1,
                                                               .duration_min = 10 };
    EXPECT_EQ( ScheduleHelper::adjustSchedule( schedule, std::chrono::minutes( -62 ) ), scheduleMinusOffset );
    EXPECT_TRUE( is_daily_repeating_valid( &scheduleMinusOffset ) );
}

TEST( ScheduleHelperTestSuite, adjustDailyRepeationgScheduleBigTimeOffsets ) {
    const znet_daily_repeating_schedule_t schedule{ .schedule_id = { .user_id = 1, .slot_id = 2 },
                                                    .week_day_bitmask = ( 1 << 0 ) | ( 1 << 5 ),
                                                    .start_hour = 0,
                                                    .start_min = 6,
                                                    .duration_hour = 1,
                                                    .duration_min = 20 };
    EXPECT_EQ( ScheduleHelper::adjustSchedule( schedule, {} ), schedule );
    EXPECT_TRUE( is_daily_repeating_valid( &schedule ) );

    const znet_daily_repeating_schedule_t schedulePlusOffset{ .schedule_id = { .user_id = 1, .slot_id = 2 },
                                                              .week_day_bitmask = ( 1 << 0 ) | ( 1 << 5 ),
                                                              .start_hour = 23,
                                                              .start_min = 8,
                                                              .duration_hour = 1,
                                                              .duration_min = 20 };
    EXPECT_EQ( ScheduleHelper::adjustSchedule( schedule, std::chrono::minutes( 23 * 60 + 2 ) ), schedulePlusOffset );
    EXPECT_TRUE( is_daily_repeating_valid( &schedulePlusOffset ) );

    const znet_daily_repeating_schedule_t scheduleMinusOffset{ .schedule_id = { .user_id = 1, .slot_id = 2 },
                                                               .week_day_bitmask = ( 1 << 6 ) | ( 1 << 4 ),
                                                               .start_hour = 23,
                                                               .start_min = 59,
                                                               .duration_hour = 1,
                                                               .duration_min = 20 };
    EXPECT_EQ( ScheduleHelper::adjustSchedule( schedule, std::chrono::minutes( -7 ) ), scheduleMinusOffset );
    EXPECT_TRUE( is_daily_repeating_valid( &scheduleMinusOffset ) );

    const znet_daily_repeating_schedule_t scheduleMinusBigOffset{ .schedule_id = { .user_id = 1, .slot_id = 2 },
                                                                  .week_day_bitmask = ( 1 << 5 ) | ( 1 << 3 ),
                                                                  .start_hour = 0,
                                                                  .start_min = 0,
                                                                  .duration_hour = 1,
                                                                  .duration_min = 20 };
    EXPECT_EQ( ScheduleHelper::adjustSchedule( schedule, std::chrono::minutes( -6 - 60 * 24 * ( 2 + 7 * 10 ) ) ),
               scheduleMinusBigOffset );
    EXPECT_TRUE( is_daily_repeating_valid( &scheduleMinusBigOffset ) );

    const znet_daily_repeating_schedule_t schedulePlusBigOffset{ .schedule_id = { .user_id = 1, .slot_id = 2 },
                                                                 .week_day_bitmask = ( 1 << 3 ) | ( 1 << 1 ),
                                                                 .start_hour = 0,
                                                                 .start_min = 0,
                                                                 .duration_hour = 1,
                                                                 .duration_min = 20 };
    EXPECT_EQ( ScheduleHelper::adjustSchedule( schedule, std::chrono::minutes( -6 + 60 * 24 * ( 3 + 7 * 99 ) ) ),
               schedulePlusBigOffset );
    EXPECT_TRUE( is_daily_repeating_valid( &scheduleMinusBigOffset ) );
}

}  // namespace ut
}  // namespace znet
