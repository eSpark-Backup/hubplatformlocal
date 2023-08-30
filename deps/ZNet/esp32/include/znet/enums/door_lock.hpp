#pragma once

#include <znet/optional.hpp>

#include <cstdint>
#include <utility>

namespace znet {
namespace DoorLock {

enum class Mode : uint8_t {
    Unsecured = 0x00,
    Unsecured_WithTimeout = 0x01,
    Unsecured_InsideHandles = 0x10,
    Unsecured_InsideHandles_WithTimeout = 0x11,
    Unsecured_OutsideHandles = 0x20,
    Unsecured_OutsideHandles_WithTimeout = 0x21,
    Unknown = 0xFE,
    Secured = 0xFF
};

using ModeOpt = utils::optional<Mode>;

enum class LatchState : uint8_t { Open, Closed };

enum class BoltState : uint8_t { Locked, Unlocked };

enum class DoorState : uint8_t { Open, Closed };

}  // namespace DoorLock
}  // namespace znet
