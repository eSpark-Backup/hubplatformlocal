/**
 * @file znet_defs.h
 * @date 11 Apr 2018
 * @brief Defines identifiers
 *
 * Basen on ZW_classcmd from Z-Wave SDK
 */

#ifndef ZNET_DEFS_H
#define ZNET_DEFS_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Basic Device Class identifiers
 */
#define ZNET_BASIC_CLASS_CONTROLLER 0x01        /**< Node is a portable controller */
#define ZNET_BASIC_CLASS_ROUTING_SLAVE 0x04     /**< Node is a slave with routing capabilities */
#define ZNET_BASIC_CLASS_SLAVE 0x03             /**< Node is a slave */
#define ZNET_BASIC_CLASS_STATIC_CONTROLLER 0x02 /**< Node is a static controller */

/**
 * @brief Generic and Specific Device Class identifiers
 */

/**
 * @brief Device class Av Control Point
 */
#define ZNET_GENERIC_CLASS_AV_CONTROL_POINT 0x03 /**< AV Control Point */
#define ZNET_SPECIFIC_CLASS_NOT_USED         \
    0x00 /**< Specific Device Class Not Used \
          */
#define ZNET_SPECIFIC_CLASS_SOUND_SWITCH 0x01
#define ZNET_SPECIFIC_CLASS_DOORBELL 0x12
#define ZNET_SPECIFIC_CLASS_SATELLITE_RECEIVER 0x04    /**< Satellite Receiver */
#define ZNET_SPECIFIC_CLASS_SATELLITE_RECEIVER_V2 0x11 /**< Satellite Receiver V2 */

/**
 * @brief Device class Display
 */
#define ZNET_GENERIC_CLASS_DISPLAY 0x04
#define ZNET_SPECIFIC_CLASS_NOT_USED                                                \
    0x00                                        /**< Specific Device Class Not Used \
                                                 */
#define ZNET_SPECIFIC_CLASS_SIMPLE_DISPLAY 0x01 /**< Display (simple) Device Type */

/**
 * @brief Device class Entry Control
 */
#define ZNET_GENERIC_CLASS_ENTRY_CONTROL 0x40 /**< Entry Control */
#define ZNET_SPECIFIC_CLASS_NOT_USED                                                                  \
    0x00                                                          /**< Specific Device Class Not Used \
                                                                   */
#define ZNET_SPECIFIC_CLASS_DOOR_LOCK 0x01                        /**< Door Lock */
#define ZNET_SPECIFIC_CLASS_ADVANCED_DOOR_LOCK 0x02               /**< Advanced Door Lock */
#define ZNET_SPECIFIC_CLASS_SECURE_KEYPAD_DOOR_LOCK 0x03          /**< Door Lock (keypad –lever) Device Type */
#define ZNET_SPECIFIC_CLASS_SECURE_KEYPAD_DOOR_LOCK_DEADBOLT 0x04 /**< Door Lock (keypad – deadbolt) Device Type */
#define ZNET_SPECIFIC_CLASS_SECURE_DOOR 0x05                      /**< Barrier Operator Specific Device Class */
#define ZNET_SPECIFIC_CLASS_SECURE_GATE 0x06                      /**< Barrier Operator Specific Device Class */
#define ZNET_SPECIFIC_CLASS_SECURE_BARRIER_ADDON 0x07             /**< Barrier Operator Specific Device Class */
#define ZNET_SPECIFIC_CLASS_SECURE_BARRIER_OPEN_ONLY 0x08         /**< Barrier Operator Specific Device Class */
#define ZNET_SPECIFIC_CLASS_SECURE_BARRIER_CLOSE_ONLY 0x09        /**< Barrier Operator Specific Device Class */
#define ZNET_SPECIFIC_CLASS_SECURE_LOCKBOX 0x0A                   /**< SDS12724 */

/**
 * @brief Device class Generic Controller
 */
#define ZNET_GENERIC_CLASS_GENERIC_CONTROLLER 0x01 /**< Remote Controller */
#define ZNET_SPECIFIC_CLASS_NOT_USED                                                            \
    0x00                                                    /**< Specific Device Class Not Used \
                                                             */
#define ZNET_SPECIFIC_CLASS_PORTABLE_REMOTE_CONTROLLER 0x01 /**< Remote Control (Multi Purpose) Device Type */
#define ZNET_SPECIFIC_CLASS_PORTABLE_SCENE_CONTROLLER 0x02  /**< Portable Scene Controller */
#define ZNET_SPECIFIC_CLASS_PORTABLE_INSTALLER_TOOL 0x03
#define ZNET_SPECIFIC_CLASS_REMOTE_CONTROL_AV 0x04     /**< Remote Control (AV) Device Type */
#define ZNET_SPECIFIC_CLASS_REMOTE_CONTROL_SIMPLE 0x06 /**< Remote Control (Simple) Device Type */

/**
 * @brief Device class Meter
 */
#define ZNET_GENERIC_CLASS_METER 0x31 /**< Meter */
#define ZNET_SPECIFIC_CLASS_NOT_USED                                                         \
    0x00                                                 /**< Specific Device Class Not Used \
                                                          */
#define ZNET_SPECIFIC_CLASS_SIMPLE_METER 0x01            /**< Sub Energy Meter Device Type */
#define ZNET_SPECIFIC_CLASS_ADV_ENERGY_CONTROL 0x02      /**< Whole Home Energy Meter (Advanced) Device Type */
#define ZNET_SPECIFIC_CLASS_WHOLE_HOME_METER_SIMPLE 0x03 /**< Whole Home Meter (Simple) Device Type */

/**
 * @brief Device class Meter Pulse
 */
#define ZNET_GENERIC_CLASS_METER_PULSE 0x30 /**< Pulse Meter */
#define ZNET_SPECIFIC_CLASS_NOT_USED         \
    0x00 /**< Specific Device Class Not Used \
          */

/**
 * @brief Device class Non Interoperable
 */
#define ZNET_GENERIC_CLASS_NON_INTEROPERABLE 0xFF /**< Non interoperable */
#define ZNET_SPECIFIC_CLASS_NOT_USED         \
    0x00 /**< Specific Device Class Not Used \
          */

/**
 * @brief Device class Repeater Slave
 */
#define ZNET_GENERIC_CLASS_REPEATER_SLAVE 0x0F /**< Repeater Slave */
#define ZNET_SPECIFIC_CLASS_NOT_USED                                                \
    0x00                                        /**< Specific Device Class Not Used \
                                                 */
#define ZNET_SPECIFIC_CLASS_REPEATER_SLAVE 0x01 /**< Basic Repeater Slave */

/**
 * @brief Device class Security Panel
 */
#define ZNET_GENERIC_CLASS_SECURITY_PANEL 0x17
#define ZNET_SPECIFIC_CLASS_NOT_USED         \
    0x00 /**< Specific Device Class Not Used \
          */
#define ZNET_SPECIFIC_CLASS_ZONED_SECURITY_PANEL 0x01

/**
 * @brief Device class Semi Interoperable
 */
#define ZNET_GENERIC_CLASS_SEMI_INTEROPERABLE 0x50 /**< Semi Interoperable */
#define ZNET_SPECIFIC_CLASS_NOT_USED                                                   \
    0x00                                           /**< Specific Device Class Not Used \
                                                    */
#define ZNET_SPECIFIC_CLASS_ENERGY_PRODUCTION 0x01 /**< Energy Production */

/**
 * @brief Device class Sensor Alarm
 */
#define ZNET_GENERIC_CLASS_SENSOR_ALARM 0xA1
#define ZNET_SPECIFIC_CLASS_NOT_USED         \
    0x00 /**< Specific Device Class Not Used \
          */
#define ZNET_SPECIFIC_CLASS_ADV_ZENSOR_NET_ALARM_SENSOR 0x05
#define ZNET_SPECIFIC_CLASS_ADV_ZENSOR_NET_SMOKE_SENSOR 0x0A
#define ZNET_SPECIFIC_CLASS_BASIC_ROUTING_ALARM_SENSOR 0x01
#define ZNET_SPECIFIC_CLASS_BASIC_ROUTING_SMOKE_SENSOR 0x06
#define ZNET_SPECIFIC_CLASS_BASIC_ZENSOR_NET_ALARM_SENSOR 0x03
#define ZNET_SPECIFIC_CLASS_BASIC_ZENSOR_NET_SMOKE_SENSOR 0x08
#define ZNET_SPECIFIC_CLASS_ROUTING_ALARM_SENSOR 0x02
#define ZNET_SPECIFIC_CLASS_ROUTING_SMOKE_SENSOR 0x07
#define ZNET_SPECIFIC_CLASS_ZENSOR_NET_ALARM_SENSOR 0x04
#define ZNET_SPECIFIC_CLASS_ZENSOR_NET_SMOKE_SENSOR 0x09
#define ZNET_SPECIFIC_CLASS_ALARM_SENSOR \
    0x0B /**< Sensor (Alarm) Device Type \
          */

/**
 * @brief Device class Sensor Binary
 */
#define ZNET_GENERIC_CLASS_SENSOR_BINARY 0x20 /**< Binary Sensor */
#define ZNET_SPECIFIC_CLASS_NOT_USED                                                       \
    0x00                                               /**< Specific Device Class Not Used \
                                                        */
#define ZNET_SPECIFIC_CLASS_ROUTING_SENSOR_BINARY 0x01 /**< Routing Binary Sensor */

/**
 * @brief Device class Sensor Multilevel
 */
#define ZNET_GENERIC_CLASS_SENSOR_MULTILEVEL 0x21 /**< Multilevel Sensor */
#define ZNET_SPECIFIC_CLASS_NOT_USED                                                           \
    0x00                                                   /**< Specific Device Class Not Used \
                                                            */
#define ZNET_SPECIFIC_CLASS_ROUTING_SENSOR_MULTILEVEL 0x01 /**< Sensor (Multilevel) Device Type */
#define ZNET_SPECIFIC_CLASS_CHIMNEY_FAN 0x02

/**
 * @brief Device class Static Controller
 */
#define ZNET_GENERIC_CLASS_STATIC_CONTROLLER 0x02 /**< Static Controller */
#define ZNET_SPECIFIC_CLASS_NOT_USED                                                  \
    0x00                                          /**< Specific Device Class Not Used \
                                                   */
#define ZNET_SPECIFIC_CLASS_PC_CONTROLLER 0x01    /**< Central Controller Device Type */
#define ZNET_SPECIFIC_CLASS_SCENE_CONTROLLER 0x02 /**< Scene Controller */
#define ZNET_SPECIFIC_CLASS_STATIC_INSTALLER_TOOL 0x03
#define ZNET_SPECIFIC_CLASS_SET_TOP_BOX 0x04           /**< Set Top Box Device Type */
#define ZNET_SPECIFIC_CLASS_SUB_SYSTEM_CONTROLLER 0x05 /**< Sub System Controller Device Type */
#define ZNET_SPECIFIC_CLASS_TV 0x06                    /**< TV Device Type */
#define ZNET_SPECIFIC_CLASS_GATEWAY 0x07               /**< Gateway Device Type */

/**
 * @brief Device class Switch Binary
 */
#define ZNET_GENERIC_CLASS_SWITCH_BINARY 0x10 /**< Binary Switch */
#define ZNET_SPECIFIC_CLASS_NOT_USED                                                     \
    0x00                                             /**< Specific Device Class Not Used \
                                                      */
#define ZNET_SPECIFIC_CLASS_POWER_SWITCH_BINARY 0x01 /**< On/Off Power Switch Device Type */
#define ZNET_SPECIFIC_CLASS_SCENE_SWITCH_BINARY                            \
    0x03                                          /**< Binary Scene Switch \
                                                   */
#define ZNET_SPECIFIC_CLASS_POWER_STRIP 0x04      /**< Power Strip Device Type */
#define ZNET_SPECIFIC_CLASS_SIREN 0x05            /**< Siren Device Type */
#define ZNET_SPECIFIC_CLASS_VALVE_OPEN_CLOSE 0x06 /**< Valve (open/close) Device Type */
#define ZNET_SPECIFIC_CLASS_COLOR_TUNABLE_BINARY 0x02

/**
 * @brief Device class Switch Multilevel
 */
#define ZNET_GENERIC_CLASS_SWITCH_MULTILEVEL 0x11 /**< Multilevel Switch */
#define ZNET_SPECIFIC_CLASS_NOT_USED                                                       \
    0x00                                               /**< Specific Device Class Not Used \
                                                        */
#define ZNET_SPECIFIC_CLASS_CLASS_A_MOTOR_CONTROL 0x05 /**< Window Covering No Position/Endpoint Device Type */
#define ZNET_SPECIFIC_CLASS_CLASS_B_MOTOR_CONTROL 0x06 /**< Window Covering Endpoint Aware Device Type */
#define ZNET_SPECIFIC_CLASS_CLASS_C_MOTOR_CONTROL 0x07 /**< Window Covering Position/Endpoint Aware Device Type */
#define ZNET_SPECIFIC_CLASS_MOTOR_MULTIPOSITION                                   \
    0x03                                                 /**< Multiposition Motor \
                                                          */
#define ZNET_SPECIFIC_CLASS_POWER_SWITCH_MULTILEVEL 0x01 /**< Light Dimmer Switch Device Type */
#define ZNET_SPECIFIC_CLASS_SCENE_SWITCH_MULTILEVEL 0x04 /**< Multilevel Scene Switch */
#define ZNET_SPECIFIC_CLASS_FAN_SWITCH 0x08              /**< Fan Switch Device Type */
#define ZNET_SPECIFIC_CLASS_COLOR_TUNABLE_MULTILEVEL 0x02

/**
 * @brief Device class Switch Remote
 */
#define ZNET_GENERIC_CLASS_SWITCH_REMOTE 0x12 /**< Remote Switch */
#define ZNET_SPECIFIC_CLASS_NOT_USED                                                                 \
    0x00                                                         /**< Specific Device Class Not Used \
                                                                  */
#define ZNET_SPECIFIC_CLASS_SWITCH_REMOTE_BINARY 0x01            /**< Binary Remote Switch */
#define ZNET_SPECIFIC_CLASS_SWITCH_REMOTE_MULTILEVEL 0x02        /**< Multilevel Remote Switch */
#define ZNET_SPECIFIC_CLASS_SWITCH_REMOTE_TOGGLE_BINARY 0x03     /**< Binary Toggle Remote Switch */
#define ZNET_SPECIFIC_CLASS_SWITCH_REMOTE_TOGGLE_MULTILEVEL 0x04 /**< Multilevel Toggle Remote Switch */

/**
 * @brief Device class Switch Toggle
 */
#define ZNET_GENERIC_CLASS_SWITCH_TOGGLE 0x13 /**< Toggle Switch */
#define ZNET_SPECIFIC_CLASS_NOT_USED                                                          \
    0x00                                                  /**< Specific Device Class Not Used \
                                                           */
#define ZNET_SPECIFIC_CLASS_SWITCH_TOGGLE_BINARY 0x01     /**< Binary Toggle Switch */
#define ZNET_SPECIFIC_CLASS_SWITCH_TOGGLE_MULTILEVEL 0x02 /**< Multilevel Toggle Switch */

/**
 * @brief Device class Thermostat
 */
#define ZNET_GENERIC_CLASS_THERMOSTAT 0x08 /**< Thermostat */
#define ZNET_SPECIFIC_CLASS_NOT_USED                                                             \
    0x00                                                     /**< Specific Device Class Not Used \
                                                              */
#define ZNET_SPECIFIC_CLASS_SETBACK_SCHEDULE_THERMOSTAT 0x03 /**< Setback Schedule Thermostat */
#define ZNET_SPECIFIC_CLASS_SETBACK_THERMOSTAT 0x05          /**< Thermostat (Setback) Device Type */
#define ZNET_SPECIFIC_CLASS_SETPOINT_THERMOSTAT 0x04
#define ZNET_SPECIFIC_CLASS_THERMOSTAT_GENERAL 0x02    /**< Thermostat General */
#define ZNET_SPECIFIC_CLASS_THERMOSTAT_GENERAL_V2 0x06 /**< Thermostat (HVAC) Device Type */
#define ZNET_SPECIFIC_CLASS_THERMOSTAT_HEATING 0x01    /**< Thermostat Heating */

/**
 * @brief Device class Ventilation
 */
#define ZNET_GENERIC_CLASS_VENTILATION 0x16
#define ZNET_SPECIFIC_CLASS_NOT_USED         \
    0x00 /**< Specific Device Class Not Used \
          */
#define ZNET_SPECIFIC_CLASS_RESIDENTIAL_HRV 0x01

/**
 * @brief Device class Window Covering
 */
#define ZNET_GENERIC_CLASS_WINDOW_COVERING 0x09 /**< Window Covering */
#define ZNET_SPECIFIC_CLASS_NOT_USED                                                        \
    0x00                                                /**< Specific Device Class Not Used \
                                                         */
#define ZNET_SPECIFIC_CLASS_SIMPLE_WINDOW_COVERING 0x01 /**< Simple Window Covering Control */

/**
 * @brief Device class Zip Node
 */
#define ZNET_GENERIC_CLASS_ZIP_NODE 0x15
#define ZNET_SPECIFIC_CLASS_NOT_USED         \
    0x00 /**< Specific Device Class Not Used \
          */
#define ZNET_SPECIFIC_CLASS_ZIP_ADV_NODE 0x02
#define ZNET_SPECIFIC_CLASS_ZIP_TUN_NODE 0x01

/**
 * @brief Device class Wall Controller
 */
#define ZNET_GENERIC_CLASS_WALL_CONTROLLER 0x18
#define ZNET_SPECIFIC_CLASS_NOT_USED                                                       \
    0x00                                               /**< Specific Device Class Not Used \
                                                        */
#define ZNET_SPECIFIC_CLASS_BASIC_WALL_CONTROLLER 0x01 /**< Wall Controller Device Type */

/**
 * @brief Device class Network Extender
 */
#define ZNET_GENERIC_CLASS_NETWORK_EXTENDER 0x05 /**< Network Extender Generic Device Class */
#define ZNET_SPECIFIC_CLASS_NOT_USED                                                 \
    0x00                                         /**< Specific Device Class Not Used \
                                                  */
#define ZNET_SPECIFIC_CLASS_SECURE_EXTENDER 0x01 /**< Specific Device Secure Extender */

/**
 * @brief Device class Appliance
 */
#define ZNET_GENERIC_CLASS_APPLIANCE 0x06
#define ZNET_SPECIFIC_CLASS_NOT_USED         \
    0x00 /**< Specific Device Class Not Used \
          */
#define ZNET_SPECIFIC_CLASS_GENERAL_APPLIANCE 0x01
#define ZNET_SPECIFIC_CLASS_KITCHEN_APPLIANCE 0x02
#define ZNET_SPECIFIC_CLASS_LAUNDRY_APPLIANCE 0x03

/**
 * @brief Device class Sensor Notification
 */
#define ZNET_GENERIC_CLASS_SENSOR_NOTIFICATION 0x07
#define ZNET_SPECIFIC_CLASS_NOT_USED         \
    0x00 /**< Specific Device Class not used \
          */
#define ZNET_SPECIFIC_CLASS_NOTIFICATION_SENSOR 0x01

/**
 * @brief Command class identifiers
 */
// #define ZNET_COMMAND_CLASS_ALARM
// 0x71 // renamed to ZNET_COMMAND_CLASS_NOTIFICATION
#define ZNET_COMMAND_CLASS_NOTIFICATION 0x71
#define ZNET_COMMAND_CLASS_APPLICATION_STATUS 0x22
#define ZNET_COMMAND_CLASS_ASSOCIATION_COMMAND_CONFIGURATION 0x9B
#define ZNET_COMMAND_CLASS_ASSOCIATION 0x85
#define ZNET_COMMAND_CLASS_AV_CONTENT_DIRECTORY_MD 0x95
#define ZNET_COMMAND_CLASS_AV_CONTENT_SEARCH_MD 0x97
#define ZNET_COMMAND_CLASS_AV_RENDERER_STATUS 0x96
#define ZNET_COMMAND_CLASS_AV_TAGGING_MD 0x99
#define ZNET_COMMAND_CLASS_BASIC_TARIFF_INFO 0x36
#define ZNET_COMMAND_CLASS_BASIC_WINDOW_COVERING 0x50
#define ZNET_COMMAND_CLASS_BASIC 0x20
#define ZNET_COMMAND_CLASS_BATTERY 0x80
#define ZNET_COMMAND_CLASS_CHIMNEY_FAN 0x2A
#define ZNET_COMMAND_CLASS_CLIMATE_CONTROL_SCHEDULE 0x46
#define ZNET_COMMAND_CLASS_CLOCK 0x81
#define ZNET_COMMAND_CLASS_CONFIGURATION 0x70
#define ZNET_COMMAND_CLASS_CONTROLLER_REPLICATION 0x21
#define ZNET_COMMAND_CLASS_CRC_16_ENCAP 0x56
#define ZNET_COMMAND_CLASS_DCP_CONFIG 0x3A
#define ZNET_COMMAND_CLASS_DCP_MONITOR 0x3B
#define ZNET_COMMAND_CLASS_DOOR_LOCK_LOGGING 0x4C
#define ZNET_COMMAND_CLASS_DOOR_LOCK 0x62
#define ZNET_COMMAND_CLASS_ENERGY_PRODUCTION 0x90
#define ZNET_COMMAND_CLASS_FIRMWARE_UPDATE_MD 0x7A
#define ZNET_COMMAND_CLASS_GEOGRAPHIC_LOCATION 0x8C
#define ZNET_COMMAND_CLASS_GROUPING_NAME 0x7B
#define ZNET_COMMAND_CLASS_HAIL 0x82
#define ZNET_COMMAND_CLASS_HRV_CONTROL 0x39
#define ZNET_COMMAND_CLASS_HRV_STATUS 0x37
#define ZNET_COMMAND_CLASS_INDICATOR 0x87
#define ZNET_COMMAND_CLASS_IP_CONFIGURATION 0x9A
#define ZNET_COMMAND_CLASS_LANGUAGE 0x89
#define ZNET_COMMAND_CLASS_LOCK 0x76
#define ZNET_COMMAND_CLASS_MANUFACTURER_PROPRIETARY 0x91
#define ZNET_COMMAND_CLASS_MANUFACTURER_SPECIFIC 0x72
#define ZNET_COMMAND_CLASS_MARK 0xEF
#define ZNET_COMMAND_CLASS_METER_PULSE 0x35
#define ZNET_COMMAND_CLASS_METER_TBL_CONFIG 0x3C
#define ZNET_COMMAND_CLASS_METER_TBL_MONITOR 0x3D
#define ZNET_COMMAND_CLASS_METER_TBL_PUSH 0x3E
#define ZNET_COMMAND_CLASS_METER 0x32
#define ZNET_COMMAND_CLASS_MTP_WINDOW_COVERING 0x51
#define ZNET_COMMAND_CLASS_MULTI_CHANNEL_ASSOCIATION 0x8E
#define ZNET_COMMAND_CLASS_MULTI_CHANNEL 0x60
#define ZNET_COMMAND_CLASS_MULTI_CMD 0x8F
#define ZNET_COMMAND_CLASS_MULTI_INSTANCE_ASSOCIATION 0x8E
#define ZNET_COMMAND_CLASS_MULTI_INSTANCE 0x60
#define ZNET_COMMAND_CLASS_NETWORK_MANAGEMENT_PROXY 0x52
#define ZNET_COMMAND_CLASS_NETWORK_MANAGEMENT_BASIC 0x4D
#define ZNET_COMMAND_CLASS_NETWORK_MANAGEMENT_INCLUSION 0x34
#define ZNET_COMMAND_CLASS_NO_OPERATION 0x00
#define ZNET_COMMAND_CLASS_NODE_NAMING 0x77
#define ZNET_COMMAND_CLASS_NON_INTEROPERABLE 0xF0
#define ZNET_COMMAND_CLASS_POWERLEVEL 0x73
#define ZNET_COMMAND_CLASS_PREPAYMENT_ENCAPSULATION 0x41
#define ZNET_COMMAND_CLASS_PREPAYMENT 0x3F
#define ZNET_COMMAND_CLASS_PROPRIETARY 0x88
#define ZNET_COMMAND_CLASS_PROTECTION 0x75
#define ZNET_COMMAND_CLASS_RATE_TBL_CONFIG 0x48
#define ZNET_COMMAND_CLASS_RATE_TBL_MONITOR 0x49
#define ZNET_COMMAND_CLASS_REMOTE_ASSOCIATION_ACTIVATE 0x7C
#define ZNET_COMMAND_CLASS_REMOTE_ASSOCIATION 0x7D
#define ZNET_COMMAND_CLASS_SCENE_ACTIVATION 0x2B
#define ZNET_COMMAND_CLASS_SCENE_ACTUATOR_CONF 0x2C
#define ZNET_COMMAND_CLASS_SCENE_CONTROLLER_CONF 0x2D
#define ZNET_COMMAND_CLASS_SCHEDULE_ENTRY_LOCK 0x4E
#define ZNET_COMMAND_CLASS_SCREEN_ATTRIBUTES 0x93
#define ZNET_COMMAND_CLASS_SCREEN_MD 0x92
#define ZNET_COMMAND_CLASS_SECURITY_PANEL_MODE 0x24
#define ZNET_COMMAND_CLASS_SECURITY_PANEL_ZONE_SENSOR 0x2F
#define ZNET_COMMAND_CLASS_SECURITY_PANEL_ZONE 0x2E
#define ZNET_COMMAND_CLASS_SECURITY 0x98
#define ZNET_COMMAND_CLASS_SENSOR_ALARM 0x9C
#define ZNET_COMMAND_CLASS_SENSOR_BINARY 0x30
#define ZNET_COMMAND_CLASS_SENSOR_CONFIGURATION 0x9E
#define ZNET_COMMAND_CLASS_SENSOR_MULTILEVEL 0x31
#define ZNET_COMMAND_CLASS_SILENCE_ALARM 0x9D
#define ZNET_COMMAND_CLASS_SIMPLE_AV_CONTROL 0x94
#define ZNET_COMMAND_CLASS_SWITCH_ALL 0x27
#define ZNET_COMMAND_CLASS_SWITCH_BINARY 0x25
#define ZNET_COMMAND_CLASS_SWITCH_MULTILEVEL 0x26
#define ZNET_COMMAND_CLASS_SWITCH_TOGGLE_BINARY 0x28
#define ZNET_COMMAND_CLASS_SWITCH_TOGGLE_MULTILEVEL 0x29
#define ZNET_COMMAND_CLASS_TARIFF_CONFIG 0x4A
#define ZNET_COMMAND_CLASS_TARIFF_TBL_MONITOR 0x4B
#define ZNET_COMMAND_CLASS_THERMOSTAT_FAN_MODE 0x44
#define ZNET_COMMAND_CLASS_THERMOSTAT_FAN_STATE 0x45
#define ZNET_COMMAND_CLASS_THERMOSTAT_HEATING 0x38
#define ZNET_COMMAND_CLASS_THERMOSTAT_MODE 0x40
#define ZNET_COMMAND_CLASS_THERMOSTAT_OPERATING_STATE 0x42
#define ZNET_COMMAND_CLASS_THERMOSTAT_SETBACK 0x47
#define ZNET_COMMAND_CLASS_THERMOSTAT_SETPOINT 0x43
#define ZNET_COMMAND_CLASS_TIME_PARAMETERS 0x8B
#define ZNET_COMMAND_CLASS_TIME 0x8A
#define ZNET_COMMAND_CLASS_TRANSPORT_SERVICE 0x55
#define ZNET_COMMAND_CLASS_USER_CODE 0x63
#define ZNET_COMMAND_CLASS_VERSION 0x86
#define ZNET_COMMAND_CLASS_WAKE_UP 0x84
#define ZNET_COMMAND_CLASS_ZIP_6LOWPAN 0x4F
#define ZNET_COMMAND_CLASS_ZIP 0x23
#define ZNET_COMMAND_CLASS_APPLICATION_CAPABILITY 0x57
#define ZNET_COMMAND_CLASS_SWITCH_COLOR 0x33
#define ZNET_COMMAND_CLASS_SCHEDULE 0x53
#define ZNET_COMMAND_CLASS_NETWORK_MANAGEMENT_PRIMARY 0x54
#define ZNET_COMMAND_CLASS_ZIP_ND 0x58
#define ZNET_COMMAND_CLASS_ASSOCIATION_GRP_INFO 0x59
#define ZNET_COMMAND_CLASS_DEVICE_RESET_LOCALLY 0x5A
#define ZNET_COMMAND_CLASS_CENTRAL_SCENE 0x5B
#define ZNET_COMMAND_CLASS_IP_ASSOCIATION 0x5C
#define ZNET_COMMAND_CLASS_ANTITHEFT 0x5D
#define ZNET_COMMAND_CLASS_ZWAVEPLUS_INFO 0x5E
#define ZNET_COMMAND_CLASS_ZIP_GATEWAY 0x5F
#define ZNET_COMMAND_CLASS_ZIP_PORTAL 0x61
#define ZNET_COMMAND_CLASS_APPLIANCE 0x64
#define ZNET_COMMAND_CLASS_DMX 0x65
#define ZNET_COMMAND_CLASS_BARRIER_OPERATOR 0x66
#define ZNET_COMMAND_CLASS_NETWORK_MANAGEMENT_INSTALLATION_MAINTENANCE 0x67
#define ZNET_COMMAND_CLASS_ZIP_NAMING 0x68
#define ZNET_COMMAND_CLASS_MAILBOX 0x69
#define ZNET_COMMAND_CLASS_WINDOW_COVERING 0x6A
#define ZNET_COMMAND_CLASS_SECURITY_2 0x9F
#define ZNET_COMMAND_CLASS_IRRIGATION 0x6B
#define ZNET_COMMAND_CLASS_SUPERVISION 0x6C
#define ZNET_COMMAND_CLASS_HUMIDITY_CONTROL_SETPOINT 0x64
#define ZNET_COMMAND_CLASS_HUMIDITY_CONTROL_MODE 0x6D
#define ZNET_COMMAND_CLASS_HUMIDITY_CONTROL_OPERATING_STATE 0x6E
#define ZNET_COMMAND_CLASS_ENTRY_CONTROL 0x6F
#define ZNET_COMMAND_CLASS_INCLUSION_CONTROLLER 0x74
#define ZNET_COMMAND_CLASS_NODE_PROVISIONING 0x78
#define ZNET_COMMAND_CLASS_SOUND_SWITCH 0x79
#define ZNET_COMMAND_CLASS_ANTITHEFT_UNLOCK 0x7E

#ifdef __cplusplus
}
#endif

#endif  // ZNET_DEFS_H
