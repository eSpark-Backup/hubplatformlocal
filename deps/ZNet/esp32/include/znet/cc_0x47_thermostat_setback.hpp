#pragma once

#include <znet/znet_command_class.hpp>

#include <znet/enums/thermostat_setback.hpp>

namespace znet {

class ZCc0x47ThermostatSetback final : public ZClass {
public:
    using ID = ZClassID<ZNET_COMMAND_CLASS_THERMOSTAT_SETBACK>;
    using ZClass::ZClass;

    bool setSetback( thermostat_setback::OverrideType type,
                     thermostat_setback::Mode mode,
                     thermostat_setback::SetbackValueOpt value = {} );

    bool requestValue( const RequestValue& request ) override;

    ZNET_DEFINE_ACCEPT()
};

}  // namespace znet
