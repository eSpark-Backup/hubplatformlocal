#pragma once

#include <znet/enums/door_lock.hpp>

#include <stdint.h>
#include <utility>

namespace znet {
namespace door_lock_converter {

const char* modeToString( DoorLock::Mode mode );
DoorLock::ModeOpt modeFromString( const char* modeName );

const char* latchStateToString( DoorLock::LatchState state );
const char* boltStateToString( DoorLock::BoltState state );
const char* doorStateToString( DoorLock::DoorState state );

}  // namespace door_lock_converter
}  // namespace znet
