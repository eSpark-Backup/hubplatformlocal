#pragma once

#include <znet/enums/security_mode.hpp>

#include <cstdint>
#include <vector>

namespace znet {
namespace security_modes_converter {

const char* typeToString( SecurityMode::Class mode ) noexcept;
uint8_t typeToBitset( SecurityMode::Class mode ) noexcept;
uint8_t strToType( const char* str ) noexcept;

uint8_t listToBitset( const std::vector<const char*>& list ) noexcept;
std::vector<const char*> bitsetToList( uint8_t mode );

}  // namespace security_modes_converter
}  // namespace znet
