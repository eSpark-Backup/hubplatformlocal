#include "znet/converters/thermostat_setback_converter.hpp"

#include "znet/znet_utils.hpp"

#include <algorithm>
#include <cmath>

namespace znet {
namespace thermostat_setback {

namespace {
using StringList = std::initializer_list<const char* const>;
using RangedModesList = utils::RangedList<Mode, const char*>;
using RangedOverrideTypeList = utils::RangedList<OverrideType, const char*>;

const StringList Modes_0x79 = {
    "frost_protection",    // 0x79
    "energy_saving_mode",  // 0x7A
    "custom_value",        // 0x7B
    "unknown"              // 0x7C
};
const RangedModesList ModesList = { { Mode::FrostProtection, Modes_0x79 } };

const StringList OverrideTypes_0x00 = {
    "no_override",         // 0x00
    "temporary_override",  // 0x01
    "permanent_override",  // 0x02
    "unknown"              // 0x03
};
const RangedOverrideTypeList OverrideTypeList = { { OverrideType::NoOverride, OverrideTypes_0x00 } };

constexpr float SetbackValueFactor = 10.f;

}  // namespace

RawValue toRawValue( thermostat_setback::Mode mode, thermostat_setback::SetbackValueOpt value ) {
    switch( mode ) {
    case Mode::FrostProtection:
    case Mode::EnergySavingMode:
        return static_cast<RawValue>( mode );
    case Mode::CustomValue:
        return value ? static_cast<RawValue>( std::floor(
                           std::min( MaxSetbackValue, std::max( MinSetbackValue, *value ) ) * SetbackValueFactor +
                           0.5f ) ) :
                       0;
    case Mode::Unknown:
        return value ? static_cast<RawValue>( *value ) : 0;
    }

    assert( false );
    return value ? static_cast<RawValue>( *value ) : 0;
}

ModeAndValue fromRawValue( RawValue value ) {
    constexpr RawValue MaxRawValue = static_cast<RawValue>( MaxSetbackValue * SetbackValueFactor );
    constexpr RawValue MinRawValue = static_cast<RawValue>( MinSetbackValue * SetbackValueFactor );
    if( value >= MinRawValue && value <= MaxRawValue ) {
        return { Mode::CustomValue, { value / SetbackValueFactor } };
    }

    const auto mode = static_cast<Mode>( value );
    if( mode == Mode::FrostProtection || mode == Mode::EnergySavingMode ) {
        return { mode, {} };
    }

    return { Mode::Unknown, { static_cast<SetbackValue>( value ) } };
}

ModeOpt stringToMode( const char* str ) {
    return utils::findIndexInRangedList( ModesList,
                                         [str]( const char* modeStr ) { return strcmp( str, modeStr ) == 0; } );
}

const char* modeToString( Mode mode ) {
    const auto result = utils::findInRangedList( ModesList, mode );
    return result ? *result : nullptr;
}

OverrideTypeOpt stringToOverrideType( const char* str ) {
    return utils::findIndexInRangedList( OverrideTypeList,
                                         [str]( const char* typeStr ) { return strcmp( str, typeStr ) == 0; } );
}

const char* overrideTypeToString( OverrideType type ) {
    const auto result = utils::findInRangedList( OverrideTypeList, type );
    return result ? *result : nullptr;
}

}  // namespace thermostat_setback
}  // namespace znet
