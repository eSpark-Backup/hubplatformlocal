#pragma once

#include <znet/znet_command_class.hpp>

namespace znet {

class ZCc0x25Switch : public ZClass {
public:
    using ID = ZClassID<ZNET_COMMAND_CLASS_SWITCH_BINARY>;

    using ZClass::ZClass;

    bool requestValue( const RequestValue& request ) override;

    ZNET_DEFINE_ACCEPT()

protected:
    bool setValue( const uint8_t value ) override;
    bool setValueMulticast( const uint8_t nodeQuantity,
                            const znet_node_id_t* listNodeId,
                            const uint8_t value,
                            bool sendSingleCasts ) override;
};

}  // namespace znet
