#pragma once

#include <znet/znet.h>

#include <chrono>

namespace znet {

class ScheduleHelper {
public:
    static znet_year_day_schedule_t adjustSchedule( const znet_year_day_schedule_t& schedule,
                                                    std::chrono::minutes timeDifference );

    static znet_daily_repeating_schedule_t adjustSchedule( const znet_daily_repeating_schedule_t& schedule,
                                                           std::chrono::minutes timeDifference );
};

}  // namespace znet
