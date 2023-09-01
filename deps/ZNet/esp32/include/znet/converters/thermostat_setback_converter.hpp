#pragma once

#include <znet/enums/thermostat_setback.hpp>

namespace znet {
namespace thermostat_setback {

RawValue toRawValue( thermostat_setback::Mode mode, thermostat_setback::SetbackValueOpt value );

using ModeAndValue = std::pair<Mode, SetbackValueOpt>;

ModeAndValue fromRawValue( RawValue value );

ModeOpt stringToMode( const char* str );
const char* modeToString( Mode mode );

OverrideTypeOpt stringToOverrideType( const char* str );
const char* overrideTypeToString( OverrideType type );

}  // namespace thermostat_setback
}  // namespace znet
