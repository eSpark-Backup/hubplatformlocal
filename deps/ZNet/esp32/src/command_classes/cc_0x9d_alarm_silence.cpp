#include <znet/cc_0x9d_alarm_silence.hpp>
#include <znet/znet_node.hpp>

extern "C" {
#include <znet_log.h>
}

namespace znet {

void ZCc0x9dAlarmSilence::setSilentSensors( uint8_t channelId,
                                            SensorsSoundDisablingMode mode,
                                            uint16_t delay,
                                            uint8_t sensors_bitmask_size,
                                            const uint8_t* sensors_bitmask ) {
    ZNET_LOGD( "set silent sensors %i %i %i\n", mode, delay, sensors_bitmask_size );

    const auto res = znet_node_cmd_alarm_silence_set( getParentNode().getId(),
                                                      channelId,
                                                      static_cast<uint8_t>( mode ),
                                                      delay,
                                                      sensors_bitmask_size,
                                                      sensors_bitmask );

    if( res != ZNET_ERR_OK ) {
        ZNET_LOGE( "Fail to set silent sensors. Error: %i\n", res );
    }
}

}  // namespace znet
