#pragma once
#include <znet/znet_command_class.hpp>

namespace znet {

class ZCc0x5EZwavePlusInfo final : public ZClass {
public:
    using ID = ZClassID<ZNET_COMMAND_CLASS_ZWAVEPLUS_INFO>;
    using ZClass::ZClass;

    ZNET_DEFINE_ACCEPT()
};

}  // namespace znet
