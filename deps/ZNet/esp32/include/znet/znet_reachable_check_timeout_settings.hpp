#pragma once

#include <chrono>

namespace znet {

enum class ReachableCheckTimeoutPolicy;

class ReachableCheckTimeoutSettings {
public:
    ReachableCheckTimeoutSettings();

    ReachableCheckTimeoutSettings( bool useFixedWakeUpInterval,
                                   uint32_t wakeUpInterval,
                                   uint32_t alwaysOnDevicesInetrval,
                                   uint8_t attemptsCount );

    void update( bool useFixedWakeUpInterval,
                 uint32_t wakeUpInterval,
                 uint32_t alwaysOnDevicesInetrval,
                 uint8_t attemptsCount );

    bool useFixedWakeUpInterval() const;
    uint32_t getWakeUpInterval() const;
    uint32_t getAlwaysOnDevicesInterval() const;
    uint8_t getAttemptsCount() const;

private:
    bool m_useFixedWakeUpInterval;
    uint32_t m_wakeUpInterval;
    uint32_t m_alwaysOnDevicesInetrval;
    uint8_t m_attemptsCount;
};

}  // namespace znet
