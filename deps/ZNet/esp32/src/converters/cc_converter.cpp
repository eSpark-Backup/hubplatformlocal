#include <znet/converters/cc_converter.hpp>

#include <znet/converters/resources.hpp>
#include <znet/znet_defs.h>

#include <cassert>
#include <cstring>

namespace znet {
namespace cc_converter {

struct ZWaveClassData {
    uint8_t id;
    const char* const name;
};

static const ZWaveClassData zwaveClassesData[] = {
    { ZNET_COMMAND_CLASS_SENSOR_MULTILEVEL, resources::cc_sensor_multilevel::Name },
    { ZNET_COMMAND_CLASS_SENSOR_BINARY, resources::cc_sensor_binary::Name },
    { ZNET_COMMAND_CLASS_METER, resources::cc_meter::Name },
    { ZNET_COMMAND_CLASS_SWITCH_COLOR, resources::cc_switch_color::Name },
    { ZNET_COMMAND_CLASS_THERMOSTAT_MODE, resources::cc_thermostat_mode::Name },
    { ZNET_COMMAND_CLASS_THERMOSTAT_OPERATING_STATE, resources::cc_thermostat_operating_state::Name },
    { ZNET_COMMAND_CLASS_THERMOSTAT_SETPOINT, resources::cc_thermostat_setpoint::Name },
    { ZNET_COMMAND_CLASS_THERMOSTAT_FAN_MODE, resources::cc_thermostat_fan_mode::Name },
    { ZNET_COMMAND_CLASS_THERMOSTAT_FAN_STATE, resources::cc_thermostat_fan_state::Name },
    { ZNET_COMMAND_CLASS_DOOR_LOCK, resources::cc_door_lock::Name },
    { ZNET_COMMAND_CLASS_SWITCH_MULTILEVEL, resources::cc_switch_multilevel::Name },
    { ZNET_COMMAND_CLASS_USER_CODE, resources::cc_user_code::Name }
};

const char* idToString( uint8_t id ) {
    for( const auto& data : zwaveClassesData ) {
        if( data.id == id ) {
            return data.name;
        }
    }

    assert( false );

    return nullptr;
}

uint8_t idFromString( const char* name ) {
    for( const auto& data : zwaveClassesData ) {
        if( strcmp( data.name, name ) == 0 ) {
            return data.id;
        }
    }

    assert( false );

    return 0;
}

}  // namespace cc_converter
}  // namespace znet
