#pragma once

#include <znet/iznet_timer_provider.hpp>
#include <map>

namespace znet {
namespace ut {

class FakeTimerProvider : public virtual ITimerProvider {
public:
    class FakeTimer;

    TimerUPtr createTimer( TimerCallback cb ) override;

    void addTime( TimerInterval interval );

private:
    uint32_t m_index = 0;
    std::map<uint32_t, FakeTimer&> m_timers;
    std::chrono::system_clock::time_point m_timePoint;
};

}  // namespace ut
}  // namespace znet
