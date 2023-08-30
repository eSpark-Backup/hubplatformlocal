#pragma once

#include <cstdint>

namespace znet {

enum class ValueChangeCommand : std::uint8_t { Up = 0, Down = 1, Stop = 2 };

}  // namespace znet
