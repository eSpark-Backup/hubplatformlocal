#include <gtest/gtest.h>

#include <znet/converters/agi_profile_converter.hpp>
#include <znet/converters/agi_commands_converter.hpp>

extern "C" {
#include <znet_lib_cc_codes.h>
}

namespace znet {
namespace ut {

TEST( AgiConverters, ProfileConverter ) {
    using namespace agi_profile_converter;

    ASSERT_EQ( profileToString( 0x01, 0x01 ), "" );

    ASSERT_EQ( profileToString( 0x00, 0x00 ), "General:NA" );
    ASSERT_EQ( profileToString( 0x00, 0x01 ), "General:Lifeline" );
    ASSERT_EQ( profileToString( 0x00, 0x02 ), "General:Unknown" );

    ASSERT_EQ( profileToString( 0x20, 0x00 ), "Control:Key00" );
    ASSERT_EQ( profileToString( 0x20, 0x02 ), "Control:Key02" );
    ASSERT_EQ( profileToString( 0x20, 0x20 ), "Control:Key32" );

    ASSERT_EQ( profileToString( 0x31, 0x00 ), "Sensor:Unknown" );
    ASSERT_EQ( profileToString( 0x31, 0x11 ), "Sensor:Co2 Level" );
    ASSERT_EQ( profileToString( 0x31, 0x23 ), "Sensor:Particulate Matter 2 Dot 5" );

    ASSERT_EQ( profileToString( 0x71, 0x00 ), "Notification:Unknown" );
    ASSERT_EQ( profileToString( 0x71, 0x03 ), "Notification:Co2 Alarm" );
    ASSERT_EQ( profileToString( 0x71, 0x15 ), "Notification:Water Quality Monitoring" );

    ASSERT_EQ( profileToString( 0x32, 0x00 ), "Meter:Unknown" );
    ASSERT_EQ( profileToString( 0x32, 0x01 ), "Meter:Electric" );
    ASSERT_EQ( profileToString( 0x32, 0x05 ), "Meter:Cooling" );

    ASSERT_EQ( profileToString( 0x6b, 0x00 ), "Irrigation:Channel 00" );
    ASSERT_EQ( profileToString( 0x6b, 0x02 ), "Irrigation:Channel 02" );
    ASSERT_EQ( profileToString( 0x6b, 0x20 ), "Irrigation:Channel 32" );
}

TEST( AgiConverters, CommandsConverter ) {
    using namespace agi_commands_converter;

    ASSERT_EQ( commandToString( COMMAND_CLASS_BASIC, 0x00 ), nullptr );
    ASSERT_STREQ( commandToString( COMMAND_CLASS_BASIC, BASIC_SET ), "BASIC_SET" );
    ASSERT_STREQ( commandToString( COMMAND_CLASS_BARRIER_OPERATOR, BARRIER_OPERATOR_SET ), "BARRIER_OPERATOR_SET" );
    ASSERT_STREQ( commandToString( COMMAND_CLASS_SWITCH_BINARY, SWITCH_BINARY_SET ), "SWITCH_BINARY_SET" );
    ASSERT_STREQ( commandToString( COMMAND_CLASS_SWITCH_MULTILEVEL, SWITCH_MULTILEVEL_SET ), "SWITCH_MULTILEVEL_SET" );
    ASSERT_STREQ( commandToString( COMMAND_CLASS_SCENE_ACTIVATION, SCENE_ACTIVATION_SET ), "SCENE_ACTIVATION_SET" );
    ASSERT_STREQ( commandToString( COMMAND_CLASS_SENSOR_BINARY, SENSOR_BINARY_REPORT ), "SENSOR_BINARY_REPORT" );
    ASSERT_STREQ( commandToString( COMMAND_CLASS_SENSOR_MULTILEVEL, SENSOR_MULTILEVEL_REPORT ),
                  "SENSOR_MULTILEVEL_REPORT" );
    ASSERT_STREQ( commandToString( COMMAND_CLASS_METER, METER_REPORT ), "METER_REPORT" );
    ASSERT_STREQ( commandToString( COMMAND_CLASS_SWITCH_COLOR, SWITCH_COLOR_SET ), "SWITCH_COLOR_SET" );
    ASSERT_STREQ( commandToString( COMMAND_CLASS_THERMOSTAT_MODE, THERMOSTAT_MODE_SET ), "THERMOSTAT_MODE_SET" );
    ASSERT_STREQ( commandToString( COMMAND_CLASS_THERMOSTAT_OPERATING_STATE, THERMOSTAT_OPERATING_STATE_REPORT ),
                  "THERMOSTAT_OPERATING_STATE_REPORT" );
    ASSERT_STREQ( commandToString( COMMAND_CLASS_THERMOSTAT_SETPOINT, THERMOSTAT_SETPOINT_SET ),
                  "THERMOSTAT_SETPOINT_SET" );
    ASSERT_STREQ( commandToString( COMMAND_CLASS_THERMOSTAT_FAN_MODE, THERMOSTAT_FAN_MODE_SET ),
                  "THERMOSTAT_FAN_MODE_SET" );
    ASSERT_STREQ( commandToString( COMMAND_CLASS_THERMOSTAT_FAN_STATE, THERMOSTAT_FAN_STATE_REPORT ),
                  "THERMOSTAT_FAN_STATE_REPORT" );
    ASSERT_STREQ( commandToString( COMMAND_CLASS_THERMOSTAT_SETBACK, THERMOSTAT_SETBACK_SET ),
                  "THERMOSTAT_SETBACK_SET" );
    ASSERT_STREQ( commandToString( COMMAND_CLASS_CENTRAL_SCENE, CENTRAL_SCENE_NOTIFICATION ),
                  "CENTRAL_SCENE_NOTIFICATION" );
    ASSERT_STREQ( commandToString( COMMAND_CLASS_DOOR_LOCK, DOOR_LOCK_OPERATION_SET ), "DOOR_LOCK_OPERATION_SET" );
    ASSERT_STREQ( commandToString( COMMAND_CLASS_NOTIFICATION_V3, NOTIFICATION_REPORT_V3 ), "NOTIFICATION_REPORT" );
    ASSERT_STREQ( commandToString( COMMAND_CLASS_SOUND_SWITCH, SOUND_SWITCH_TONE_PLAY_SET ),
                  "SOUND_SWITCH_TONE_PLAY_SET" );
    ASSERT_STREQ( commandToString( COMMAND_CLASS_BATTERY, BATTERY_REPORT ), "BATTERY_REPORT" );
    ASSERT_STREQ( commandToString( COMMAND_CLASS_INDICATOR, INDICATOR_SET ), "INDICATOR_SET" );
    ASSERT_STREQ( commandToString( COMMAND_CLASS_SENSOR_ALARM, SENSOR_ALARM_REPORT ), "SENSOR_ALARM_REPORT" );
}

}  // namespace ut
}  // namespace znet
