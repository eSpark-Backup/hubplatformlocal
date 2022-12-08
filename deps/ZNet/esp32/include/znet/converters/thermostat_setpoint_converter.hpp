#pragma once

#include <stdint.h>
#include <utility>

namespace znet {
namespace thermostat_setpoint_converter {

const char* scaleToString( uint8_t scale ) noexcept;

using ScaleResult = std::pair<uint8_t, bool>;
ScaleResult scaleFromString( const char* scale ) noexcept;

}  // namespace thermostat_setpoint_converter
}  // namespace znet
