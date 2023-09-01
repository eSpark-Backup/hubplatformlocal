#pragma once

#include <stdint.h>

namespace znet {

enum class GenericType;

namespace specific_types_converter {

const char* typeToString( GenericType genericType, uint8_t specificType ) noexcept;

}  // namespace specific_types_converter
}  // namespace znet
