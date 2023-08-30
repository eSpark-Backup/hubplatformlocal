#pragma once

#include <znet/znet_command_class.hpp>
#include <znet/znet_alarm_silence.hpp>

namespace znet {

class ZCc0x9dAlarmSilence final : public ZClass {
public:
    using ID = ZClassID<ZNET_COMMAND_CLASS_SILENCE_ALARM>;
    using ZClass::ZClass;

    void setSilentSensors( uint8_t channelId,
                           SensorsSoundDisablingMode mode,
                           uint16_t delay,
                           uint8_t sensors_bitmask_size,
                           const uint8_t* sensors_bitmask );

    ZNET_DEFINE_ACCEPT()
};

}  // namespace znet
