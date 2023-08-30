#pragma once

#include <cstdint>
#include <znet/optional.hpp>

namespace znet {

enum class ColorComponentID : uint8_t {
    Wwhite = 0,
    Cwhite = 1,
    Red = 2,
    Green = 3,
    Blue = 4,
    Amber = 5,
    Cyan = 6,
    Purple = 7,
    Indexed = 8,

    ColorComponentsCount
};

using ColorComponentIDOpt = utils::optional<ColorComponentID>;

}  // namespace znet
