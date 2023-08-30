#include "znet/converters/door_lock.hpp"

#include "znet/converters/resources.hpp"

#include "znet/znet_utils.hpp"

#include <cassert>
#include <cstring>

namespace znet {
namespace door_lock_converter {

using namespace resources::cc_door_lock;

using StringList = std::initializer_list<const char* const>;
using RangedModesList = utils::RangedList<DoorLock::Mode, const char*>;

static const StringList Modes_00 = {
    modes::Unsecured,             // 0x00
    modes::Unsecured_WithTimeout  // 0x01
};

static const StringList Modes_10 = {
    modes::Unsecured_InsideHandles,             // 0x10
    modes::Unsecured_InsideHandles_WithTimeout  // 0x11
};

static const StringList Modes_20 = {
    modes::Unsecured_OutsideHandles,             // 0x20
    modes::Unsecured_OutsideHandles_WithTimeout  // 0x21
};

static const StringList Modes_FE = {
    modes::Unknown,  // 0xFE
    modes::Secured   // 0xFF
};

static const RangedModesList ModesList = {
    { DoorLock::Mode::Unsecured, Modes_00 },
    { DoorLock::Mode::Unsecured_InsideHandles, Modes_10 },
    { DoorLock::Mode::Unsecured_OutsideHandles, Modes_20 },
    { DoorLock::Mode::Unknown, Modes_FE },
};

const char* modeToString( DoorLock::Mode mode ) {
    const auto result = utils::findInRangedList( ModesList, mode );
    return result ? *result : nullptr;
}

DoorLock::ModeOpt modeFromString( const char* modeName ) {
    return utils::findIndexInRangedList( ModesList,
                                         [modeName]( const char* value ) { return strcmp( value, modeName ) == 0; } );
}

const char* latchStateToString( DoorLock::LatchState state ) {
    using namespace door_condition::latch_states;

    switch( state ) {
    case DoorLock::LatchState::Open:
        return LatchOpen;

    case DoorLock::LatchState::Closed:
        return LatchClosed;
    }

    assert( false );

    return nullptr;
}

const char* boltStateToString( DoorLock::BoltState state ) {
    using namespace door_condition::bolt_states;

    switch( state ) {
    case DoorLock::BoltState::Unlocked:
        return BoltUnlocked;

    case DoorLock::BoltState::Locked:
        return BoltLocked;
    }

    assert( false );

    return nullptr;
}

const char* doorStateToString( DoorLock::DoorState state ) {
    using namespace door_condition::door_states;

    switch( state ) {
    case DoorLock::DoorState::Open:
        return DoorOpen;

    case DoorLock::DoorState::Closed:
        return DoorClosed;
    }

    assert( false );

    return nullptr;
}

}  // namespace door_lock_converter
}  // namespace znet
