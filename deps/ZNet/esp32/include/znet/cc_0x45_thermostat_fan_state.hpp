#pragma once

#include <znet/znet_command_class.hpp>

namespace znet {

class ZCc0x45ThermostatFanState final : public ZClass {
public:
    using ID = ZClassID<ZNET_COMMAND_CLASS_THERMOSTAT_FAN_STATE>;
    using ZClass::ZClass;

    bool requestValue( const RequestValue& request ) override;

    ZNET_DEFINE_ACCEPT()
};

}  // namespace znet
