#pragma once

#include <znet/iznet_timer_provider.hpp>

namespace znet {

class DefaultTimerProvider : public virtual ITimerProvider {
public:
    TimerUPtr createTimer( TimerCallback cb ) override;
};

}  // namespace znet
