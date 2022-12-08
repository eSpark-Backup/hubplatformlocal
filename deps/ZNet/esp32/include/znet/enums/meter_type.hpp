#pragma once

#include <znet/znet_utils.hpp>

#include <utility>
#include <cstdint>

namespace znet {
namespace meter {

enum class MeterType : uint8_t {
    Unknown = 0x00,
    Electric = 0x01,
    Gas = 0x02,
    Water = 0x03,
    Heating = 0x04,
    Cooling = 0x05
};

using MeterTypeOpt = utils::optional<MeterType>;

using Scale = uint8_t;

}  // namespace meter
}  // namespace znet
