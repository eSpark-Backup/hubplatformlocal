#pragma once

#include <znet/znet_command_class.hpp>

namespace znet {

class ZCc0x98Security final : public ZClass {
public:
    using ID = ZClassID<ZNET_COMMAND_CLASS_SECURITY>;
    using ZClass::ZClass;

    ZNET_DEFINE_ACCEPT();
};

}  // namespace znet
