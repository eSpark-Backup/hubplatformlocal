#pragma once

namespace znet {

enum class ThermostatOperatingState;

namespace thermostat_operating_state_converter {

const char* typeToString( ThermostatOperatingState scale ) noexcept;

}  // namespace thermostat_operating_state_converter
}  // namespace znet
