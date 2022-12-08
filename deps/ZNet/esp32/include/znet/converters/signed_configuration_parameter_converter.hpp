#pragma once

#include <stdint.h>

namespace znet {

enum class ConfigurationFormat : uint8_t;

namespace signed_configuration_parameter_converter {

int64_t toUiValue( uint32_t value, uint8_t size, ConfigurationFormat format ) noexcept;

uint32_t toZNetValue( int64_t value, uint8_t size, ConfigurationFormat format ) noexcept;

}  // namespace signed_configuration_parameter_converter
}  // namespace znet
