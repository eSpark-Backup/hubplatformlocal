#pragma once
#include <znet/znet_command_class.hpp>

namespace znet {

class ZCc0x2BSceneActivation final : public ZClass {
public:
    using ID = ZClassID<ZNET_COMMAND_CLASS_SCENE_ACTIVATION>;
    using ZClass::ZClass;

    bool setActivation( uint8_t channelId, uint8_t sceneId, uint8_t value );

    ZNET_DEFINE_ACCEPT()
};

}  // namespace znet
