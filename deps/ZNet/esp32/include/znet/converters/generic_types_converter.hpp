#pragma once

namespace znet {

enum class GenericType;

namespace generic_types_converter {

const char* typeToString( GenericType type ) noexcept;

}  // namespace generic_types_converter
}  // namespace znet
