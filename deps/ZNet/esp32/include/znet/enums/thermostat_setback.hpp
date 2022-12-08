#pragma once

#include <cstdint>

#include <znet/optional.hpp>

namespace znet {
namespace thermostat_setback {

enum class OverrideType : uint8_t {
    NoOverride = 0x00,
    TemporaryOverride = 0x01,
    PermanentOverride = 0x02,
    Unknown = 0x03
};

using OverrideTypeOpt = utils::optional<OverrideType>;

enum class Mode : int8_t { FrostProtection = 0x79, EnergySavingMode = 0x7A, CustomValue = 0x7B, Unknown = 0x7C };

using ModeOpt = utils::optional<Mode>;
using RawValue = int8_t;
using SetbackValue = float;
using SetbackValueOpt = utils::optional<SetbackValue>;

constexpr SetbackValue MinSetbackValue = -12.8f;
constexpr SetbackValue MaxSetbackValue = 12.f;
}  // namespace thermostat_setback
}  // namespace znet
