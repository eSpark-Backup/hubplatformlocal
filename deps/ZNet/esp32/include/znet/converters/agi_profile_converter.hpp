#pragma once

#include <cstdint>
#include <string>

namespace znet {
namespace agi_profile_converter {

std::string profileToString( uint8_t msb, uint8_t lsb ) noexcept;

}  // namespace agi_profile_converter
}  // namespace znet
