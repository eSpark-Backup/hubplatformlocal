#pragma once

#include "scenes_manager/scenes_engine/helpers/clock_interface.h"

namespace SceneEngine {

class Clock : public IClock {
public:
    std::chrono::system_clock::time_point greenwichMeanTimePoint() override {
        return std::chrono::system_clock::now();
    }

    std::chrono::system_clock::duration greenwichMeanTimeSinceEpoch() override {
        return greenwichMeanTimePoint().time_since_epoch();
    }

    std::chrono::seconds timeZoneOffset() override {
        std::time_t current_time;
        std::time( &current_time );
        struct std::tm* timeinfo = std::localtime( &current_time );
        return std::chrono::seconds( timeinfo->tm_gmtoff );
    }

    std::chrono::system_clock::time_point localTime() override {
        return greenwichMeanTimePoint() + timeZoneOffset();
    }

    std::chrono::system_clock::duration localTimeSinceEpoch() override {
        return localTime().time_since_epoch();
    }
};

}  // namespace SceneEngine
