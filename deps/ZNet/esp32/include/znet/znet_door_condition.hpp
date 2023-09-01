#pragma once

#include <znet/enums/door_lock.hpp>

namespace znet {
using namespace DoorLock;
struct DoorCondition {
    LatchState latchState;
    BoltState boltState;
    DoorState doorState;
};
}  // namespace znet
