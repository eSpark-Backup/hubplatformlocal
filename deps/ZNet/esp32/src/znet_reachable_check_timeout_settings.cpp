#include "znet/znet_reachable_check_timeout_settings.hpp"

#include "stdio.h"

namespace znet {

ReachableCheckTimeoutSettings::ReachableCheckTimeoutSettings()
: m_useFixedWakeUpInterval( false )
, m_wakeUpInterval( 5400 )
, m_alwaysOnDevicesInetrval( 10800 )  // 3 hours
, m_attemptsCount( 3 ) {}

ReachableCheckTimeoutSettings::ReachableCheckTimeoutSettings( bool useFixedWakeUpInterval,
                                                              uint32_t wakeUpInterval,
                                                              uint32_t alwaysOnDevicesInetrval,
                                                              uint8_t attemptsCount )
: m_useFixedWakeUpInterval( useFixedWakeUpInterval )
, m_wakeUpInterval( wakeUpInterval )
, m_alwaysOnDevicesInetrval( alwaysOnDevicesInetrval )
, m_attemptsCount( attemptsCount ) {}

void ReachableCheckTimeoutSettings::update( bool useFixedWakeUpInterval,
                                            uint32_t wakeUpInterval,
                                            uint32_t alwaysOnDevicesInetrval,
                                            uint8_t attemptsCount ) {
    m_useFixedWakeUpInterval = useFixedWakeUpInterval;
    m_wakeUpInterval = wakeUpInterval;
    m_alwaysOnDevicesInetrval = alwaysOnDevicesInetrval;
    m_attemptsCount = attemptsCount;
}

bool ReachableCheckTimeoutSettings::useFixedWakeUpInterval() const {
    return m_useFixedWakeUpInterval;
}

uint32_t ReachableCheckTimeoutSettings::getWakeUpInterval() const {
    return m_wakeUpInterval;
}

uint32_t ReachableCheckTimeoutSettings::getAlwaysOnDevicesInterval() const {
    return m_alwaysOnDevicesInetrval;
}

uint8_t ReachableCheckTimeoutSettings::getAttemptsCount() const {
    return m_attemptsCount;
}

}  // namespace znet
