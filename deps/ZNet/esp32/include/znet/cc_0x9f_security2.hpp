#pragma once

#include <znet/znet_command_class.hpp>

namespace znet {

class ZCc0x9FSecurity2 final : public ZClass {
public:
    using ID = ZClassID<ZNET_COMMAND_CLASS_SECURITY_2>;
    using ZClass::ZClass;

    ZNET_DEFINE_ACCEPT();
};

}  // namespace znet
