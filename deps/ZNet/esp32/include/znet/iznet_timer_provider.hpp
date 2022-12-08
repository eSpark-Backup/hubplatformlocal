#pragma once

#include <chrono>
#include <memory>
#include <functional>

namespace znet {

class ITimerProvider {
public:
    using TimerInterval = std::chrono::milliseconds;
    using TimerCallback = std::function<void()>;

    class Timer {
    public:
        virtual ~Timer() = default;
        virtual void runPeriodically( TimerInterval first, TimerInterval interval ) = 0;
        virtual void runOnce( TimerInterval first ) = 0;
    };

    using TimerUPtr = std::unique_ptr<Timer>;

public:
    virtual ~ITimerProvider() = default;

    virtual TimerUPtr createTimer( TimerCallback cb ) = 0;
};

}  // namespace znet
