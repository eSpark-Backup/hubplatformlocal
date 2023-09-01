#pragma once

#include <cstdint>

namespace znet {

enum class ConfigurationFormat : uint8_t;

namespace configuration_format_converter {

const char* formatToString( ConfigurationFormat format ) noexcept;

ConfigurationFormat stringToFormat( const char* name ) noexcept;

}  // namespace configuration_format_converter
}  // namespace znet
