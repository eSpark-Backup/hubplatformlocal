#pragma once

#include <chrono>

class TimeMeter {
public:
    TimeMeter() : 
        m_timePoint(std::chrono::steady_clock::now()) {}

    template <typename Duration = std::chrono::milliseconds>
    Duration getElapsedTime() const {
        return std::chrono::duration_cast<Duration>(std::chrono::steady_clock::now() - m_timePoint);
    }
private:
    std::chrono::steady_clock::time_point m_timePoint;
};
