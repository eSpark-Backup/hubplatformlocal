#pragma once

#include <cinttypes>

namespace znet {

enum class SensorsSoundDisablingMode : uint8_t {
    All = 0x00,
    AllAlarmed = 0x01,
    UsingBitmask = 0x02,
    AlarmedUsingBitmask = 0x03
};

}  // namespace znet
