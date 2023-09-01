
#pragma once

#include <znet/znet_command_class.hpp>

namespace znet {

class ZCc0x2CSceneActuatorConf final : public ZClass {
public:
    using ID = ZClassID<ZNET_COMMAND_CLASS_SCENE_ACTUATOR_CONF>;
    using ZClass::ZClass;

    bool setConfiguration( uint8_t channelId,
                           uint8_t sceneId,
                           uint8_t dimmingDuration,
                           uint8_t isOverride,
                           uint8_t level );
    bool requestValue( const RequestValue& request ) override;

    ZNET_DEFINE_ACCEPT()

private:
    uint8_t m_sceneId = 0;
    uint8_t m_dimmingDuration = 0;
};

}  // namespace znet
