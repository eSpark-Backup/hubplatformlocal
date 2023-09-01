#pragma once

#include <cstdint>

namespace znet {

enum class ConfigurationFormat : uint8_t {
    SignedInteger = 0x00,
    UnsignedInteger = 0x01,
    Enumerated = 0x02,
    BitField = 0x03,

    Unknown = 0xFF
};

}  // namespace znet
