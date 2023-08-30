#pragma once

#include <chrono>

namespace SceneEngine {

class IClock {
public:
    virtual ~IClock() = default;

    virtual std::chrono::system_clock::time_point greenwichMeanTimePoint() = 0;
    virtual std::chrono::system_clock::duration greenwichMeanTimeSinceEpoch() = 0;

    virtual std::chrono::system_clock::time_point localTime() = 0;
    virtual std::chrono::system_clock::duration localTimeSinceEpoch() = 0;

    virtual std::chrono::seconds timeZoneOffset() = 0;
};

}  // namespace SceneEngine
