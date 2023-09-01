#pragma once

#include <znet/enums/color_component.hpp>
#include <znet/optional.hpp>
#include <chrono>
#include <vector>

namespace znet {

using ColorComponentValue = uint8_t;
using ColorComponentValueOpt = utils::optional<ColorComponentValue>;

using Seconds = std::chrono::seconds;
using SecondsOpt = utils::optional<Seconds>;

struct ColorComponentInfo {
    ColorComponentID id;
    uint8_t value;
    ColorComponentValueOpt targetValue;
    SecondsOpt targetDuration;
};

using ColorComponentInfos = std::vector<ColorComponentInfo>;

}  // namespace znet
