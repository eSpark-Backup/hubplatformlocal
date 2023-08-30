#pragma once

#include <znet/znet_command_class.hpp>

namespace znet {

class ZCc0x6FEntryControl final : public ZClass {
public:
    using ID = ZClassID<ZNET_COMMAND_CLASS_ENTRY_CONTROL>;
    using ZClass::ZClass;

    ZNET_DEFINE_ACCEPT()

protected:
    bool setValue( const uint8_t value ) override;
};

}  // namespace znet
