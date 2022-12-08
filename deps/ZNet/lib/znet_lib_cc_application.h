
#ifndef __ZNET_LIB_CC_APPLICATION_H__
#define __ZNET_LIB_CC_APPLICATION_H__

#include "znet.h"

#include "ZFunctionClass.h"

#include "znet_lib_cc_codes.h"

//////////////////////////////////////////////////////////////////////////////
// Command Class Basic //
// Send Basic Get
//
// @param: node_id
// Destination Node Id
//
// @param: Callback
// @default: NULL
// Custom function to be called on function failure
// NULL if callback is not needed
//
// @param: callbackArg
// Custom argument to be passed to custom function to be called on function
// success or failure
//
znet_error_t ICACHE_FLASH_ATTR znet_cc_basic_get( uint8_t node_id,
                                                  uint8_t channel_id,
                                                  ZFunction_Callback Callback,
                                                  void* callbackArg,
                                                  encap_type_t encap_type );

// Send Basic Set
//
// @param: node_id
// Destination Node Id
//
// @param: value
// Value
//
// @param: Callback
// @default: NULL
// Custom function to be called on function failure
// NULL if callback is not needed
//
// @param: callbackArg
// Custom argument to be passed to custom function to be called on function
// success or failure
//
znet_error_t ICACHE_FLASH_ATTR znet_cc_basic_set( uint8_t node_id,
                                                  uint8_t channel_id,
                                                  uint8_t value,
                                                  ZFunction_Callback Callback,
                                                  void* callbackArg,
                                                  encap_type_t encap_type );

// Basic Report CB
//
void ICACHE_FLASH_ATTR znet_cc_basic_report( const ZFunction func,
                                             uint8_t node_id,
                                             int cc_data_len,
                                             const uint8_t* cc_data );

// Basic Set CB
//
void ICACHE_FLASH_ATTR znet_cc_basic_receive_set( const ZFunction func,
                                                  uint8_t node_id,
                                                  int cc_data_len,
                                                  const uint8_t* cc_data );

// Basic Receive
//
void ICACHE_FLASH_ATTR znet_cc_basic_receive( const ZFunction func,
                                              uint8_t node_id,
                                              int cc_data_len,
                                              const uint8_t* cc_data );

//////////////////////////////////////////////////////////////////////////////
// Command Class NoOperation //

// Send NoOperation empty packet
//
// @param: node_id
// Destination Node Id
//
// @param: instance_id
// Instance Id
//
// @param: Callback
// @default: NULL
// Custom function to be called on function failure
// NULL if callback is not needed
//
// @param: callbackArg
// Custom argument to be passed to custom function to be called on function
// success or failure
//
// This function is not exported into C and JS. Please use
// z_way_device_sned_nop() instead
// ///znet_error_t ICACHE_FLASH_ATTR znet_cc_nop_send( uint8_t node_id,
// uint8_t instance_id, ZFunction_Callback
// Callback, void* callbackArg, encap_type_t encap_type );

//////////////////////////////////////////////////////////////////////////////
// Command Class Configuration //

// Send Configuration Get
//
// @param: znet
// ZNet object instance
//
// @param: node_id
// Destination Node Id
//
// @param: parameter
// Parameter number (from 1 to 255)
//
// @param: Callback
// @default: NULL
// Custom function to be called on function failure
// NULL if callback is not needed
//
// @param: callbackArg
// Custom argument to be passed to custom function to be called on function
// success or failure
//
znet_error_t ICACHE_FLASH_ATTR znet_cc_configuration_get( uint8_t node_id,
                                                          uint8_t channel_id,
                                                          uint8_t parameter,
                                                          ZFunction_Callback Callback,
                                                          void* callbackArg,
                                                          encap_type_t encap_type );

// Send Configuration Set
//
// @param: node_id
// Destination Node Id
//
// @param: parameter
// Parameter number (from 1 to 255)
//
// @param: value
// Value to be sent (negative and positive values are accepted, but will be
// stripped to size)
//
// @param: size
// @default: 0
// Size of the value (1, 2 or 4 bytes). Use 0 to guess from previously reported
// value if any 0 means use size previously obtained Get
//
// @param: Callback
// @default: NULL
// Custom function to be called on function failure
// NULL if callback is not needed
//
// @param: callbackArg
// Custom argument to be passed to custom function to be called on function
// success or failure
//
znet_error_t ICACHE_FLASH_ATTR znet_cc_configuration_set( uint8_t node_id,
                                                          uint8_t channel_id,
                                                          uint8_t parameter,
                                                          bool set_default,
                                                          uint32_t value,
                                                          uint8_t size,
                                                          ZFunction_Callback Callback,
                                                          void* callbackArg,
                                                          encap_type_t encap_type );

//----------------------------------------------------------------------------
// Configuration Report Command
//
void ICACHE_FLASH_ATTR znet_cc_configuration_report( const ZFunction func,
                                                     uint8_t node_id,
                                                     int cc_data_len,
                                                     const uint8_t* cc_data );

znet_error_t ICACHE_FLASH_ATTR znet_cc_configuration_bulk_get( uint8_t node_id,
                                                               uint8_t channel_id,
                                                               uint16_t parameter_offset,
                                                               uint8_t parameter_count,
                                                               ZFunction_Callback Callback,
                                                               void* callbackArg,
                                                               encap_type_t encap_type );

znet_error_t ICACHE_FLASH_ATTR znet_cc_configuration_bulk_set( uint8_t node_id,
                                                               uint8_t channel_id,
                                                               uint16_t parameter_offset,
                                                               uint8_t parameter_count,
                                                               bool set_default,
                                                               bool handshake,
                                                               uint8_t size,
                                                               const uint8_t* parametr,
                                                               ZFunction_Callback Callback,
                                                               void* callbackArg,
                                                               encap_type_t encap_type );

//----------------------------------------------------------------------------
// Configuration Bulk Report Command
//
void ICACHE_FLASH_ATTR znet_cc_configuration_bulk_report( const ZFunction func,
                                                          uint8_t node_id,
                                                          int cc_data_len,
                                                          const uint8_t* cc_data );

znet_error_t ICACHE_FLASH_ATTR znet_cc_configuration_name_get( uint8_t node_id,
                                                               uint8_t channel_id,
                                                               uint16_t parameter_number,
                                                               ZFunction_Callback Callback,
                                                               void* callbackArg,
                                                               encap_type_t encap_type );

//----------------------------------------------------------------------------
// Configuration Name Report Command
//
void ICACHE_FLASH_ATTR znet_cc_configuration_name_report( const ZFunction func,
                                                          uint8_t node_id,
                                                          int cc_data_len,
                                                          const uint8_t* cc_data );

znet_error_t ICACHE_FLASH_ATTR znet_cc_configuration_info_get( uint8_t node_id,
                                                               uint8_t channel_id,
                                                               uint16_t parameter_number,
                                                               ZFunction_Callback Callback,
                                                               void* callbackArg,
                                                               encap_type_t encap_type );

//----------------------------------------------------------------------------
// Configuration Info Report Command
//
void ICACHE_FLASH_ATTR znet_cc_configuration_info_report( const ZFunction func,
                                                          uint8_t node_id,
                                                          int cc_data_len,
                                                          const uint8_t* cc_data );

znet_error_t ICACHE_FLASH_ATTR znet_cc_configuration_properties_get( uint8_t node_id,
                                                                     uint8_t channel_id,
                                                                     uint16_t parameter_number,
                                                                     ZFunction_Callback Callback,
                                                                     void* callbackArg,
                                                                     encap_type_t encap_type );

//----------------------------------------------------------------------------
// Configuration Properties Report Command
//
void ICACHE_FLASH_ATTR znet_cc_configuration_properties_report( const ZFunction func,
                                                                uint8_t node_id,
                                                                int cc_data_len,
                                                                const uint8_t* cc_data );

znet_error_t ICACHE_FLASH_ATTR znet_cc_configuration_default_reset( uint8_t node_id,
                                                                    uint8_t channel_id,
                                                                    ZFunction_Callback Callback,
                                                                    void* callbackArg,
                                                                    encap_type_t encap_type );

/**
 * Retrieves parameter number from a serialized command considering its CC version.
 * @param command_data An array of bytes filled with properly formatted Configuration CC command data.
 * @param size The length of @param command_data
 * @return A valid parameter number found in @param command_data or UINT16_MAX in case of a failure.
 */
uint16_t znet_cc_confguration_get_param_number( const uint8_t* command_data, uint8_t size );

//----------------------------------------------------------------------------
// Command Class SensorBinary //

// Send SensorBinary Get
//
// @param: node_id
// Destination Node Id
//
// @param: channel_id
// Destination channel Id
//
// @param: sensorType_available
//
// @param: sensorType
// Type of sensor to query information for
// @default: -1
// 0xFF to query information for the first available sensor type
// -1 to query information for all supported sensor types
//
// @param: Callback
// @default: NULL
// Custom function to be called on function failure
// NULL if callback is not needed
//
// @param: callbackArg
// Custom argument to be passed to custom function to be called on function
// success or failure
//
// @param: encap_type
// Defines which Transport Service use to send commands
//
// @param: version
// CC version
//
znet_error_t ICACHE_FLASH_ATTR znet_cc_sensor_binary_get( uint8_t node_id,
                                                          uint8_t channel_id,
                                                          uint8_t sensorType,
                                                          ZFunction_Callback Callback,
                                                          void* callbackArg,
                                                          encap_type_t encap_type,
                                                          uint8_t version );

//----------------------------------------------------------------------------
// Sensor Binary Report Command
//
void ICACHE_FLASH_ATTR znet_cc_sensor_binary_report( const ZFunction func,
                                                     uint8_t node_id,
                                                     int cc_data_len,
                                                     const uint8_t* cc_data );

// Send SensorBinary Get
//
// @param: node_id
// Destination Node Id
//
// @param: channel_id
// Destination channel Id
//
// @param: Callback
// @default: NULL
// Custom function to be called on function failure
// NULL if callback is not needed
//
// @param: callbackArg
// Custom argument to be passed to custom function to be called on function
// success or failure
//
// @param: encap_type
// Defines which Transport Service use to send commands
//
// @param: version
// CC version
//
znet_error_t ICACHE_FLASH_ATTR znet_cc_sensor_binary_supported_get( uint8_t node_id,
                                                                    uint8_t channel_id,
                                                                    ZFunction_Callback Callback,
                                                                    void* callbackArg,
                                                                    encap_type_t encap_type,
                                                                    uint8_t version );

//----------------------------------------------------------------------------
// Sensor Binary Supported Sensor Report Command
//
void ICACHE_FLASH_ATTR znet_cc_sensor_binary_supported_report( const ZFunction func,
                                                               uint8_t node_id,
                                                               int cc_data_len,
                                                               const uint8_t* cc_data );

//////////////////////////////////////////////////////////////////////////////
// Command Class Meter //

// Send Meter Get
//
// @param: node_id
// Destination Node Id
//
// @param: scale
// @default: -1
// Desired scale
// -1 for all scales
//
// @param: Callback
// @default: NULL
// Custom function to be called on function failure
// NULL if callback is not needed
//
// @param: callbackArg
// Custom argument to be passed to custom function to be called on function
// success or failure
//
znet_error_t ICACHE_FLASH_ATTR znet_cc_meter_get( uint8_t node_id,
                                                  uint8_t channel_id,
                                                  uint8_t scale,
                                                  uint8_t scale2,
                                                  uint8_t rate_type,
                                                  ZFunction_Callback Callback,
                                                  void* callbackArg,
                                                  encap_type_t encap_type,
                                                  uint8_t version );

// Send Meter Reset
//
// @param: node_id
// Destination Node Id
//
// @param: Callback
// @default: NULL
// Custom function to be called on function failure
// NULL if callback is not needed
//
// @param: callbackArg
// Custom argument to be passed to custom function to be called on function
// success or failure
//
znet_error_t ICACHE_FLASH_ATTR znet_cc_meter_reset( uint8_t node_id,
                                                    uint8_t channel_id,
                                                    ZFunction_Callback Callback,
                                                    void* callbackArg,
                                                    encap_type_t encap_type );

znet_error_t ICACHE_FLASH_ATTR znet_cc_meter_reset_v6( uint8_t node_id,
                                                       uint8_t channel_id,
                                                       uint8_t type,
                                                       uint8_t data_size,
                                                       const uint8_t* data,
                                                       ZFunction_Callback Callback,
                                                       void* callbackArg,
                                                       encap_type_t encap_type );

//----------------------------------------------------------------------------
// Meter Report Command
//
void ICACHE_FLASH_ATTR znet_cc_meter_report( const ZFunction func,
                                             uint8_t node_id,
                                             int cc_data_len,
                                             const uint8_t* cc_data );

// Send Meter SupportedGet
//
// @param: node_id
// Destination Node Id
//
// @param: Callback
// @default: NULL
// Custom function to be called on function failure
// NULL if callback is not needed
//
// @param: callbackArg
// Custom argument to be passed to custom function to be called on function
// success or failure
//
znet_error_t ICACHE_FLASH_ATTR znet_cc_meter_supported_get( uint8_t node_id,
                                                            uint8_t channel_id,
                                                            ZFunction_Callback Callback,
                                                            void* callbackArg,
                                                            encap_type_t encap_type );

//----------------------------------------------------------------------------
// Meter Supported Report Command
//
void ICACHE_FLASH_ATTR znet_cc_meter_supported_report( const ZFunction func,
                                                       uint8_t node_id,
                                                       int cc_data_len,
                                                       const uint8_t* cc_data );

//////////////////////////////////////////////////////////////////////////////

znet_error_t ICACHE_FLASH_ATTR znet_cc_multilevel_sensor_get_supported_sensor( uint8_t node_id,
                                                                               uint8_t channel_id,
                                                                               ZFunction_Callback Callback,
                                                                               void* callbackArg,
                                                                               encap_type_t encap_type );

znet_error_t ICACHE_FLASH_ATTR znet_cc_multilevel_sensor_get( uint8_t node_id,
                                                              uint8_t channel_id,
                                                              uint8_t sensor_type,
                                                              uint8_t scale,
                                                              ZFunction_Callback Callback,
                                                              void* callbackArg,
                                                              encap_type_t encap_type );

//////////////////////////////////////////////////////////////////////////////

// Command Class SensorMultilevel //

// Send SensorMultilevel Get
//
// @param: node_id
// Destination Node Id
//
// @param: sensor_type
// @default: -1
// Type of sensor to be requested.
// -1 means all sensor types supported by the device
//
// @param: Callback
// @default: NULL
// Custom function to be called on function failure
// NULL if callback is not needed
//
// @param: callbackArg
// Custom argument to be passed to custom function to be called on function
// success or failure
//
znet_error_t ICACHE_FLASH_ATTR znet_cc_sensor_multilevel_get( uint8_t node_id,
                                                              uint8_t channel_id,
                                                              uint8_t sensor_type,
                                                              uint8_t scale,
                                                              ZFunction_Callback Callback,
                                                              void* callbackArg,
                                                              encap_type_t encap_type,
                                                              uint8_t version );

// SwitchMultilevel Report CB
//
void ICACHE_FLASH_ATTR znet_cc_sensor_multilevel_report( const ZFunction func,
                                                         uint8_t node_id,
                                                         int cc_data_len,
                                                         const uint8_t* cc_data );

znet_error_t ICACHE_FLASH_ATTR znet_cc_sensor_multilevel_supported_sensor_get( uint8_t node_id,
                                                                               uint8_t channel_id,
                                                                               ZFunction_Callback Callback,
                                                                               void* callbackArg,
                                                                               encap_type_t encap_type );

// SwitchMultilevel Supported Sensor Report CB
//
void ICACHE_FLASH_ATTR znet_cc_sensor_multilevel_supported_sensor_report( const ZFunction func,
                                                                          uint8_t node_id,
                                                                          int cc_data_len,
                                                                          const uint8_t* cc_data );

znet_error_t ICACHE_FLASH_ATTR znet_cc_sensor_multilevel_supported_scale_get( uint8_t node_id,
                                                                              uint8_t channel_id,
                                                                              uint8_t sensor_type,
                                                                              ZFunction_Callback Callback,
                                                                              void* callbackArg,
                                                                              encap_type_t encap_type );

// SwitchMultilevel Supported Scale Report CB
//
void ICACHE_FLASH_ATTR znet_cc_sensor_multilevel_supported_scale_report( const ZFunction func,
                                                                         uint8_t node_id,
                                                                         int cc_data_len,
                                                                         const uint8_t* cc_data );

//////////////////////////////////////////////////////////////////////////////
// Command Class SensorConfiguration //

// Send SensorConfiguration Get
//
// @param: node_id
// Destination Node Id
//
// @param: instance_id
// Instance Id
//
// @param: Callback
// @default: NULL
// Custom function to be called on function failure
// NULL if callback is not needed
//
// @param: callbackArg
// Custom argument to be passed to custom function to be called on function
// success or failure
//
/// znet_error_t ICACHE_FLASH_ATTR znet_cc_sensor_configuration_get( uint8_t
/// node_id, uint8_t instance_id,
/// ZFunction_Callback Callback, void* callbackArg, encap_type_t encap_type );

// Send SensorConfiguration Set
//
// @param: znet
// ZNet object instance
//
// @param: node_id
// Destination Node Id
//
// @param: instance_id
// Instance Id
//
// @param: mode
// Value set mode
//
// @param: value
// Value
//
// @param: Callback
// @default: NULL
// Custom function to be called on function failure
// NULL if callback is not needed
//
// @param: callbackArg
// Custom argument to be passed to custom function to be called on function
// success or failure
//
/// znet_error_t ICACHE_FLASH_ATTR znet_cc_sensor_configuration_set( uint8_t
/// node_id, uint8_t instance_id, uint8_t mode, float value,
/// ZFunction_Callback Callback, void* callbackArg, encap_type_t
/// encap_type );

//////////////////////////////////////////////////////////////////////////////

// Command Class SwitchAll //

// Send SwitchAll Get
//
// @param: node_id
// Destination Node Id
//
// @param: Callback
// @default: NULL
// Custom function to be called on function failure
// NULL if callback is not needed
//
// @param: callbackArg
// Custom argument to be passed to custom function to be called on function
// success or failure
//
znet_error_t ICACHE_FLASH_ATTR znet_cc_switch_all_get( uint8_t node_id,
                                                       ZFunction_Callback Callback,
                                                       void* callbackArg,
                                                       encap_type_t encap_type );

// Send SwitchAll Set
//
// @param: node_id
// Destination Node Id
//
// @param: mode
// SwitchAll Mode: see definitions below
//
// @param: Callback
// @default: NULL
// Custom function to be called on function failure
// NULL if callback is not needed
//
// @param: callbackArg
// Custom argument to be passed to custom function to be called on function
// success or failure
//
znet_error_t ICACHE_FLASH_ATTR znet_cc_switch_all_set( uint8_t node_id,
                                                       uint8_t mode,
                                                       ZFunction_Callback Callback,
                                                       void* callbackArg,
                                                       encap_type_t encap_type );

// Send SwitchAll Set On
//
// @param: node_id
// Destination Node Id
//
// @param: Callback
// @default: NULL
// Custom function to be called on function failure
// NULL if callback is not needed
//
// @param: callbackArg
// Custom argument to be passed to custom function to be called on function
// success or failure
//
znet_error_t ICACHE_FLASH_ATTR znet_cc_switch_all_set_on( uint8_t node_id,
                                                          ZFunction_Callback Callback,
                                                          void* callbackArg,
                                                          encap_type_t encap_type );

// Send SwitchAll Set Off
//
// @param: node_id
// Destination Node Id
//
// @param: Callback
// @default: NULL
// Custom function to be called on function failure
// NULL if callback is not needed
//
// @param: callbackArg
// Custom argument to be passed to custom function to be called on function
// success or failure
//
znet_error_t ICACHE_FLASH_ATTR znet_cc_switch_all_set_off( uint8_t node_id,
                                                           ZFunction_Callback Callback,
                                                           void* callbackArg,
                                                           encap_type_t encap_type );

//----------------------------------------------------------------------------
// Switch All Report Command
void ICACHE_FLASH_ATTR znet_cc_switch_all_report( const ZFunction func,
                                                  uint8_t node_id,
                                                  int cc_data_len,
                                                  const uint8_t* cc_data );

//////////////////////////////////////////////////////////////////////////////

// Command Class SwitchBinary //

// Send SwitchBinary Get
//
// @param: node_id
// Destination Node Id
//
// @param: Callback
// @default: NULL
// Custom function to be called on function failure
// NULL if callback is not needed
//
// @param: callbackArg
// Custom argument to be passed to custom function to be called on function
// success or failure
//
znet_error_t ICACHE_FLASH_ATTR znet_cc_switch_binary_get( uint8_t node_id,
                                                          uint8_t channel_id,
                                                          ZFunction_Callback Callback,
                                                          void* callbackArg,
                                                          encap_type_t encap_type );

// Send SwitchBinary Set
//
// @param: node_id
// Destination Node Id
//
// @param: value
// Value
//
// @param: default_duration_available
//
// @param: default_duration
//
// @param: Callback
// @default: NULL
// Custom function to be called on function failure
// NULL if callback is not needed
//
// @param: callbackArg
// Custom argument to be passed to custom function to be called on function
// success or failure
//
znet_error_t ICACHE_FLASH_ATTR znet_cc_switch_binary_set( uint8_t node_id,
                                                          uint8_t channel_id,
                                                          bool value,
                                                          uint8_t duration,
                                                          ZFunction_Callback Callback,
                                                          void* callbackArg,
                                                          encap_type_t encap_type );

// SwitchBinary Report CB
//
void ICACHE_FLASH_ATTR znet_cc_switch_binary_report( const ZFunction func,
                                                     uint8_t node_id,
                                                     int cc_data_len,
                                                     const uint8_t* cc_data );

//////////////////////////////////////////////////////////////////////////////

// Command Class SwitchMultilevel //

// Send SwitchMultilevel Get
//
// @param: node_id
// Destination Node Id
//
// @param: Callback
// @default: NULL
// Custom function to be called on function failure
// NULL if callback is not needed
//
// @param: callbackArg
// Custom argument to be passed to custom function to be called on function
// success or failure
//
znet_error_t ICACHE_FLASH_ATTR znet_cc_switch_multilevel_get( uint8_t node_id,
                                                              uint8_t channel_id,
                                                              ZFunction_Callback Callback,
                                                              void* callbackArg,
                                                              encap_type_t encap_type );

// Send SwitchMultilevel Set
//
// @param: node_id
// Destination Node Id
//
// @param: level
// Level to be set
//
// @param: duration
// @default: 0xff
// Duration of change:
//  0 instantly
//  0x01-0x7f in seconds
//  0x80-0xfe in minutes mapped to 1-127 (value 0x80=128 is 1 minute)
//  0xff use device factory default
//
// @param: Callback
// @default: NULL
// Custom function to be called on function failure
// NULL if callback is not needed
//
// @param: callbackArg
// Custom argument to be passed to custom function to be called on function
// success or failure
//
znet_error_t ICACHE_FLASH_ATTR znet_cc_switch_multilevel_set( uint8_t node_id,
                                                              uint8_t channel_id,
                                                              uint8_t level,
                                                              bool duration_available,
                                                              uint8_t duration,
                                                              ZFunction_Callback Callback,
                                                              void* callbackArg,
                                                              encap_type_t encap_type );

// SwitchMultilevel Report CB
//
void ICACHE_FLASH_ATTR znet_cc_switch_multilevel_report( const ZFunction func,
                                                         uint8_t node_id,
                                                         int cc_data_len,
                                                         const uint8_t* cc_data );

// Send SwitchMultilevel StartLevelChange
//
// @param: node_id
// Destination Node Id
//
// @param: dir
// Direction of change: 0 to incrase, 1 to decrase
//
// @param: ignoreStartLevel
// @default: TRUE
// If set to True, device will ignore start level value and will use it's curent
// value
//
// @param: startLevel
// @default: 50
// Start level to change from
//
// @param: duration_available
//
// @param: duration
// @default: 0xff
// Duration of change:
//  0 instantly
//  0x01-0x7f in seconds
//  0x80-0xfe in minutes mapped to 1-127 (value 0x80=128 is 1 minute)
//  0xff use device factory default
//
// @param: step_available
//
// @param: step
// @default: 0xff
// Step to be used in level change in percentage
// 0-99 mapped to 1-100%
// 0xff uses device factory default
//
// @param: incdec
// @default: 0
// Increment/decrement type for step:
//  0 Increment
//  1 Decrement
//  2 Reserved
//  3 No Inc/Dec
//
// @param: Callback
// @default: NULL
// Custom function to be called on function failure
// NULL if callback is not needed
//
// @param: callbackArg
// Custom argument to be passed to custom function to be called on function
// success or failure
//
znet_error_t ICACHE_FLASH_ATTR znet_cc_switch_multilevel_start_level_change( uint8_t node_id,
                                                                             uint8_t channel_id,
                                                                             uint8_t dir,
                                                                             bool ignore_start_level,
                                                                             uint8_t start_level,
                                                                             bool duration_available,
                                                                             uint8_t duration,
                                                                             bool secondary_switch_step_available,
                                                                             uint8_t secondary_switch_step,
                                                                             uint8_t secondary_switch_dir,
                                                                             ZFunction_Callback Callback,
                                                                             void* callbackArg,
                                                                             encap_type_t encap_type );

// Send SwitchMultilevel StopLevelChange
//
// @param: node_id
// Destination Node Id
//
// @param: Callback
// @default: NULL
// Custom function to be called on function failure
// NULL if callback is not needed
//
// @param: callbackArg
// Custom argument to be passed to custom function to be called on function
// success or failure
//
znet_error_t ICACHE_FLASH_ATTR znet_cc_switch_multilevel_stop_level_change( uint8_t node_id,
                                                                            uint8_t channel_id,
                                                                            ZFunction_Callback Callback,
                                                                            void* callbackArg,
                                                                            encap_type_t encap_type );

znet_error_t ICACHE_FLASH_ATTR znet_cc_switch_multilevel_supported_get( uint8_t node_id,
                                                                        uint8_t channel_id,
                                                                        ZFunction_Callback Callback,
                                                                        void* callbackArg,
                                                                        encap_type_t encap_type );

// SwitchMultilevel Supported Report CB
//
void ICACHE_FLASH_ATTR znet_cc_switch_multilevel_supported_report( const ZFunction func,
                                                                   uint8_t node_id,
                                                                   int cc_data_len,
                                                                   const uint8_t* cc_data );

//////////////////////////////////////////////////////////////////////////////

// Command Class Sound Switch // // Todo

// Tone number

znet_error_t ICACHE_FLASH_ATTR znet_cc_sound_switch_tones_number_get( uint8_t node_id,
                                                                      uint8_t channel_id,
                                                                      ZFunction_Callback Callback,
                                                                      void* callbackArg,
                                                                      encap_type_t encap_type );

void ICACHE_FLASH_ATTR znet_cc_sound_switch_tones_number_report( const ZFunction func,
                                                                 uint8_t node_id,
                                                                 int cc_data_len,
                                                                 const uint8_t* cc_data );

// Tone info

znet_error_t ICACHE_FLASH_ATTR znet_cc_sound_switch_tone_info_get( uint8_t node_id,
                                                                   uint8_t channel_id,
                                                                   uint8_t tone_id,
                                                                   ZFunction_Callback Callback,
                                                                   void* callbackArg,
                                                                   encap_type_t encap_type );

void ICACHE_FLASH_ATTR znet_cc_sound_switch_tone_info_report( const ZFunction func,
                                                              uint8_t node_id,
                                                              int cc_data_len,
                                                              const uint8_t* cc_data );

// Configuration

znet_error_t ICACHE_FLASH_ATTR znet_cc_sound_switch_configuration_set( uint8_t node_id,
                                                                       uint8_t channel_id,
                                                                       uint8_t volume,
                                                                       uint8_t default_tone_id,
                                                                       ZFunction_Callback Callback,
                                                                       void* callbackArg,
                                                                       encap_type_t encap_type );

znet_error_t ICACHE_FLASH_ATTR znet_cc_sound_switch_configuration_get( uint8_t node_id,
                                                                       uint8_t channel_id,
                                                                       ZFunction_Callback Callback,
                                                                       void* callbackArg,
                                                                       encap_type_t encap_type );

void ICACHE_FLASH_ATTR znet_cc_sound_switch_configuration_report( const ZFunction func,
                                                                  uint8_t node_id,
                                                                  int cc_data_len,
                                                                  const uint8_t* cc_data );

// Tone play

znet_error_t ICACHE_FLASH_ATTR znet_cc_sound_switch_tone_play_set( uint8_t node_id,
                                                                   uint8_t channel_id,
                                                                   uint8_t tone_id,
                                                                   uint8_t volume,
                                                                   ZFunction_Callback Callback,
                                                                   void* callbackArg,
                                                                   encap_type_t encap_type );

znet_error_t ICACHE_FLASH_ATTR znet_cc_sound_switch_tone_play_get( uint8_t node_id,
                                                                   uint8_t channel_id,
                                                                   ZFunction_Callback Callback,
                                                                   void* callbackArg,
                                                                   encap_type_t encap_type );

void ICACHE_FLASH_ATTR znet_cc_sound_switch_tone_play_report( const ZFunction func,
                                                              uint8_t node_id,
                                                              int cc_data_len,
                                                              const uint8_t* cc_data );

//////////////////////////////////////////////////////////////////////////////
// Command Class Scene Activation //

// Send SceneActivation Set
//
// @param: node_id
// Destination Node Id
//
// @param: sceneId
// Scene Id
//
// @param: dimmingDuration
// @default: 0xff
// Dimming duration
//
// @param: Callback
// @default: NULL
// Custom function to be called on function failure
// NULL if callback is not needed
//
// @param: callbackArg
// Custom argument to be passed to custom function to be called on function
// success or failure
//
znet_error_t ICACHE_FLASH_ATTR znet_cc_scene_activation_set( uint8_t node_id,
                                                             uint8_t channel_id,
                                                             uint8_t sceneId,
                                                             uint8_t dimmingDuration,
                                                             ZFunction_Callback Callback,
                                                             void* callbackArg,
                                                             encap_type_t encap_type );

// Scene Activation Set Receive Command
//
void ICACHE_FLASH_ATTR znet_cc_scene_activation_receive_set( const ZFunction func,
                                                             uint8_t node_id,
                                                             int cc_data_len,
                                                             const uint8_t* cc_data );

//----------------------------------------------------------------------------
// Command Class Scene Actuator Conf //

// Send SceneActuatorConf Get
//
// @param: node_id
// Destination Node Id
//
// @param: scene
// @default: 0
// Scene Id
// 0 means get current scene
//
// @param: Callback
// @default: NULL
// Custom function to be called on function failure
// NULL if callback is not needed
//
// @param: callbackArg
// Custom argument to be passed to custom function to be called on function
// success or failure
//
znet_error_t ICACHE_FLASH_ATTR znet_cc_scene_actuator_conf_get( uint8_t node_id,
                                                                uint8_t channel_id,
                                                                uint8_t scene,
                                                                ZFunction_Callback Callback,
                                                                void* callbackArg,
                                                                encap_type_t encap_type );

// Send SceneActuatorConf Set
//
// @param: node_id
// Destination Node Id
//
// @param: scene
// Scene Id
//
// @param: dimming
// @default: 0xff
// Dimming
//
// @param: override
// @default: TRUE
// If false then the current settings in the device is associated with the Scene
// Id. If true then the Level value is used
//
// @param: level
// Level
//
// @param: Callback
// @default: NULL
// Custom function to be called on function failure
// NULL if callback is not needed
//
// @param: callbackArg
// Custom argument to be passed to custom function to be called on function
// success or failure
//
znet_error_t ICACHE_FLASH_ATTR znet_cc_scene_actuator_conf_set( uint8_t node_id,
                                                                uint8_t channel_id,
                                                                uint8_t scene,
                                                                uint8_t dimming,
                                                                bool override,
                                                                uint8_t level,
                                                                ZFunction_Callback Callback,
                                                                void* callbackArg,
                                                                encap_type_t encap_type );

// Scene Actuator Conf Report Command
//
void ICACHE_FLASH_ATTR znet_cc_scene_actuator_conf_report( const ZFunction func,
                                                           uint8_t node_id,
                                                           int cc_data_len,
                                                           const uint8_t* cc_data );

//////////////////////////////////////////////////////////////////////////////

// Command Class Protection //

// Send Protection Get
//
// @param: node_id
// Destination Node Id
//
// @param: Callback
// @default: NULL
// Custom function to be called on function failure
// NULL if callback is not needed
//
// @param: callbackArg
// Custom argument to be passed to custom function to be called on function
// success or failure
//
znet_error_t ICACHE_FLASH_ATTR znet_cc_protection_get( uint8_t node_id,
                                                       ZFunction_Callback Callback,
                                                       void* callbackArg,
                                                       encap_type_t encap_type );

// Send Protection Set
//
// @param: node_id
// Destination Node Id
//
// @param: state
// Local control protection state
//
// @param: rfState
// @default: 0
// RF control protection state
//
// @param: Callback
// @default: NULL
// Custom function to be called on function failure
// NULL if callback is not needed
//
// @param: callbackArg
// Custom argument to be passed to custom function to be called on function
// success or failure
//
znet_error_t ICACHE_FLASH_ATTR znet_cc_protection_set( uint8_t node_id,
                                                       uint8_t state_available,
                                                       uint8_t state,
                                                       uint8_t localState,
                                                       uint8_t rfState,
                                                       ZFunction_Callback Callback,
                                                       void* callbackArg,
                                                       encap_type_t encap_type );

// Protection supported Report CB
//
void ICACHE_FLASH_ATTR znet_cc_protection_report( const ZFunction func,
                                                  uint8_t node_id,
                                                  int cc_data_len,
                                                  const uint8_t* cc_data );

// Send Protection Exclusive Control Get
//
// @param: node_id
// Destination Node Id
//
// @param: Callback
// @default: NULL
// Custom function to be called on function failure
// NULL if callback is not needed
//
// @param: callbackArg
// Custom argument to be passed to custom function to be called on function
// success or failure
//
znet_error_t ICACHE_FLASH_ATTR znet_cc_protection_ec_get( uint8_t node_id,
                                                          ZFunction_Callback Callback,
                                                          void* callbackArg,
                                                          encap_type_t encap_type );

// Send Protection Exclusive Control Set
//
// @param: node_id
// Destination Node Id
//
// @param: controlNodeId
// Node Id to have exclusive control over destination node
//
// @param: Callback
// @default: NULL
// Custom function to be called on function failure
// NULL if callback is not needed
//
// @param: callbackArg
// Custom argument to be passed to custom function to be called on function
// success or failure
//
znet_error_t ICACHE_FLASH_ATTR znet_cc_protection_ec_set( uint8_t node_id,
                                                          uint8_t controlNodeId,
                                                          ZFunction_Callback Callback,
                                                          void* callbackArg,
                                                          encap_type_t encap_type );

// Protection Timeout Report CB
//
void ICACHE_FLASH_ATTR znet_cc_protection_ec_report( const ZFunction func,
                                                     uint8_t node_id,
                                                     int cc_data_len,
                                                     const uint8_t* cc_data );

// Send Protection Supported Get
//
// @param: node_id
// Destination Node Id
//
// @param: Callback
// @default: NULL
// Custom function to be called on function failure
// NULL if callback is not needed
//
// @param: callbackArg
// Custom argument to be passed to custom function to be called on function
// success or failure
//
znet_error_t ICACHE_FLASH_ATTR znet_cc_protection_supported_get( uint8_t node_id,
                                                                 ZFunction_Callback Callback,
                                                                 void* callbackArg,
                                                                 encap_type_t encap_type );

// Protection supported Report CB
//
void ICACHE_FLASH_ATTR znet_cc_protection_supported_report( const ZFunction func,
                                                            uint8_t node_id,
                                                            int cc_data_len,
                                                            const uint8_t* cc_data );

// Send Protection Timeout Get
//
// @param: node_id
// Destination Node Id
//
// @param: Callback
// @default: NULL
// Custom function to be called on function failure
// NULL if callback is not needed
//
// @param: callbackArg
// Custom argument to be passed to custom function to be called on function
// success or failure
//
znet_error_t ICACHE_FLASH_ATTR znet_cc_protection_timeout_get( uint8_t node_id,
                                                               ZFunction_Callback Callback,
                                                               void* callbackArg,
                                                               encap_type_t encap_type );

// Send Protection Timeout Set
//
// @param: node_id
// Destination Node Id
//
// @param: timeout
// Timeout in seconds
// 0 is no timer set
// -1 is infinite timeout
// max value is 191 minute (11460 seconds)
// values above 1 minute are rounded to 1-minute boundary
//
// @param: Callback
// @default: NULL
// Custom function to be called on function failure
// NULL if callback is not needed
//
// @param: callbackArg
// Custom argument to be passed to custom function to be called on function
// success or failure
//
znet_error_t ICACHE_FLASH_ATTR znet_cc_protection_timeout_set( uint8_t node_id,
                                                               uint8_t timeout,
                                                               ZFunction_Callback Callback,
                                                               void* callbackArg,
                                                               encap_type_t encap_type );

//----------------------------------------------------------------------------
// Protection Timeout Report CB
//
void ICACHE_FLASH_ATTR znet_cc_protection_timeout_report( const ZFunction func,
                                                          uint8_t node_id,
                                                          int cc_data_len,
                                                          const uint8_t* cc_data );

//////////////////////////////////////////////////////////////////////////////

// Command Class Alarm //

// Send Alarm Get
//
// @param: node_id
// Destination Node Id
//
// @param: type
// @default: 0
// Type of alarm to get level for
// 0 to get level for all supported alarms (v2 and higher)
// 0xFF to get level for first supported alarm (v2 and higher)
//
// @param: event_available
//
// @param: event
// @default: 0
// Notification event to get level for
// This argument is ignored prior to Notification v3
// Must be 0 if type is 0xFF
//
// @param: Callback
// @default: NULL
// Custom function to be called on function failure
// NULL if callback is not needed
//
// @param: callbackArg
// Custom argument to be passed to custom function to be called on function
// success or failure
//
znet_error_t ICACHE_FLASH_ATTR znet_cc_alarm_get( uint8_t node_id,
                                                  uint8_t type,
                                                  bool event_available,
                                                  uint8_t event,
                                                  ZFunction_Callback Callback,
                                                  void* callbackArg,
                                                  encap_type_t encap_type );

// Send Alarm Set (v2 and higher)
//
// @param: node_id
// Destination Node Id
//
// @param: type
// Type of alarm to set level for
//
// @param: level
// Level to set (0x0 = off, 0xFF = on, other values are reserved)
//
// @param: Callback
// @default: NULL
// Custom function to be called on function failure
// NULL if callback is not needed
//
// @param: callbackArg
// Custom argument to be passed to custom function to be called on function
// success or failure
//
znet_error_t ICACHE_FLASH_ATTR znet_cc_alarm_set( uint8_t node_id,
                                                  uint8_t type,
                                                  uint8_t level,
                                                  ZFunction_Callback Callback,
                                                  void* callbackArg,
                                                  encap_type_t encap_type );

// Alarm Report CB
//
void ICACHE_FLASH_ATTR znet_cc_alarm_report( const ZFunction func,
                                             uint8_t node_id,
                                             int cc_data_len,
                                             const uint8_t* cc_data );

// Send Alarm Type Supported Get
//
// @param: node_id
// Destination Node Id
//
// @param: Callback
// @default: NULL
// Custom function to be called on function failure
// NULL if callback is not needed
//
// @param: callbackArg
// Custom argument to be passed to custom function to be called on function
// success or failure
//
znet_error_t ICACHE_FLASH_ATTR znet_cc_alarm_type_supported_get( uint8_t node_id,
                                                                 ZFunction_Callback Callback,
                                                                 void* callbackArg,
                                                                 encap_type_t encap_type );

// Alarm Type Supported Report CB
//
void ICACHE_FLASH_ATTR znet_cc_alarm_type_supported_report( const ZFunction func,
                                                            uint8_t node_id,
                                                            int cc_data_len,
                                                            const uint8_t* cc_data );

//////////////////////////////////////////////////////////////////////////////
// Command Class Firmware Update //

// Send Firmware Metadata Get
//
// @param: node_id
// Destination Node Id
//
// @param: instance_id
// Instance Id
//
// @param: Callback
// @default: NULL
// Custom function to be called on function failure
// NULL if callback is not needed
//
// @param: callbackArg
// Custom argument to be passed to custom function to be called on function
// success or failure
//
znet_error_t ICACHE_FLASH_ATTR znet_cc_firmware_update_get( uint8_t node_id,
                                                            uint8_t instance_id,
                                                            ZFunction_Callback Callback,
                                                            void* callbackArg,
                                                            encap_type_t encap_type );

// Send Firmware Update Request Get
//
// @param: node_id
// Destination Node Id
//
// @param: instance_id
// Instance Id
//
// @param: manufacturerId
// Manufacturer Id (2 bytes)
//
// @param: firmwareId
// Firmware Id (2 bytes)
//
// @param: firmwareTarget
// Firmware target number (0 for main chip, 1..255 for additional chips)
// Used only for CC v3 and above
//
// @param: length
// Size of firmware image
//
// @param: data
// Firmware image data
//
// @param: Callback
// @default: NULL
// Custom function to be called on function failure
// NULL if callback is not needed
//
// @param: callbackArg
// Custom argument to be passed to custom function to be called on function
// success or failure
//
znet_error_t ICACHE_FLASH_ATTR znet_cc_firmware_update_perform( uint8_t node_id,
                                                                uint8_t instance_id,
                                                                int manufacturerId,
                                                                int firmwareId,
                                                                uint8_t firmwareTarget,
                                                                size_t length,
                                                                const uint8_t* data,
                                                                ZFunction_Callback Callback,
                                                                void* callbackArg,
                                                                encap_type_t encap_type );

//////////////////////////////////////////////////////////////////////////////
// Command Notification //

//----------------------------------------------------------------------------
znet_error_t ICACHE_FLASH_ATTR znet_cc_notification_get( uint8_t node_id,
                                                         uint8_t channel_id,
                                                         uint8_t v1_alarm_type,
                                                         uint8_t type,
                                                         uint8_t event,
                                                         ZFunction_Callback Callback,
                                                         void* callbackArg,
                                                         encap_type_t encap_type,
                                                         uint8_t version );

znet_error_t ICACHE_FLASH_ATTR znet_cc_notification_set( uint8_t node_id,
                                                         uint8_t channel_id,
                                                         uint8_t type,
                                                         uint8_t status,
                                                         ZFunction_Callback Callback,
                                                         void* callbackArg,
                                                         encap_type_t encap_type );

//----------------------------------------------------------------------------
// Notification Report Command
//
void ICACHE_FLASH_ATTR znet_cc_notification_report( const ZFunction func,
                                                    uint8_t node_id,
                                                    int cc_data_len,
                                                    const uint8_t* cc_data );

znet_error_t ICACHE_FLASH_ATTR znet_cc_notification_supported_get( uint8_t node_id,
                                                                   uint8_t channel_id,
                                                                   ZFunction_Callback Callback,
                                                                   void* callbackArg,
                                                                   encap_type_t encap_type );

//----------------------------------------------------------------------------
// Notification Supported Report Command
//
void ICACHE_FLASH_ATTR znet_cc_notification_supported_report( const ZFunction func,
                                                              uint8_t node_id,
                                                              int cc_data_len,
                                                              const uint8_t* cc_data );

znet_error_t ICACHE_FLASH_ATTR znet_cc_notification_event_supported_get( uint8_t node_id,
                                                                         uint8_t channel_id,
                                                                         uint8_t type,
                                                                         ZFunction_Callback Callback,
                                                                         void* callbackArg,
                                                                         encap_type_t encap_type );

//----------------------------------------------------------------------------
// Notification Event Supported Report Command
//
void ICACHE_FLASH_ATTR znet_cc_notification_event_supported_report( const ZFunction func,
                                                                    uint8_t node_id,
                                                                    int cc_data_len,
                                                                    const uint8_t* cc_data );

//////////////////////////////////////////////////////////////////////////////
// Command Class Window Covering //

//----------------------------------------------------------------------------
// Send Window Covering Supported Get
//
// @param: node_id
// Destination Node Id
//
// @param: Callback
// @default: NULL
// Custom function to be called on function failure
// NULL if callback is not needed
//
// @param: callbackArg
// Custom argument to be passed to custom function to be called on function
// success or failure
//
znet_error_t ICACHE_FLASH_ATTR znet_cc_window_covering_supported_get( uint8_t node_id,
                                                                      ZFunction_Callback Callback,
                                                                      void* callbackArg,
                                                                      encap_type_t encap_type );

//----------------------------------------------------------------------------
// Send Window Covering Get
//
// @param: node_id
// Destination Node Id
//
// @param: parameter_id
// Parametr ID
//
// @param: Callback
// @default: NULL
// Custom function to be called on function failure
// NULL if callback is not needed
//
// @param: callbackArg
// Custom argument to be passed to custom function to be called on function
// success or failure
//
znet_error_t ICACHE_FLASH_ATTR znet_cc_window_covering_get( uint8_t node_id,
                                                            uint8_t parameter_id,
                                                            ZFunction_Callback Callback,
                                                            void* callbackArg,
                                                            encap_type_t encap_type );

//----------------------------------------------------------------------------
// Send Window Covering Set
//
// @param: node_id
// Destination Node Id
//
// @param: parameter_count
// Parameter Count
//
// @param: parameters
// Parameter ID + value
//
// @param: duration
// Duration
//
// @param: Callback
// @default: NULL
// Custom function to be called on function failure
// NULL if callback is not needed
//
// @param: callbackArg
// Custom argument to be passed to custom function to be called on function
// success or failure
//
znet_error_t ICACHE_FLASH_ATTR znet_cc_window_covering_set( uint8_t node_id,
                                                            uint8_t parameter_count,
                                                            const uint8_t* parameters,
                                                            uint8_t duration,
                                                            ZFunction_Callback Callback,
                                                            void* callbackArg,
                                                            encap_type_t encap_type );

//----------------------------------------------------------------------------
// Send Window Covering Start Level Change
//
// @param: node_id
// Destination Node Id
//
// @param: up_down
// UpDown (1 bits)
//
// @param: parameter_id
// Parametr ID
//
// @param: duration
// Duration
//
// @param: Callback
// @default: NULL
// Custom function to be called on function failure
// NULL if callback is not needed
//
// @param: callbackArg
// Custom argument to be passed to custom function to be called on function
// success or failure
//
znet_error_t ICACHE_FLASH_ATTR znet_cc_window_covering_start_level_change( uint8_t node_id,
                                                                           bool up_down,
                                                                           uint8_t parameter_id,
                                                                           uint8_t duration,
                                                                           ZFunction_Callback Callback,
                                                                           void* callbackArg,
                                                                           encap_type_t encap_type );

//----------------------------------------------------------------------------
// Send Window Covering Stop Level Change
//
// @param: node_id
// Destination Node Id
//
// @param: parameter_id
// Parametr ID
//
// @param: Callback
// @default: NULL
// Custom function to be called on function failure
// NULL if callback is not needed
//
// @param: callbackArg
// Custom argument to be passed to custom function to be called on function
// success or failure
//
znet_error_t ICACHE_FLASH_ATTR znet_cc_window_covering_stop_level_change( uint8_t node_id,
                                                                          uint8_t parameter_id,
                                                                          ZFunction_Callback Callback,
                                                                          void* callbackArg,
                                                                          encap_type_t encap_type );

//////////////////////////////////////////////////////////////////////////////
// Command Class Switch Toggle Multilevel //

//----------------------------------------------------------------------------
// Switch Toggle Multilevel Set Command
//
// @param: node_id
// Destination Node Id
//
// @param: Callback
// @default: NULL
// Custom function to be called on function failure
// NULL if callback is not needed
//
// @param: callbackArg
// Custom argument to be passed to custom function to be called on function
// success or failure
//
znet_error_t ICACHE_FLASH_ATTR znet_cc_switch_toggle_multilevel_set( uint8_t node_id,
                                                                     ZFunction_Callback Callback,
                                                                     void* callbackArg,
                                                                     encap_type_t encap_type );

//----------------------------------------------------------------------------
// Switch Toggle Multilevel Get Command
//
// @param: node_id
// Destination Node Id
//
// @param: Callback
// @default: NULL
// Custom function to be called on function failure
// NULL if callback is not needed
//
// @param: callbackArg
// Custom argument to be passed to custom function to be called on function
// success or failure
//
znet_error_t ICACHE_FLASH_ATTR znet_cc_switch_toggle_multilevel_get( uint8_t node_id,
                                                                     ZFunction_Callback Callback,
                                                                     void* callbackArg,
                                                                     encap_type_t encap_type );

//----------------------------------------------------------------------------
// Switch Toggle Multilevel Start Level Change Command
//
// @param: node_id
// Destination Node Id
//
// @param: ignore_start_level
// Ignore Start Level (1 bits)
//
// @param: roll_over
// Roll Over (1 bits)
//
// @param: start_level
// Start Level
//
// @param: Callback
// @default: NULL
// Custom function to be called on function failure
// NULL if callback is not needed
//
// @param: callbackArg
// Custom argument to be passed to custom function to be called on function
// success or failure
//
znet_error_t ICACHE_FLASH_ATTR znet_cc_switch_toggle_multilevel_start_level_change( uint8_t node_id,
                                                                                    bool ignore_start_level,
                                                                                    bool roll_over,
                                                                                    uint8_t start_level,
                                                                                    ZFunction_Callback Callback,
                                                                                    void* callbackArg,
                                                                                    encap_type_t encap_type );

//----------------------------------------------------------------------------
// Switch Toggle Multilevel Stop Level Change Command
//
// @param: node_id
// Destination Node Id
//
// @param: Callback
// @default: NULL
// Custom function to be called on function failure
// NULL if callback is not needed
//
// @param: callbackArg
// Custom argument to be passed to custom function to be called on function
// success or failure
//
znet_error_t ICACHE_FLASH_ATTR znet_cc_switch_toggle_multilevel_stop_level_change( uint8_t node_id,
                                                                                   ZFunction_Callback Callback,
                                                                                   void* callbackArg,
                                                                                   encap_type_t encap_type );

//////////////////////////////////////////////////////////////////////////////
// Command Class COLOR SWITCH //

//----------------------------------------------------------------------------
// Color Switch Supported Get Command
//
// @param: node_id
// Destination Node Id
//
// @param: channel_id
// Destination channel Id
//
// @param: Callback
// @default: NULL
// Custom function to be called on function failure
// NULL if callback is not needed
//
// @param: callbackArg
// Custom argument to be passed to custom function to be called on function
// success or failure
//
znet_error_t ICACHE_FLASH_ATTR znet_cc_color_switch_supported_get( uint8_t node_id,
                                                                   uint8_t channel_id,
                                                                   ZFunction_Callback Callback,
                                                                   void* callbackArg,
                                                                   encap_type_t encap_type );

//----------------------------------------------------------------------------
// Color Switch Report SupportedCommand
//
void ICACHE_FLASH_ATTR znet_cc_color_switch_supported_report( const ZFunction func,
                                                              uint8_t node_id,
                                                              int cc_data_len,
                                                              const uint8_t* cc_data );

//----------------------------------------------------------------------------
// Color Switch Get Command
//
// @param: node_id
// Destination Node Id
//
// @param: channel_id
// Destination channel Id
//
// @param: cl_component_id
// Color Component ID
//
// @param: Callback
// @default: NULL
// Custom function to be called on function failure
// NULL if callback is not needed
//
// @param: callbackArg
// Custom argument to be passed to custom function to be called on function
// success or failure
//
znet_error_t ICACHE_FLASH_ATTR znet_cc_color_switch_get( uint8_t node_id,
                                                         uint8_t channel_id,
                                                         uint8_t cl_component_id,
                                                         ZFunction_Callback Callback,
                                                         void* callbackArg,
                                                         encap_type_t encap_type );

//----------------------------------------------------------------------------
// Color Switch Report Command
//
void ICACHE_FLASH_ATTR znet_cc_color_switch_report( const ZFunction func,
                                                    uint8_t node_id,
                                                    int cc_data_len,
                                                    const uint8_t* cc_data );

//----------------------------------------------------------------------------
// Color Switch Set Command
//
// @param: node_id
// Destination Node Id
//
// @param: channel_id
// Destination channel Id
//
// @param: cl_component_count
// Destination Color Component Count
//
// @param: components
// Destination Color Component ID + Value
//
// @param: duration
// Destination Duration
//
// @param: Callback
// @default: NULL
// Custom function to be called on function failure
// NULL if callback is not needed
//
// @param: callbackArg
// Custom argument to be passed to custom function to be called on function success or failure
//
// @param: version
// CC version
//
znet_error_t ICACHE_FLASH_ATTR znet_cc_color_switch_set( uint8_t node_id,
                                                         uint8_t channel_id,
                                                         uint8_t cl_component_count,
                                                         const uint8_t* components,
                                                         uint8_t duration,
                                                         ZFunction_Callback Callback,
                                                         void* callbackArg,
                                                         encap_type_t encap_type,
                                                         uint8_t version );

//----------------------------------------------------------------------------
// Color Switch Start Level Change Command
//
// @param: node_id
// Destination Node Id
//
// @param: channel_id
// Destination channel Id
//
// @param: up_down
// Destination Up Down (1 bits)
//
// @param: ignore_start_state
// Destination Ignore Start State (1 bits)
//
// @param: cl_component_id
// Destination Color Component ID
//
// @param: start_level
// Destination Start Level
//
// @param: duration
// Destination Duration
//
// @param: Callback
// @default: NULL
// Custom function to be called on function success
// NULL if callback is not needed
//
// @param: callbackArg
// Custom argument to be passed to custom function to be called on function success or failure
//
// @param: version
// CC version
//
znet_error_t ICACHE_FLASH_ATTR znet_cc_color_switch_start_level_change( uint8_t node_id,
                                                                        uint8_t channel_id,
                                                                        bool down_direction,
                                                                        bool ignore_start_state,
                                                                        uint8_t cl_component_id,
                                                                        uint8_t start_level,
                                                                        uint8_t duration,
                                                                        ZFunction_Callback Callback,
                                                                        void* callbackArg,
                                                                        encap_type_t encap_type,
                                                                        uint8_t version );

//----------------------------------------------------------------------------
// Color Switch Stop Level Change Command
//
// @param: node_id
// Destination Node Id
//
// @param: channel_id
// Destination channel Id
//
// @param: cl_component_id
// Color Component ID
//
// @param: Callback
// @default: NULL
// Custom function to be called on function failure
// NULL if callback is not needed
//
// @param: callbackArg
// Custom argument to be passed to custom function to be called on function
// success or failure
//
znet_error_t ICACHE_FLASH_ATTR znet_cc_color_switch_stop_level_change( uint8_t node_id,
                                                                       uint8_t channel_id,
                                                                       uint8_t cl_component_id,
                                                                       ZFunction_Callback Callback,
                                                                       void* callbackArg,
                                                                       encap_type_t encap_type );

//////////////////////////////////////////////////////////////////////////////
// Command Class MTP WINDOW COVERING //

//----------------------------------------------------------------------------
// Move To Position Get Command
//
// @param: node_id
// Destination Node Id
//
// @param: value
// Value
//
// @param: Callback
// @default: NULL
// Custom function to be called on function failure
// NULL if callback is not needed
//
// @param: callbackArg
// Custom argument to be passed to custom function to be called on function
// success or failure
//
znet_error_t ICACHE_FLASH_ATTR znet_cc_move_to_position_set( uint8_t node_id,
                                                             uint8_t value,
                                                             ZFunction_Callback Callback,
                                                             void* callbackArg,
                                                             encap_type_t encap_type );

//----------------------------------------------------------------------------
// Move To Position Set Command
//
// @param: node_id
// Destination Node Id
//
// @param: Callback
// @default: NULL
// Custom function to be called on function failure
// NULL if callback is not needed
//
// @param: callbackArg
// Custom argument to be passed to custom function to be called on function
// success or failure
//
znet_error_t ICACHE_FLASH_ATTR znet_cc_move_to_position_get( uint8_t node_id,
                                                             ZFunction_Callback Callback,
                                                             void* callbackArg,
                                                             encap_type_t encap_type );

//////////////////////////////////////////////////////////////////////////////
// Command Class Language //

//----------------------------------------------------------------------------
// Language Set Command
//
// @param: node_id
// Destination Node Id
//
// @param: language1
// Language 1
//
// @param: language2
// Language 2
//
// @param: language3
// Language 3
//
// @param: country1
// Country 1
//
// @param: country2
// Country 2
//
// @param: Callback
// @default: NULL
// Custom function to be called on function failure
// NULL if callback is not needed
//
// @param: callbackArg
// Custom argument to be passed to custom function to be called on function
// success or failure
//
znet_error_t ICACHE_FLASH_ATTR znet_cc_language_set( uint8_t node_id,
                                                     uint8_t language1,
                                                     uint8_t language2,
                                                     uint8_t language3,
                                                     uint8_t country1,
                                                     uint8_t country2,
                                                     ZFunction_Callback Callback,
                                                     void* callbackArg,
                                                     encap_type_t encap_type );

//----------------------------------------------------------------------------
// Language Get Command
//
// @param: node_id
// Destination Node Id
//
// @param: Callback
// @default: NULL
// Custom function to be called on function failure
// NULL if callback is not needed
//
// @param: callbackArg
// Custom argument to be passed to custom function to be called on function
// success or failure
//
znet_error_t ICACHE_FLASH_ATTR znet_cc_language_get( uint8_t node_id,
                                                     ZFunction_Callback Callback,
                                                     void* callbackArg,
                                                     encap_type_t encap_type );

//////////////////////////////////////////////////////////////////////////////
// Command Class Hrv Status //

//----------------------------------------------------------------------------
// Hrv Status Get Command
//
// @param: node_id
// Destination Node Id
//
// @param: status_param
// Status Parameter
//
// @param: Callback
// @default: NULL
// Custom function to be called on function failure
// NULL if callback is not needed
//
// @param: callbackArg
// Custom argument to be passed to custom function to be called on function
// success or failure
//
znet_error_t ICACHE_FLASH_ATTR znet_cc_hrv_status_get( uint8_t node_id,
                                                       uint8_t status_param,
                                                       ZFunction_Callback Callback,
                                                       void* callbackArg,
                                                       encap_type_t encap_type );

//----------------------------------------------------------------------------
// Hrv Status Supported Get Command
//
// @param: node_id
// Destination Node Id
//
// @param: Callback
// @default: NULL
// Custom function to be called on function failure
// NULL if callback is not needed
//
// @param: callbackArg
// Custom argument to be passed to custom function to be called on function
// success or failure
//
znet_error_t ICACHE_FLASH_ATTR znet_cc_hrv_status_supported_get( uint8_t node_id,
                                                                 ZFunction_Callback Callback,
                                                                 void* callbackArg,
                                                                 encap_type_t encap_type );

//////////////////////////////////////////////////////////////////////////////
// Command Class Hrv Control //

//----------------------------------------------------------------------------
// Hrv Control Bypass Get Command
//
// @param: node_id
// Destination Node Id
//
// @param: Callback
// @default: NULL
// Custom function to be called on function failure
// NULL if callback is not needed
//
// @param: callbackArg
// Custom argument to be passed to custom function to be called on function
// success or failure
//
znet_error_t ICACHE_FLASH_ATTR znet_cc_hrv_control_bypass_get( uint8_t node_id,
                                                               ZFunction_Callback Callback,
                                                               void* callbackArg,
                                                               encap_type_t encap_type );

//----------------------------------------------------------------------------
// Hrv Control Bypass Set Command
//
// @param: node_id
// Destination Node Id
//
// @param: bypass
// Bypass
//
// @param: Callback
// @default: NULL
// Custom function to be called on function failure
// NULL if callback is not needed
//
// @param: callbackArg
// Custom argument to be passed to custom function to be called on function
// success or failure
//
znet_error_t ICACHE_FLASH_ATTR znet_cc_hrv_control_bypass_set( uint8_t node_id,
                                                               uint8_t bypass,
                                                               ZFunction_Callback Callback,
                                                               void* callbackArg,
                                                               encap_type_t encap_type );

//----------------------------------------------------------------------------
// Hrv Control Mode Get Command
//
// @param: node_id
// Destination Node Id
//
// @param: Callback
// @default: NULL
// Custom function to be called on function failure
// NULL if callback is not needed
//
// @param: callbackArg
// Custom argument to be passed to custom function to be called on function
// success or failure
//
znet_error_t ICACHE_FLASH_ATTR znet_cc_hrv_control_mode_get( uint8_t node_id,
                                                             ZFunction_Callback Callback,
                                                             void* callbackArg,
                                                             encap_type_t encap_type );

//----------------------------------------------------------------------------
// Hrv Control Mode Set Command
//
// @param: node_id
// Destination Node Id
//
// @param: mode
// Mode
//
// @param: Callback
// @default: NULL
// Custom function to be called on function failure
// NULL if callback is not needed
//
// @param: callbackArg
// Custom argument to be passed to custom function to be called on function
// success or failure
//
znet_error_t ICACHE_FLASH_ATTR znet_cc_hrv_control_mode_set( uint8_t node_id,
                                                             uint8_t mode,
                                                             ZFunction_Callback Callback,
                                                             void* callbackArg,
                                                             encap_type_t encap_type );

//----------------------------------------------------------------------------
// Hrv Control Mode Supported Get Command
//
// @param: node_id
// Destination Node Id
//
// @param: Callback
// @default: NULL
// Custom function to be called on function failure
// NULL if callback is not needed
//
// @param: callbackArg
// Custom argument to be passed to custom function to be called on function
// success or failure
//
znet_error_t ICACHE_FLASH_ATTR znet_cc_hrv_control_mode_supported_get( uint8_t node_id,
                                                                       ZFunction_Callback Callback,
                                                                       void* callbackArg,
                                                                       encap_type_t encap_type );

//----------------------------------------------------------------------------
// Hrv Control Ventilation Rate Get Command
//
// @param: node_id
// Destination Node Id
//
// @param: Callback
// @default: NULL
// Custom function to be called on function failure
// NULL if callback is not needed
//
// @param: callbackArg
// Custom argument to be passed to custom function to be called on function
// success or failure
//
znet_error_t ICACHE_FLASH_ATTR znet_cc_hrv_control_ventilation_rate_get( uint8_t node_id,
                                                                         ZFunction_Callback Callback,
                                                                         void* callbackArg,
                                                                         encap_type_t encap_type );

//----------------------------------------------------------------------------
// Hrv Control Ventilation Rate Set Command
//
// @param: node_id
// Destination Node Id
//
// @param: ventilation_rate
// Ventilation Rate
//
// @param: Callback
// @default: NULL
// Custom function to be called on function failure
// NULL if callback is not needed
//
// @param: callbackArg
// Custom argument to be passed to custom function to be called on function
// success or failure
//
znet_error_t ICACHE_FLASH_ATTR znet_cc_hrv_control_ventilation_rate_set( uint8_t node_id,
                                                                         uint8_t ventilation_rate,
                                                                         ZFunction_Callback Callback,
                                                                         void* callbackArg,
                                                                         encap_type_t encap_type );

//////////////////////////////////////////////////////////////////////////////
// Command Class Entry Control//

//----------------------------------------------------------------------------
// Entry Control Notification Command
//
void ICACHE_FLASH_ATTR znet_cc_entry_control_notification( const ZFunction func,
                                                           uint8_t node_id,
                                                           int cc_data_len,
                                                           const uint8_t* cc_data );

//----------------------------------------------------------------------------
// Entry Control Key Supported Get Command
//
// @param: node_id
// Destination Node Id
//
// @param: channel_id
// Destination channel Id
//
// @param: Callback
// @default: NULL
// Custom function to be called on function success
// NULL if callback is not needed
//
// @param: callbackArg
// Custom argument to be passed to custom function to be called on function success or failure
//
// @param: encap_type
// Defines which Transport Service use to send commands
//
bool ICACHE_FLASH_ATTR znet_cc_entry_control_key_supported_get( uint8_t node_id,
                                                                uint8_t channel_id,
                                                                ZFunction_Callback Callback,
                                                                void* callbackArg,
                                                                encap_type_t encap_type );

//----------------------------------------------------------------------------
// Entry Control Key Supported Report Command
//
void ICACHE_FLASH_ATTR znet_cc_entry_control_key_supported_report( const ZFunction func,
                                                                   uint8_t node_id,
                                                                   int cc_data_len,
                                                                   const uint8_t* cc_data );

//----------------------------------------------------------------------------
// Entry Control Event Supported Get Command
//
// @param: node_id
// Destination Node Id
//
// @param: channel_id
// Destination channel Id
//
// @param: Callback
// @default: NULL
// Custom function to be called on function success
// NULL if callback is not needed
//
// @param: callbackArg
// Custom argument to be passed to custom function to be called on function success or failure
//
// @param: encap_type
// Defines which Transport Service use to send commands
//
bool ICACHE_FLASH_ATTR znet_cc_entry_control_event_supported_get( uint8_t node_id,
                                                                  uint8_t channel_id,
                                                                  ZFunction_Callback Callback,
                                                                  void* callbackArg,
                                                                  encap_type_t encap_type );

//----------------------------------------------------------------------------
// Entry Control Key Supported Report Command
//
void ICACHE_FLASH_ATTR znet_cc_entry_control_event_supported_report( const ZFunction func,
                                                                     uint8_t node_id,
                                                                     int cc_data_len,
                                                                     const uint8_t* cc_data );

//----------------------------------------------------------------------------
// Entry Control Configuration Set Command
//
// @param: node_id
// Destination Node Id
//
// @param: channel_id
// Destination channel Id
//
// @param: key_cache_size
// Key Cache Size
//
// @param: key_cache_timeout
// Key Cache Timeout
//
// @param: Callback
// @default: NULL
// Custom function to be called on function success
// NULL if callback is not needed
//
// @param: callbackArg
// Custom argument to be passed to custom function to be called on function success or failure
//
// @param: encap_type
// Defines which Transport Service use to send commands
//
bool ICACHE_FLASH_ATTR znet_cc_entry_control_configuration_set( uint8_t node_id,
                                                                uint8_t channel_id,
                                                                uint8_t key_cache_size,
                                                                uint8_t key_cache_timeout,
                                                                ZFunction_Callback Callback,
                                                                void* callbackArg,
                                                                encap_type_t encap_type );
//----------------------------------------------------------------------------
// Entry Control Configuration Get Command
//
// @param: node_id
// Destination Node Id
//
// @param: channel_id
// Destination channel Id
//
// @param: Callback
// @default: NULL
// Custom function to be called on function success
// NULL if callback is not needed
//
// @param: callbackArg
// Custom argument to be passed to custom function to be called on function success or failure
//
// @param: encap_type
// Defines which Transport Service use to send commands
//
bool ICACHE_FLASH_ATTR znet_cc_entry_control_configuration_get( uint8_t node_id,
                                                                uint8_t channel_id,
                                                                ZFunction_Callback Callback,
                                                                void* callbackArg,
                                                                encap_type_t encap_type );

//----------------------------------------------------------------------------
// Entry Control Key Supported Report Command
//
void ICACHE_FLASH_ATTR znet_cc_entry_control_configuration_report( const ZFunction func,
                                                                   uint8_t node_id,
                                                                   int cc_data_len,
                                                                   const uint8_t* cc_data );

//////////////////////////////////////////////////////////////////////////////
// Command Class Switch Toggle Binary//

//----------------------------------------------------------------------------
// Switch Toggle Binary Set Command
//
// @param: node_id
// Destination Node Id
//
// @param: Callback
// @default: NULL
// Custom function to be called on function failure
// NULL if callback is not needed
//
// @param: callbackArg
// Custom argument to be passed to custom function to be called on function
// success or failure
//
znet_error_t ICACHE_FLASH_ATTR znet_cc_switch_toggle_binary_set( uint8_t node_id,
                                                                 ZFunction_Callback Callback,
                                                                 void* callbackArg,
                                                                 encap_type_t encap_type );

//----------------------------------------------------------------------------
// Switch Toggle Binary Get Command
//
// @param: node_id
// Destination Node Id
//
// @param: Callback
// @default: NULL
// Custom function to be called on function failure
// NULL if callback is not needed
//
// @param: callbackArg
// Custom argument to be passed to custom function to be called on function
// success or failure
//
znet_error_t ICACHE_FLASH_ATTR znet_cc_switch_toggle_binary_get( uint8_t node_id,
                                                                 ZFunction_Callback Callback,
                                                                 void* callbackArg,
                                                                 encap_type_t encap_type );

//////////////////////////////////////////////////////////////////////////////
// Command Class Basic Traffic Info//

//----------------------------------------------------------------------------
// Basic Tariff Info Get Command
//
// @param: node_id
// Destination Node Id
//
// @param: Callback
// @default: NULL
// Custom function to be called on function failure
// NULL if callback is not needed
//
// @param: callbackArg
// Custom argument to be passed to custom function to be called on function
// success or failure
//
znet_error_t ICACHE_FLASH_ATTR znet_cc_basic_traffic_info_get( uint8_t node_id,
                                                               ZFunction_Callback Callback,
                                                               void* callbackArg,
                                                               encap_type_t encap_type );

//////////////////////////////////////////////////////////////////////////////
// Command Class Silence Alarm//

//----------------------------------------------------------------------------
// Sensor Alarm Set Command
//
// @param: node_id
// Destination Node Id
//
// @param: Callback
// @default: NULL
// Custom function to be called on function failure
// NULL if callback is not needed
//
// @param: callbackArg
// Custom argument to be passed to custom function to be called on function
// success or failure
//
znet_error_t ICACHE_FLASH_ATTR znet_cc_sensor_alarm_set( uint8_t node_id,
                                                         uint8_t mode,
                                                         uint16_t seconds,
                                                         uint8_t number_bit_masks,
                                                         const uint8_t* bit_mask,
                                                         ZFunction_Callback Callback,
                                                         void* callbackArg,
                                                         encap_type_t encap_type );

//////////////////////////////////////////////////////////////////////////////
// Command Class Lock//

//----------------------------------------------------------------------------
// Lock Set Command
//
// @param: node_id
// Destination Node Id
//
// @param: Callback
// @default: NULL
// Custom function to be called on function failure
// NULL if callback is not needed
//
// @param: callbackArg
// Custom argument to be passed to custom function to be called on function
// success or failure
//
znet_error_t ICACHE_FLASH_ATTR znet_cc_lock_get( uint8_t node_id,
                                                 ZFunction_Callback Callback,
                                                 void* callbackArg,
                                                 encap_type_t encap_type );

//----------------------------------------------------------------------------
// Lock Set Command
//
// @param: node_id
// Destination Node Id
//
// @param: lock_state
// Lock State
//
// @param: Callback
// @default: NULL
// Custom function to be called on function failure
// NULL if callback is not needed
//
// @param: callbackArg
// Custom argument to be passed to custom function to be called on function
// success or failure
//
znet_error_t ICACHE_FLASH_ATTR znet_cc_lock_set( uint8_t node_id,
                                                 uint8_t lock_state,
                                                 ZFunction_Callback Callback,
                                                 void* callbackArg,
                                                 encap_type_t encap_type );

//////////////////////////////////////////////////////////////////////////////
// Command Class Geofraphic Location//

//----------------------------------------------------------------------------
// Geographic Location Get Command
//
// @param: node_id
// Destination Node Id
//
// @param: Callback
// @default: NULL
// Custom function to be called on function failure
// NULL if callback is not needed
//
// @param: callbackArg
// Custom argument to be passed to custom function to be called on function
// success or failure
//
znet_error_t ICACHE_FLASH_ATTR znet_cc_geographic_location_get( uint8_t node_id,
                                                                ZFunction_Callback Callback,
                                                                void* callbackArg,
                                                                encap_type_t encap_type );

//----------------------------------------------------------------------------
// Lock Set Command
//
// @param: node_id
// Destination Node Id
//
// @param: long_degrees
// Longitude Degrees
//
// @param: long_minutes
// Longitude Minutes (7 bits)
//
// @param: long_sign
// Long. Sign (1 bits)
//
// @param: lat_degrees
// Latitude Degrees
//
// @param: lat_minutes
// Latitude Minutes (7 bits)
//
// @param: lat_sign
// Lat. Sign (1 bits)
//
// @param: Callback
// @default: NULL
// Custom function to be called on function failure
// NULL if callback is not needed
//
// @param: callbackArg
// Custom argument to be passed to custom function to be called on function
// success or failure
//
znet_error_t ICACHE_FLASH_ATTR znet_cc_geographic_location_set( uint8_t node_id,
                                                                uint8_t long_degrees,
                                                                uint8_t long_minutes,
                                                                bool long_sign,
                                                                uint8_t lat_degrees,
                                                                uint8_t lat_minutes,
                                                                bool lat_sign,
                                                                ZFunction_Callback Callback,
                                                                void* callbackArg,
                                                                encap_type_t encap_type );

//////////////////////////////////////////////////////////////////////////////
// Command Class Proprietary//

//----------------------------------------------------------------------------
// Proprietary Get Command
//
// @param: node_id
// Destination Node Id
//
// @param: data
// Data
//
// @param: Callback
// @default: NULL
// Custom function to be called on function failure
// NULL if callback is not needed
//
// @param: callbackArg
// Custom argument to be passed to custom function to be called on function
// success or failure
//
znet_error_t ICACHE_FLASH_ATTR znet_cc_proprietary_get( uint8_t node_id,
                                                        uint8_t cmd_data_length,
                                                        const uint8_t* cmd_data,
                                                        ZFunction_Callback Callback,
                                                        void* callbackArg,
                                                        encap_type_t encap_type );

//----------------------------------------------------------------------------
// Proprietary Set Command
//
// @param: node_id
// Destination Node Id
//
// @param: data
// Data
//
// @param: Callback
// @default: NULL
// Custom function to be called on function failure
// NULL if callback is not needed
//
// @param: callbackArg
// Custom argument to be passed to custom function to be called on function
// success or failure
//
znet_error_t ICACHE_FLASH_ATTR znet_cc_proprietary_set( uint8_t node_id,
                                                        uint8_t cmd_data_length,
                                                        const uint8_t* cmd_data,
                                                        ZFunction_Callback Callback,
                                                        void* callbackArg,
                                                        encap_type_t encap_type );

//////////////////////////////////////////////////////////////////////////////
// Command Class Meter Tbl Push//

//----------------------------------------------------------------------------
// Meter Tbl Push Configuration Get Command
//
// @param: node_id
// Destination Node Id
//
// @param: Callback
// @default: NULL
// Custom function to be called on function failure
// NULL if callback is not needed
//
// @param: callbackArg
// Custom argument to be passed to custom function to be called on function
// success or failure
//
znet_error_t ICACHE_FLASH_ATTR znet_cc_meter_tbl_push_config_get( uint8_t node_id,
                                                                  ZFunction_Callback Callback,
                                                                  void* callbackArg,
                                                                  encap_type_t encap_type );

//----------------------------------------------------------------------------
// Meter Tbl Push Configuration Set Command
//
// @param: node_id
// Destination Node Id
//
// @param: oper_status
// Operating Status Push Mode (4 bits)
//
// @param: ps
// PS (1 bits)
//
// @param: push_dataset
// Push Dataset (24 bits)
//
// @param: interval_months
// Interval Months
//
// @param: interval_days
// Interval Days
//
// @param: interval_hours
// Interval Hours
//
// @param: interval_minutes
// Interval Minutes
//
// @param: push_node_id
// Push Node ID
//
// @param: Callback
// @default: NULL
// Custom function to be called on function failure
// NULL if callback is not needed
//
// @param: callbackArg
// Custom argument to be passed to custom function to be called on function
// success or failure
//
znet_error_t ICACHE_FLASH_ATTR znet_cc_meter_tbl_push_config_set( uint8_t node_id,
                                                                  uint8_t oper_status,
                                                                  bool ps,
                                                                  uint32_t push_dataset,
                                                                  uint8_t interval_months,
                                                                  uint8_t interval_days,
                                                                  uint8_t interval_hours,
                                                                  uint8_t interval_minutes,
                                                                  uint8_t push_node_id,
                                                                  ZFunction_Callback Callback,
                                                                  void* callbackArg,
                                                                  encap_type_t encap_type );

//////////////////////////////////////////////////////////////////////////////
// Command Class Simple Av Control

//----------------------------------------------------------------------------
// Simple Av Control Get Command
//
// @param: node_id
// Destination Node Id
//
// @param: Callback
// @default: NULL
// Custom function to be called on function failure
// NULL if callback is not needed
//
// @param: callbackArg
// Custom argument to be passed to custom function to be called on function
// success or failure
//
znet_error_t ICACHE_FLASH_ATTR znet_cc_simple_av_control_get( uint8_t node_id,
                                                              ZFunction_Callback Callback,
                                                              void* callbackArg,
                                                              encap_type_t encap_type );

//----------------------------------------------------------------------------
// Simple Av Control Set Command
//
// @param: node_id
// Destination Node Id
//
// @param: seq_number
// Sequence Number
//
// @param: key_attributes
// Key Attributes (3 bits)
//
// @param: item_ID
// Item ID
//
// @param: cmd_data_length
//
// @param: cmd_data
//
// @param: Callback
// @default: NULL
// Custom function to be called on function failure or success
// NULL if callback is not needed
//
// @param: callbackArg
// Custom argument to be passed to custom function to be called on function
// success or failure
//
znet_error_t ICACHE_FLASH_ATTR znet_cc_simple_av_control_set( uint8_t node_id,
                                                              uint8_t seq_number,
                                                              uint8_t key_attributes,
                                                              uint16_t item_ID,
                                                              uint8_t cmd_data_length,
                                                              const uint8_t* cmd_data,
                                                              ZFunction_Callback Callback,
                                                              void* callbackArg,
                                                              encap_type_t encap_type );

//----------------------------------------------------------------------------
// Simple Av Control Supported Get Command
//
// @param: node_id
// Destination Node Id
//
// @param: report_n
// Report No
//
// @param: Callback
// @default: NULL
// Custom function to be called on function failure or success
// NULL if callback is not needed
//
// @param: callbackArg
// Custom argument to be passed to custom function to be called on function
// success or failure
//
znet_error_t ICACHE_FLASH_ATTR znet_cc_simple_av_control_supported_get( uint8_t node_id,
                                                                        uint8_t report_n,
                                                                        ZFunction_Callback Callback,
                                                                        void* callbackArg,
                                                                        encap_type_t encap_type );

//////////////////////////////////////////////////////////////////////////////
// Command Class Meter Tbl Config

//----------------------------------------------------------------------------
// Meter Tbl Table Point Adm No Set Command
//
// @param: node_id
// Destination Node Id
//
// @param: number
// Number of Characters (5 bits)
//
// @param: characters
// Meter Point Adm Number Character
//
// @param: Callback
// @default: NULL
// Custom function to be called on function failure or success
// NULL if callback is not needed
//
// @param: callbackArg
// Custom argument to be passed to custom function to be called on function
// success or failure
//
znet_error_t ICACHE_FLASH_ATTR znet_cc_meter_tbl_table_point_adm_no_set( uint8_t node_id,
                                                                         uint8_t number,
                                                                         const uint8_t* characters,
                                                                         ZFunction_Callback Callback,
                                                                         void* callbackArg,
                                                                         encap_type_t encap_type );

//////////////////////////////////////////////////////////////////////////////
// Command Class Barrier Operator

//----------------------------------------------------------------------------
// Barrier Operator Set Command
//
// @param: node_id
// Destination Node Id
//
// @param: channel_id
// Destination channel Id
//
// @param: target
// Target Value
//
// @param: Callback
// @default: NULL
// Custom function to be called on function failure or success
// NULL if callback is not needed
//
// @param: callbackArg
// Custom argument to be passed to custom function to be called on function
// success or failure
//
// @param: encap_type
// Defines which Transport Service use to send commands
//
znet_error_t ICACHE_FLASH_ATTR znet_cc_barrier_operator_set( uint8_t node_id,
                                                             uint8_t channel_id,
                                                             uint8_t target,
                                                             ZFunction_Callback Callback,
                                                             void* callbackArg,
                                                             encap_type_t encap_type );

//----------------------------------------------------------------------------
// Barrier Operator Get Command
//
// @param: node_id
// Destination Node Id
//
// @param: channel_id
// Destination channel Id
//
// @param: Callback
// @default: NULL
// Custom function to be called on function failure or success
// NULL if callback is not needed
//
// @param: callbackArg
// Custom argument to be passed to custom function to be called on function
// success or failure
//
// @param: encap_type
// Defines which Transport Service use to send commands
//
znet_error_t ICACHE_FLASH_ATTR znet_cc_barrier_operator_get( uint8_t node_id,
                                                             uint8_t channel_id,
                                                             ZFunction_Callback Callback,
                                                             void* callbackArg,
                                                             encap_type_t encap_type );

//----------------------------------------------------------------------------
// Barrier Operator Signal Supported Get Command
//
// @param: node_id
// Destination Node Id
//
// @param: channel_id
// Destination channel Id
//
// @param: Callback
// @default: NULL
// Custom function to be called on function failure or success
// NULL if callback is not needed
//
// @param: callbackArg
// Custom argument to be passed to custom function to be called on function
// success or failure
//
// @param: encap_type
// Defines which Transport Service use to send commands
//
znet_error_t ICACHE_FLASH_ATTR znet_cc_barrier_operator_signal_supported_get( uint8_t node_id,
                                                                              uint8_t channel_id,
                                                                              ZFunction_Callback Callback,
                                                                              void* callbackArg,
                                                                              encap_type_t encap_type );

//----------------------------------------------------------------------------
// Barrier Operator Signal Set Command
//
// @param: node_id
// Destination Node Id
//
// @param: channel_id
// Destination channel Id
//
// @param: type
// Subsystem Type
//
// @param: state
// Subsystem State
//
// @param: Callback
// @default: NULL
// Custom function to be called on function failure or success
// NULL if callback is not needed
//
// @param: callbackArg
// Custom argument to be passed to custom function to be called on function
// success or failure
//
// @param: encap_type
// Defines which Transport Service use to send commands
//
znet_error_t ICACHE_FLASH_ATTR znet_cc_barrier_operator_signal_set( uint8_t node_id,
                                                                    uint8_t channel_id,
                                                                    uint8_t type,
                                                                    uint8_t state,
                                                                    ZFunction_Callback Callback,
                                                                    void* callbackArg,
                                                                    encap_type_t encap_type );

//----------------------------------------------------------------------------
// Barrier Operator Signal Get Command
//
// @param: node_id
// Destination Node Id
//
// @param: channel_id
// Destination channel Id
//
// @param: type
// Subsystem Type
//
// @param: Callback
// @default: NULL
// Custom function to be called on function failure or success
// NULL if callback is not needed
//
// @param: callbackArg
// Custom argument to be passed to custom function to be called on function
// success or failure
//
// @param: encap_type
// Defines which Transport Service use to send commands
//
znet_error_t ICACHE_FLASH_ATTR znet_cc_barrier_operator_signal_get( uint8_t node_id,
                                                                    uint8_t channel_id,
                                                                    uint8_t type,
                                                                    ZFunction_Callback Callback,
                                                                    void* callbackArg,
                                                                    encap_type_t encap_type );

//----------------------------------------------------------------------------
// BarrierOperator Report CB
//
void ICACHE_FLASH_ATTR znet_cc_barrier_operator_report( const ZFunction func,
                                                        uint8_t node_id,
                                                        int cc_data_len,
                                                        const uint8_t* cc_data );

//----------------------------------------------------------------------------
// BarrierOperator Signal Report CB
//
void ICACHE_FLASH_ATTR znet_cc_barrier_operator_signal_report( const ZFunction func,
                                                               uint8_t node_id,
                                                               int cc_data_len,
                                                               const uint8_t* cc_data );

//----------------------------------------------------------------------------
// BarrierOperator Supported Signals Report CB
//
void ICACHE_FLASH_ATTR znet_cc_barrier_operator_supported_signals_report( const ZFunction func,
                                                                          uint8_t node_id,
                                                                          int cc_data_len,
                                                                          const uint8_t* cc_data );

//////////////////////////////////////////////////////////////////////////////
// Command Class Screen Atributes

//----------------------------------------------------------------------------
// Screen Attributes Get Command
//
// @param: node_id
// Destination Node Id
//
// @param: Callback
// @default: NULL
// Custom function to be called on function failure or success
// NULL if callback is not needed
//
// @param: callbackArg
// Custom argument to be passed to custom function to be called on function
// success or failure
//
znet_error_t ICACHE_FLASH_ATTR znet_cc_screen_attributes_get( uint8_t node_id,
                                                              ZFunction_Callback Callback,
                                                              void* callbackArg,
                                                              encap_type_t encap_type );

//////////////////////////////////////////////////////////////////////////////
// Command Class Thermostat Setback

//----------------------------------------------------------------------------
// Thermostat Setback Get Command
//
// @param: node_id
// Destination Node Id
//
// @param: channel_id
// Destination channel Id
//
// @param: Callback
// @default: NULL
// Custom function to be called on function failure
// NULL if callback is not needed
//
// @param: callbackArg
// Custom argument to be passed to custom function to be called on function
// success or failure
//
znet_error_t ICACHE_FLASH_ATTR znet_cc_thermostat_setback_get( uint8_t node_id,
                                                               uint8_t channel_id,
                                                               ZFunction_Callback Callback,
                                                               void* callbackArg,
                                                               encap_type_t encap_type );
//----------------------------------------------------------------------------
// Thermostat Setback Report Command
//
void ICACHE_FLASH_ATTR znet_cc_thermostat_setback_report( const ZFunction func,
                                                          uint8_t node_id,
                                                          int cc_data_len,
                                                          const uint8_t* cc_data );
//----------------------------------------------------------------------------
// Thermostat Setback Set Command
//
// @param: node_id
// Destination Node Id
//
// @param: channel_id
// Destination channel Id
//
// @param: type
// Setback Type (2 bits)
//
// @param: state
// Setback State
//
// @param: Callback
// @default: NULL
// Custom function to be called on function failure
// NULL if callback is not needed
//
// @param: callbackArg
// Custom argument to be passed to custom function to be called on function
// success or failure
//
znet_error_t ICACHE_FLASH_ATTR znet_cc_thermostat_setback_set( uint8_t node_id,
                                                               uint8_t channel_id,
                                                               uint8_t type,
                                                               int8_t state,
                                                               ZFunction_Callback Callback,
                                                               void* callbackArg,
                                                               encap_type_t encap_type );

//////////////////////////////////////////////////////////////////////////////
// Command Class Screen Md

//----------------------------------------------------------------------------
// Screen Md Get Command
//
// @param: node_id
// Destination Node Id
//
// @param: n_reports
// Number of Reports
//
// @param: Callback
// @default: NULL
// Custom function to be called on function failure or success
// NULL if callback is not needed
//
// @param: callbackArg
// Custom argument to be passed to custom function to be called on function
// success or failure
//
znet_error_t ICACHE_FLASH_ATTR znet_cc_screen_md_get( uint8_t node_id,
                                                      uint8_t n_reports,
                                                      ZFunction_Callback Callback,
                                                      void* callbackArg,
                                                      encap_type_t encap_type );

//////////////////////////////////////////////////////////////////////////////
// Command Class Meter Pulse

//----------------------------------------------------------------------------
// HMeter Pulse Get Command
//
// @param: node_id
// Destination Node Id
//
// @param: Callback
// @default: NULL
// Custom function to be called on function failure or success
// NULL if callback is not needed
//
// @param: callbackArg
// Custom argument to be passed to custom function to be called on function
// success or failure
//
znet_error_t ICACHE_FLASH_ATTR znet_cc_meter_pulse_get( uint8_t node_id,
                                                        ZFunction_Callback Callback,
                                                        void* callbackArg,
                                                        encap_type_t encap_type );

//////////////////////////////////////////////////////////////////////////////
// Command Class Basic Window Covering

//----------------------------------------------------------------------------
// Basic Window Covering Start Level Change Command
//
// @param: node_id
// Destination Node Id
//
// @param: open_close
// Open/ Close (1 bits)
//
// @param: Callback
// @default: NULL
// Custom function to be called on function failure or success
// NULL if callback is not needed
//
// @param: callbackArg
// Custom argument to be passed to custom function to be called on function
// success or failure
//
znet_error_t ICACHE_FLASH_ATTR znet_cc_basic_window_covering_start_level_change( uint8_t node_id,
                                                                                 bool open_close,
                                                                                 ZFunction_Callback Callback,
                                                                                 void* callbackArg,
                                                                                 encap_type_t encap_type );

//----------------------------------------------------------------------------
// Basic Window Covering Stop Level Change Command
//
// @param: node_id
// Destination Node Id
//
// @param: Callback
// @default: NULL
// Custom function to be called on function failure or success
// NULL if callback is not needed
//
// @param: callbackArg
// Custom argument to be passed to custom function to be called on function
// success or failure
//
znet_error_t ICACHE_FLASH_ATTR znet_cc_basic_window_covering_stop_level_change( uint8_t node_id,
                                                                                ZFunction_Callback Callback,
                                                                                void* callbackArg,
                                                                                encap_type_t encap_type );

//////////////////////////////////////////////////////////////////////////////
// Command Class Meter Tbl Monitor

//----------------------------------------------------------------------------
// Meter Tbl Status Date Get Command
//
// @param: node_id
// Destination Node Id
//
// @param: max_reports
// Maximum Reports
//
// @param: start_year
// Start Year
//
// @param: start_month
// Start Month
//
// @param: start_day
// Start Day
//
// @param: start_hour
// Start Hour Local Time
//
// @param: start_minute
// Start Minute Local Time
//
// @param: start_second
// Start Second Local Time
//
// @param: stop_year
// Stop Year
//
// @param: stop_month
// Stop Month
//
// @param: stop_day
// Stop Day
//
// @param: stop_hour
// Stop Hour Local Time
//
// @param: stop_minute
// Stop Minute Local Time
//
// @param: stop_second
// Stop Second Local Time
//
// @param: Callback
// @default: NULL
// Custom function to be called on function failure or success
// NULL if callback is not needed
//
// @param: callbackArg
// Custom argument to be passed to custom function to be called on function
// success or failure
//
znet_error_t ICACHE_FLASH_ATTR znet_cc_meter_tbl_status_date_get( uint8_t node_id,
                                                                  uint8_t max_reports,
                                                                  uint16_t start_year,
                                                                  uint8_t start_month,
                                                                  uint8_t start_day,
                                                                  uint8_t start_hour,
                                                                  uint8_t start_minute,
                                                                  uint8_t start_second,
                                                                  uint16_t stop_year,
                                                                  uint8_t stop_month,
                                                                  uint8_t stop_day,
                                                                  uint8_t stop_hour,
                                                                  uint8_t stop_minute,
                                                                  uint8_t stop_second,
                                                                  ZFunction_Callback Callback,
                                                                  void* callbackArg,
                                                                  encap_type_t encap_type );

//----------------------------------------------------------------------------
// Meter Tbl Status Depth Get Command
//
// @param: node_id
// Destination Node Id
//
// @param: status
// Status Event Log Depth
//
// @param: Callback
// @default: NULL
// Custom function to be called on function failure or success
// NULL if callback is not needed
//
// @param: callbackArg
// Custom argument to be passed to custom function to be called on function
// success or failure
//
znet_error_t ICACHE_FLASH_ATTR znet_cc_meter_tbl_status_depth_get( uint8_t node_id,
                                                                   uint8_t status,
                                                                   ZFunction_Callback Callback,
                                                                   void* callbackArg,
                                                                   encap_type_t encap_type );

//----------------------------------------------------------------------------
// Meter Tbl Status Supported Get Command
//
// @param: node_id
// Destination Node Id
//
// @param: Callback
// @default: NULL
// Custom function to be called on function failure or success
// NULL if callback is not needed
//
// @param: callbackArg
// Custom argument to be passed to custom function to be called on function
// success or failure
//
znet_error_t ICACHE_FLASH_ATTR znet_cc_meter_tbl_status_supported_get( uint8_t node_id,
                                                                       ZFunction_Callback Callback,
                                                                       void* callbackArg,
                                                                       encap_type_t encap_type );

//----------------------------------------------------------------------------
// Meter Tbl Current Data Get Command
//
// @param: node_id
// Destination Node Id
//
// @param: dataset_req
// Dataset Requested (24 bits)
//
// @param: Callback
// @default: NULL
// Custom function to be called on function failure or success
// NULL if callback is not needed
//
// @param: callbackArg
// Custom argument to be passed to custom function to be called on function
// success or failure
//
znet_error_t ICACHE_FLASH_ATTR znet_cc_meter_tbl_current_data_get( uint8_t node_id,
                                                                   uint32_t dataset_req,
                                                                   ZFunction_Callback Callback,
                                                                   void* callbackArg,
                                                                   encap_type_t encap_type );

//----------------------------------------------------------------------------
// Meter Tbl Historical Data Get Command
//
// @param: node_id
// Destination Node Id
//
// @param: max_reports
// Maximum Reports
//
// @param: historical_dataset
// Historical Dataset Requested (24 bits)
//
// @param: start_year
// Start Year
//
// @param: start_month
// Start Month
//
// @param: start_day
// Start Day
//
// @param: start_hour
// Start Hour Local Time
//
// @param: start_minute
// Start Minute Local Time
//
// @param: start_second
// Start Second Local Time
//
// @param: stop_year
// Stop Year
//
// @param: stop_month
// Stop Month
//
// @param: stop_day
// Stop Day
//
// @param: stop_hour
// Stop Hour Local Time
//
// @param: stop_minute
// Stop Minute Local Time
//
// @param: stop_second
// Stop Second Local Time
//
// @param: Callback
// @default: NULL
// Custom function to be called on function failure or success
// NULL if callback is not needed
//
// @param: callbackArg
// Custom argument to be passed to custom function to be called on function
// success or failure
//
znet_error_t ICACHE_FLASH_ATTR znet_cc_meter_tbl_historical_data_get( uint8_t node_id,
                                                                      uint8_t max_reports,
                                                                      uint32_t historical_dataset,
                                                                      uint16_t start_year,
                                                                      uint8_t start_month,
                                                                      uint8_t start_day,
                                                                      uint8_t start_hour,
                                                                      uint8_t start_minute,
                                                                      uint8_t start_second,
                                                                      uint16_t stop_year,
                                                                      uint8_t stop_month,
                                                                      uint8_t stop_day,
                                                                      uint8_t stop_hour,
                                                                      uint8_t stop_minute,
                                                                      uint8_t stop_second,
                                                                      ZFunction_Callback Callback,
                                                                      void* callbackArg,
                                                                      encap_type_t encap_type );

//----------------------------------------------------------------------------
// Meter Tbl Table Capability Get Command
//
// @param: node_id
// Destination Node Id
//
// @param: Callback
// @default: NULL
// Custom function to be called on function failure or success
// NULL if callback is not needed
//
// @param: callbackArg
// Custom argument to be passed to custom function to be called on function
// success or failure
//
znet_error_t ICACHE_FLASH_ATTR znet_cc_meter_tbl_table_capability_get( uint8_t node_id,
                                                                       ZFunction_Callback Callback,
                                                                       void* callbackArg,
                                                                       encap_type_t encap_type );

//----------------------------------------------------------------------------
// Meter Tbl Table Id Get Command
//
// @param: node_id
// Destination Node Id
//
// @param: Callback
// @default: NULL
// Custom function to be called on function failure or success
// NULL if callback is not needed
//
// @param: callbackArg
// Custom argument to be passed to custom function to be called on function
// success or failure
//
znet_error_t ICACHE_FLASH_ATTR znet_cc_meter_tbl_table_id_get( uint8_t node_id,
                                                               ZFunction_Callback Callback,
                                                               void* callbackArg,
                                                               encap_type_t encap_type );

//----------------------------------------------------------------------------
// Meter Tbl Table Point Adm No Get Command
//
// @param: node_id
// Destination Node Id
//
// @param: Callback
// @default: NULL
// Custom function to be called on function failure or success
// NULL if callback is not needed
//
// @param: callbackArg
// Custom argument to be passed to custom function to be called on function
// success or failure
//
znet_error_t ICACHE_FLASH_ATTR znet_cc_meter_tbl_point_adm_no_get( uint8_t node_id,
                                                                   ZFunction_Callback Callback,
                                                                   void* callbackArg,
                                                                   encap_type_t encap_type );

//////////////////////////////////////////////////////////////////////////////
// Command Class Climate Control Schedule

//----------------------------------------------------------------------------
// Schedule Changed Get Command
//
// @param: node_id
// Destination Node Id
//
// @param: Callback
// @default: NULL
// Custom function to be called on function failure or success
// NULL if callback is not needed
//
// @param: callbackArg
// Custom argument to be passed to custom function to be called on function
// success or failure
//
znet_error_t ICACHE_FLASH_ATTR znet_cc_shedule_changed_get( uint8_t node_id,
                                                            ZFunction_Callback Callback,
                                                            void* callbackArg,
                                                            encap_type_t encap_type );

//----------------------------------------------------------------------------
// Schedule Get Command
//
// @param: node_id
// Destination Node Id
//
// @param: weekday
// Weekday (3 bits)
//
// @param: Callback
// @default: NULL
// Custom function to be called on function failure or success
// NULL if callback is not needed
//
// @param: callbackArg
// Custom argument to be passed to custom function to be called on function
// success or failure
//
znet_error_t ICACHE_FLASH_ATTR znet_cc_climate_shedule_get( uint8_t node_id,
                                                            uint8_t weekday,
                                                            ZFunction_Callback Callback,
                                                            void* callbackArg,
                                                            encap_type_t encap_type );

//----------------------------------------------------------------------------
// Schedule Override Get Command
//
// @param: node_id
// Destination Node Id
//
// @param: Callback
// @default: NULL
// Custom function to be called on function failure or success
// NULL if callback is not needed
//
// @param: callbackArg
// Custom argument to be passed to custom function to be called on function
// success or failure
//
znet_error_t ICACHE_FLASH_ATTR znet_cc_shedule_override_get( uint8_t node_id,
                                                             ZFunction_Callback Callback,
                                                             void* callbackArg,
                                                             encap_type_t encap_type );

//----------------------------------------------------------------------------
// Schedule Override Set Command
//
// @param: node_id
// Destination Node Id
//
// @param: type
// Override Type (2 bits)
//
// @param: status
// Override State
//
// @param: Callback
// @default: NULL
// Custom function to be called on function failure or success
// NULL if callback is not needed
//
// @param: callbackArg
// Custom argument to be passed to custom function to be called on function
// success or failure
//
znet_error_t ICACHE_FLASH_ATTR znet_cc_shedule_override_set( uint8_t node_id,
                                                             uint8_t type,
                                                             uint8_t state,
                                                             ZFunction_Callback Callback,
                                                             void* callbackArg,
                                                             encap_type_t encap_type );

//----------------------------------------------------------------------------
// Schedule Set Command
//
// @param: node_id
// Destination Node Id
//
// @param: weekday
// Weekday (3 bits)
//
// @param: switchpoint0
// Switchpoint 0 (24 bits)
//
// @param: switchpoint1
// Switchpoint 1 (24 bits)
//
// @param: switchpoint2
// Switchpoint 2 (24 bits)
//
// @param: switchpoint3
// Switchpoint 3 (24 bits)
//
// @param: switchpoint4
// Switchpoint 4 (24 bits)
//
// @param: switchpoint5
// Switchpoint 5 (24 bits)
//
// @param: switchpoint6
// Switchpoint 6 (24 bits)
//
// @param: switchpoint7
// Switchpoint 7 (24 bits)
//
// @param: switchpoint9
// Switchpoint 8 (24 bits)
//
// @param: Callback
// @default: NULL
// Custom function to be called on function failure or success
// NULL if callback is not needed
//
// @param: callbackArg
// Custom argument to be passed to custom function to be called on function
// success or failure
//
znet_error_t ICACHE_FLASH_ATTR znet_cc_climate_shedule_set( uint8_t node_id,
                                                            uint8_t weekday,
                                                            uint32_t switchpoint0,
                                                            uint32_t switchpoint1,
                                                            uint32_t switchpoint2,
                                                            uint32_t switchpoint3,
                                                            uint32_t switchpoint4,
                                                            uint32_t switchpoint5,
                                                            uint32_t switchpoint6,
                                                            uint32_t switchpoint7,
                                                            uint32_t switchpoint8,
                                                            ZFunction_Callback Callback,
                                                            void* callbackArg,
                                                            encap_type_t encap_type );

//////////////////////////////////////////////////////////////////////////////

// Command Class Anti-theft

//----------------------------------------------------------------------------
// Anti-theft Set Command
//
// @param: node_id
// Destination Node Id
//
// @param: n_magic_code
// Number of Magic Code bytes (7 bits)
//
// @param: magic_code
// Magic Code
//
// @param: man_id
// Manufacturer ID (16 bits)
//
// @param: n_hint_number
// Anti-theft Hint Number Bytes
//
// @param: hint_number
// Anti-theft Hint Byte
//
// @param: Callback
// @default: NULL
// Custom function to be called on function failure or success
// NULL if callback is not needed
//
// @param: callbackArg
// Custom argument to be passed to custom function to be called on function
// success or failure
//
znet_error_t ICACHE_FLASH_ATTR znet_cc_antitheft_set( uint8_t node_id,
                                                      uint8_t n_magic_code,
                                                      const uint8_t* magic_code,
                                                      uint16_t man_id,
                                                      uint8_t n_hint_number,
                                                      const uint8_t* hint_number,
                                                      ZFunction_Callback Callback,
                                                      void* callbackArg,
                                                      encap_type_t encap_type );

//----------------------------------------------------------------------------
// Anti-theft Get Command
//
// @param: node_id
// Destination Node Id
//
// @param: Callback
// @default: NULL
// Custom function to be called on function failure or success
// NULL if callback is not needed
//
// @param: callbackArg
// Custom argument to be passed to custom function to be called on function
// success or failure
//
znet_error_t ICACHE_FLASH_ATTR znet_cc_antitheft_get( uint8_t node_id,
                                                      ZFunction_Callback Callback,
                                                      void* callbackArg,
                                                      encap_type_t encap_type );

//----------------------------------------------------------------------------
// Anti-theft Get Command
//
void ICACHE_FLASH_ATTR znet_cc_anti_theft_unlock_state_report( const ZFunction func,
                                                               uint8_t node_id,
                                                               int cc_data_len,
                                                               const uint8_t* cc_data );

//////////////////////////////////////////////////////////////////////////////
// Command Class Scene Controller Conf

//----------------------------------------------------------------------------
// Scene Controller Conf Get Command
//
// @param: node_id
// Destination Node Id
//
// @param: group_id
// Group ID
//
// @param: Callback
// @default: NULL
// Custom function to be called on function failure or success
// NULL if callback is not needed
//
// @param: callbackArg
// Custom argument to be passed to custom function to be called on function
// success or failure
//
znet_error_t ICACHE_FLASH_ATTR znet_cc_scene_controller_conf_get( uint8_t node_id,
                                                                  uint8_t channel_id,
                                                                  uint8_t group_id,
                                                                  ZFunction_Callback Callback,
                                                                  void* callbackArg,
                                                                  encap_type_t encap_type );

//----------------------------------------------------------------------------
// Scene Controller Conf Set Command
//
// @param: node_id
// Destination Node Id
//
// @param: channel_id
// Destination channel Id
//
// @param: group_id
// Group ID
//
// @param: scene_id
// Scene ID
//
// @param: dim_duration
// Dimming Duration
//
// @param: Callback
// @default: NULL
// Custom function to be called on function failure or success
// NULL if callback is not needed
//
// @param: callbackArg
// Custom argument to be passed to custom function to be called on function
// success or failure
//
znet_error_t ICACHE_FLASH_ATTR znet_cc_scene_controller_conf_set( uint8_t node_id,
                                                                  uint8_t channel_id,
                                                                  uint8_t group_id,
                                                                  uint8_t scene_id,
                                                                  uint8_t dim_duration,
                                                                  ZFunction_Callback Callback,
                                                                  void* callbackArg,
                                                                  encap_type_t encap_type );

void ICACHE_FLASH_ATTR znet_cc_scene_controller_conf_report( const ZFunction func,
                                                             uint8_t node_id,
                                                             int cc_data_len,
                                                             const uint8_t* cc_data );
//////////////////////////////////////////////////////////////////////////////
// Command Class Sensor Alarm

//----------------------------------------------------------------------------
// Sensor Alarm Get Command
//
// @param: node_id
// Destination Node Id
//
// @param: channel_id
// Destination channel Id
//
// @param: type
// Sensor Type
//
// @param: Callback
// @default: NULL
// Custom function to be called on function failure or success
// NULL if callback is not needed
//
// @param: callbackArg
// Custom argument to be passed to custom function to be called on function
// success or failure
//
// @param: encap_type
// Defines which Transport Service use to send commands
//
znet_error_t ICACHE_FLASH_ATTR znet_cc_sensor_alarm_get( uint8_t node_id,
                                                         uint8_t channel_id,
                                                         uint8_t type,
                                                         ZFunction_Callback Callback,
                                                         void* callbackArg,
                                                         encap_type_t encap_type );

//----------------------------------------------------------------------------
// Sensor Alarm Report Command
//
void ICACHE_FLASH_ATTR znet_cc_sensor_alarm_report( const ZFunction func,
                                                    uint8_t node_id,
                                                    int cc_data_len,
                                                    const uint8_t* cc_data );

//----------------------------------------------------------------------------
// Sensor Alarm Supported Get Command
//
// @param: node_id
// Destination Node Id
//
// @param: Callback
// @default: NULL
// Custom function to be called on function failure or success
// NULL if callback is not needed
//
// @param: callbackArg
// Custom argument to be passed to custom function to be called on function
// success or failure
//
// @param: encap_type
// Defines which Transport Service use to send commands
//
znet_error_t ICACHE_FLASH_ATTR znet_cc_sensor_alarm_supported_get( uint8_t node_id,
                                                                   uint8_t channel_id,
                                                                   ZFunction_Callback Callback,
                                                                   void* callbackArg,
                                                                   encap_type_t encap_type );

//----------------------------------------------------------------------------
// Sensor Alarm Supported Report Command
//
void ICACHE_FLASH_ATTR znet_cc_sensor_alarm_supported_report( const ZFunction func,
                                                              uint8_t node_id,
                                                              int cc_data_len,
                                                              const uint8_t* cc_data );

//----------------------------------------------------------------------------
// Silence Alarm Set Command
//
znet_error_t ICACHE_FLASH_ATTR znet_cc_silence_alarm_set( uint8_t node_id,
                                                          uint8_t channel_id,
                                                          uint8_t mode,
                                                          uint16_t time,
                                                          uint8_t sensors_mask_size,
                                                          const uint8_t* sensors_mask,
                                                          ZFunction_Callback Callback,
                                                          void* callbackArg,
                                                          encap_type_t encap_type );

//////////////////////////////////////////////////////////////////////////////
// Command Class Thermostat Fan State

//----------------------------------------------------------------------------
// Thermostat Fan State Get Command
//
// @param: node_id
// Destination Node Id
//
// @param: channel_id
// Destination channel Id
//
// @param: Callback
// @default: NULL
// Custom function to be called on function failure
// NULL if callback is not needed
//
// @param: callbackArg
// Custom argument to be passed to custom function to be called on function
// success or failure
//
znet_error_t ICACHE_FLASH_ATTR znet_cc_thermostat_fan_state_get( uint8_t node_id,
                                                                 uint8_t channel_id,
                                                                 ZFunction_Callback Callback,
                                                                 void* callbackArg,
                                                                 encap_type_t encap_type );

//----------------------------------------------------------------------------
// Thermostat Fan State Report Command
//
void ICACHE_FLASH_ATTR znet_cc_thermostat_fan_state_report( const ZFunction func,
                                                            uint8_t node_id,
                                                            int cc_data_len,
                                                            const uint8_t* cc_data );

//////////////////////////////////////////////////////////////////////////////

// Command Class Door Lock Logging

//----------------------------------------------------------------------------
// Door Lock Logging Records Supported Get Command
//
// @param: node_id
// Destination Node Id
//
// @param: Callback
// @default: NULL
// Custom function to be called on function failure or success
// NULL if callback is not needed
//
// @param: callbackArg
// Custom argument to be passed to custom function to be called on function
// success or failure
//
znet_error_t ICACHE_FLASH_ATTR znet_cc_door_lock_logging_records_supported_get( uint8_t node_id,
                                                                                ZFunction_Callback Callback,
                                                                                void* callbackArg,
                                                                                encap_type_t encap_type );

//----------------------------------------------------------------------------
// Door Lock Logging Records Supported Report CB
//
void ICACHE_FLASH_ATTR znet_cc_thermostat_fan_state_report( const ZFunction func,
                                                            uint8_t node_id,
                                                            int cc_data_len,
                                                            const uint8_t* cc_data );

//----------------------------------------------------------------------------
// Record Get Command
//
// @param: node_id
// Destination Node Id
//
// @param: record_n
// Record number
//
// @param: Callback
// @default: NULL
// Custom function to be called on function failure or success
// NULL if callback is not needed
//
// @param: callbackArg
// Custom argument to be passed to custom function to be called on function
// success or failure
//
znet_error_t ICACHE_FLASH_ATTR znet_cc_record_get( uint8_t node_id,
                                                   uint8_t record_n,
                                                   ZFunction_Callback Callback,
                                                   void* callbackArg,
                                                   encap_type_t encap_type );

//----------------------------------------------------------------------------
// Door Lock Logging Records Supported Report CB
//
void ICACHE_FLASH_ATTR znet_cc_record_report( const ZFunction func,
                                              uint8_t node_id,
                                              int cc_data_len,
                                              const uint8_t* cc_data );

//////////////////////////////////////////////////////////////////////////////
// Command Class Controller Replication

//----------------------------------------------------------------------------
// Ctrl Replication Transfer Group Command
//
// @param: node_id
// Destination Node Id
//
// @param: seq_n
// Sequence Number
//
// @param: group_id
// Group ID
//
// @param: Callback
// @default: NULL
// Custom function to be called on function failure or success
// NULL if callback is not needed
//
// @param: callbackArg
// Custom argument to be passed to custom function to be called on function
// success or failure
//
znet_error_t ICACHE_FLASH_ATTR znet_cc_ctrl_replication_transfer_group( uint8_t node_id,
                                                                        uint8_t seq_n,
                                                                        uint8_t group_id,
                                                                        ZFunction_Callback Callback,
                                                                        void* callbackArg,
                                                                        encap_type_t encap_type );

//----------------------------------------------------------------------------
// Ctrl Replication Transfer Group Name Command
//
// @param: node_id
// Destination Node Id
//
// @param: seq_n
// Sequence Number
//
// @param: group_id
// Group ID
//
// @param: Callback
// @default: NULL
// Custom function to be called on function failure or success
// NULL if callback is not needed
//
// @param: callbackArg
// Custom argument to be passed to custom function to be called on function
// success or failure
//
znet_error_t ICACHE_FLASH_ATTR znet_cc_ctrl_replication_transfer_group_name( uint8_t node_id,
                                                                             uint8_t seq_n,
                                                                             uint8_t group_id,
                                                                             uint8_t cmd_data_length,
                                                                             const uint8_t* cmd_data,
                                                                             ZFunction_Callback Callback,
                                                                             void* callbackArg,
                                                                             encap_type_t encap_type );

//----------------------------------------------------------------------------
// Ctrl Replication Transfer Scene Command
//
// @param: node_id
// Destination Node Id
//
// @param: seq_n
// Sequence Number
//
// @param: scene_id
// Scene ID
//
// @param: level
// Level
//
// @param: Callback
// @default: NULL
// Custom function to be called on function failure or success
// NULL if callback is not needed
//
// @param: callbackArg
// Custom argument to be passed to custom function to be called on function
// success or failure
//
znet_error_t ICACHE_FLASH_ATTR znet_cc_ctrl_replication_transfer_scene( uint8_t node_id,
                                                                        uint8_t seq_n,
                                                                        uint8_t scene_id,
                                                                        uint8_t level,
                                                                        ZFunction_Callback Callback,
                                                                        void* callbackArg,
                                                                        encap_type_t encap_type );

//----------------------------------------------------------------------------
// Ctrl Replication Transfer Scene Name Command
//
// @param: node_id
// Destination Node Id
//
// @param: seq_n
// Sequence Number
//
// @param: scene_id
// Scene ID
//
// @param: Callback
// @default: NULL
// Custom function to be called on function failure or success
// NULL if callback is not needed
//
// @param: callbackArg
// Custom argument to be passed to custom function to be called on function
// success or failure
//
znet_error_t ICACHE_FLASH_ATTR znet_cc_ctrl_replication_transfer_scene_name( uint8_t node_id,
                                                                             uint8_t seq_n,
                                                                             uint8_t scene_id,
                                                                             uint8_t cmd_data_length,
                                                                             const uint8_t* cmd_data,
                                                                             ZFunction_Callback Callback,
                                                                             void* callbackArg,
                                                                             encap_type_t encap_type );

//////////////////////////////////////////////////////////////////////////////

// Command Class Controller Replication

//----------------------------------------------------------------------------
// Schedule Entry Lock Enable All Set Command
//
// @param: node_id
// Destination Node Id
//
// @param: enabled
// Enabled
//
// @param: Callback
// @default: NULL
// Custom function to be called on function failure or success
// NULL if callback is not needed
//
// @param: callbackArg
// Custom argument to be passed to custom function to be called on function
// success or failure
//
znet_error_t ICACHE_FLASH_ATTR znet_cc_schedule_entry_lock_enable_all_set( uint8_t node_id,
                                                                           uint8_t channel_id,
                                                                           bool enabled,
                                                                           ZFunction_Callback Callback,
                                                                           void* callbackArg,
                                                                           encap_type_t encap_type );

//----------------------------------------------------------------------------
// Schedule Entry Lock Enable Set Command
//
// @param: node_id
// Destination Node Id
//
// @param: user_id
// User Identifier
//
// @param: enabled
// Enabled
//
// @param: Callback
// @default: NULL
// Custom function to be called on function failure or success
// NULL if callback is not needed
//
// @param: callbackArg
// Custom argument to be passed to custom function to be called on function
// success or failure
//
znet_error_t ICACHE_FLASH_ATTR znet_cc_schedule_entry_lock_enable_set( uint8_t node_id,
                                                                       uint8_t channel_id,
                                                                       uint8_t user_id,
                                                                       bool enabled,
                                                                       ZFunction_Callback Callback,
                                                                       void* callbackArg,
                                                                       encap_type_t encap_type );

//----------------------------------------------------------------------------
// Schedule Entry Lock Time Offset Get Command
//
// @param: node_id
// Destination Node Id
//
// @param: Callback
// @default: NULL
// Custom function to be called on function failure or success
// NULL if callback is not needed
//
// @param: callbackArg
// Custom argument to be passed to custom function to be called on function
// success or failure
//
znet_error_t ICACHE_FLASH_ATTR znet_cc_schedule_entry_lock_time_offset_get( uint8_t node_id,
                                                                            uint8_t channel_id,
                                                                            ZFunction_Callback Callback,
                                                                            void* callbackArg,
                                                                            encap_type_t encap_type );

//----------------------------------------------------------------------------
// Schedule Entry Lock Time Offset Set Command
//
// @param: node_id
// Destination Node Id
//
// @param: hour_tzo
// Hour TZO
//
// @param: sign_tzo
// Sign TZO (1 bits)
//
// @param: min_tzo
// Minute TZO
//
// @param: min_offset_dst
// Minute Offset DST (7 bits)
//
// @param: sign_offset_dst
// Sign Offset DST (1 bits)
//
// @param: Callback
// @default: NULL
// Custom function to be called on function failure or success
// NULL if callback is not needed
//
// @param: callbackArg
// Custom argument to be passed to custom function to be called on function
// success or failure
//
znet_error_t ICACHE_FLASH_ATTR znet_cc_schedule_entry_lock_time_offset_set( uint8_t node_id,
                                                                            uint8_t channel_id,
                                                                            bool is_tzo_negative,
                                                                            uint8_t hour_tzo,
                                                                            uint8_t min_tzo,
                                                                            int8_t min_offset_dst,
                                                                            ZFunction_Callback Callback,
                                                                            void* callbackArg,
                                                                            encap_type_t encap_type );

//----------------------------------------------------------------------------
// Schedule Entry Lock Time Offset Report CB
//
void ICACHE_FLASH_ATTR znet_cc_schedule_entry_lock_time_offset_report( const ZFunction func,
                                                                       uint8_t node_id,
                                                                       int cc_data_len,
                                                                       const uint8_t* cc_data );

//----------------------------------------------------------------------------
// Schedule Entry Lock Week Day Get Command
//
// @param: node_id
// Destination Node Id
//
// @param: user_id
// User Identifier
//
// @param: slot_id
// Schedule Slot ID
//
// @param: Callback
// @default: NULL
// Custom function to be called on function failure or success
// NULL if callback is not needed
//
// @param: callbackArg
// Custom argument to be passed to custom function to be called on function
// success or failure
//
znet_error_t ICACHE_FLASH_ATTR znet_cc_schedule_entry_lock_week_day_get( uint8_t node_id,
                                                                         uint8_t channel_id,
                                                                         uint8_t user_id,
                                                                         uint8_t slot_id,
                                                                         ZFunction_Callback Callback,
                                                                         void* callbackArg,
                                                                         encap_type_t encap_type );

//----------------------------------------------------------------------------
// Schedule Entry Lock Week Day Set Command
//
// @param: node_id
// Destination Node Id
//
// @param: action
// Set Action
//
// @param: user_id
// User Identifier
//
// @param: slot_id
// Schedule Slot ID
//
// @param: day_of_week
// Day of Week
//
// @param: start_hour
// Start Hour
//
// @param: start_min
// Start Minute
//
// @param: stop_hour
// Stop Hour
//
// @param: stop_min
// Stop Minute
//
// @param: Callback
// @default: NULL
// Custom function to be called on function failure or success
// NULL if callback is not needed
//
// @param: callbackArg
// Custom argument to be passed to custom function to be called on function
// success or failure
//
znet_error_t ICACHE_FLASH_ATTR znet_cc_schedule_entry_lock_week_day_set( uint8_t node_id,
                                                                         uint8_t channel_id,
                                                                         uint8_t action,
                                                                         uint8_t user_id,
                                                                         uint8_t slot_id,
                                                                         uint8_t day_of_week,
                                                                         uint8_t start_hour,
                                                                         uint8_t start_min,
                                                                         uint8_t stop_hour,
                                                                         uint8_t stop_min,
                                                                         ZFunction_Callback Callback,
                                                                         void* callbackArg,
                                                                         encap_type_t encap_type );

//----------------------------------------------------------------------------
// Schedule Entry Lock Week Day Report CB
//
void ICACHE_FLASH_ATTR znet_cc_schedule_entry_lock_week_day_report( const ZFunction func,
                                                                    uint8_t node_id,
                                                                    int cc_data_len,
                                                                    const uint8_t* cc_data );

//----------------------------------------------------------------------------
// Schedule Entry Lock Year Day Get Command
//
// @param: node_id
// Destination Node Id
//
// @param: user_id
// User Identifier
//
// @param: slot_id
// Schedule Slot ID
//
// @param: Callback
// @default: NULL
// Custom function to be called on function failure or success
// NULL if callback is not needed
//
// @param: callbackArg
// Custom argument to be passed to custom function to be called on function
// success or failure
//
znet_error_t ICACHE_FLASH_ATTR znet_cc_schedule_entry_lock_year_day_get( uint8_t node_id,
                                                                         uint8_t channel_id,
                                                                         uint8_t user_id,
                                                                         uint8_t slot_id,
                                                                         ZFunction_Callback Callback,
                                                                         void* callbackArg,
                                                                         encap_type_t encap_type );

//----------------------------------------------------------------------------
// Schedule Entry Lock Year Day Set Command
//
// @param: node_id
// Destination Node Id
//
// @param: action
// Set Action
//
// @param: user_id
// User Identifier
//
// @param: slot_id
// Schedule Slot ID
//
// @param: start_year
// Start Year
//
// @param: start_month
// Start Month
//
// @param: start_day
// Start Day
//
// @param: start_hour
// Start Hour
//
// @param: start_min
// Start Minute
//
// @param: stop_year
// Start Year
//
// @param: stop_month
// Start Month
//
// @param: stop_day
// Start Day
//
// @param: stop_hour
// Stop Hour
//
// @param: stop_min
// Stop Minute
//
// @param: Callback
// @default: NULL
// Custom function to be called on function failure or success
// NULL if callback is not needed
//
// @param: callbackArg
// Custom argument to be passed to custom function to be called on function
// success or failure
//
znet_error_t ICACHE_FLASH_ATTR znet_cc_schedule_entry_lock_year_day_set( uint8_t node_id,
                                                                         uint8_t channel_id,
                                                                         uint8_t action,
                                                                         uint8_t user_id,
                                                                         uint8_t slot_id,
                                                                         uint8_t start_year,
                                                                         uint8_t start_month,
                                                                         uint8_t start_day,
                                                                         uint8_t start_hour,
                                                                         uint8_t start_min,
                                                                         uint8_t stop_year,
                                                                         uint8_t stop_month,
                                                                         uint8_t stop_day,
                                                                         uint8_t stop_hour,
                                                                         uint8_t stop_min,
                                                                         ZFunction_Callback Callback,
                                                                         void* callbackArg,
                                                                         encap_type_t encap_type );

//----------------------------------------------------------------------------
// Schedule Entry Lock Year Day Report CB
//
void ICACHE_FLASH_ATTR znet_cc_schedule_entry_lock_year_day_report( const ZFunction func,
                                                                    uint8_t node_id,
                                                                    int cc_data_len,
                                                                    const uint8_t* cc_data );

//----------------------------------------------------------------------------
// Schedule Entry Type Supported Get Command
//
// @param: node_id
// Destination Node Id
//
// @param: Callback
// @default: NULL
// Custom function to be called on function failure or success
// NULL if callback is not needed
//
// @param: callbackArg
// Custom argument to be passed to custom function to be called on function
// success or failure
//
znet_error_t ICACHE_FLASH_ATTR znet_cc_schedule_entry_type_supported_get( uint8_t node_id,
                                                                          uint8_t channel_id,
                                                                          ZFunction_Callback Callback,
                                                                          void* callbackArg,
                                                                          encap_type_t encap_type );

//----------------------------------------------------------------------------
// Schedule Entry Type Supported  Report CB
//
void ICACHE_FLASH_ATTR znet_cc_schedule_entry_type_supported_report( const ZFunction func,
                                                                     uint8_t node_id,
                                                                     int cc_data_len,
                                                                     const uint8_t* cc_data );

//----------------------------------------------------------------------------
// Schedule Entry Lock Daily Repeating Get Command
//
// @param: node_id
// Destination Node Id
//
// @param: user_id
// User Identifier
//
// @param: slot_id
// Schedule Slot ID
//
// @param: Callback
// @default: NULL
// Custom function to be called on function failure or success
// NULL if callback is not needed
//
// @param: callbackArg
// Custom argument to be passed to custom function to be called on function
// success or failure
//
znet_error_t ICACHE_FLASH_ATTR znet_cc_schedule_entry_lock_daily_repeating_get( uint8_t node_id,
                                                                                uint8_t channel_id,
                                                                                uint8_t user_id,
                                                                                uint8_t slot_id,
                                                                                ZFunction_Callback Callback,
                                                                                void* callbackArg,
                                                                                encap_type_t encap_type );

//----------------------------------------------------------------------------
// Schedule Entry Lock Daily Repeating Set Command
//
// @param: node_id
// Destination Node Id
//
// @param: action
// Set Action
//
// @param: user_id
// User Identifier
//
// @param: slot_id
// Schedule Slot ID
//
// @param: week_day
// Week Day Bitmask
//
// @param: start_hour
// Start Hour
//
// @param: start_min
// Start Minute
//
// @param: duration_hour
// Duration Hour
//
// @param: duration_min
// Duration Minute
//
// @param: Callback
// @default: NULL
// Custom function to be called on function failure or success
// NULL if callback is not needed
//
// @param: callbackArg
// Custom argument to be passed to custom function to be called on function
// success or failure
//
znet_error_t ICACHE_FLASH_ATTR znet_cc_schedule_entry_lock_daily_repeating_set( uint8_t node_id,
                                                                                uint8_t channel_id,
                                                                                uint8_t action,
                                                                                uint8_t user_id,
                                                                                uint8_t slot_id,
                                                                                uint8_t week_day,
                                                                                uint8_t start_hour,
                                                                                uint8_t start_min,
                                                                                uint8_t duration_hour,
                                                                                uint8_t duration_min,
                                                                                ZFunction_Callback Callback,
                                                                                void* callbackArg,
                                                                                encap_type_t encap_type );

//----------------------------------------------------------------------------
// Schedule Entry Lock Daily Repeating Report CB
//
void ICACHE_FLASH_ATTR znet_cc_schedule_entry_lock_daily_repeating_report( const ZFunction func,
                                                                           uint8_t node_id,
                                                                           int cc_data_len,
                                                                           const uint8_t* cc_data );

//////////////////////////////////////////////////////////////////////////////
// Command Class Central Scene

//----------------------------------------------------------------------------
// Central Scene Supported Get Command
//
// @param: node_id
// Destination Node Id
//
// @param: channel_id
// Destination channel Id
//
// @param: Callback
// @default: NULL
// Custom function to be called on function failure or success
// NULL if callback is not needed
//
// @param: callbackArg
// Custom argument to be passed to custom function to be called on function
// success or failure
//
znet_error_t ICACHE_FLASH_ATTR znet_cc_central_scene_supported_get( uint8_t node_id,
                                                                    uint8_t channel_id,
                                                                    ZFunction_Callback Callback,
                                                                    void* callbackArg,
                                                                    encap_type_t encap_type );

//----------------------------------------------------------------------------
// Central Scene Supported Report
void ICACHE_FLASH_ATTR znet_cc_central_scene_supported_report( const ZFunction func,
                                                               uint8_t node_id,
                                                               int cc_data_len,
                                                               const uint8_t* cc_data );

//----------------------------------------------------------------------------
// Central Scene Notification Command/Report
//
void ICACHE_FLASH_ATTR znet_cc_central_scene_notification( const ZFunction func,
                                                           uint8_t node_id,
                                                           int cc_data_len,
                                                           const uint8_t* cc_data );
//----------------------------------------------------------------------------
// Central Scene Configuration Set Command
//
// @param: node_id
// Destination Node Id
//
// @param: channel_id
// Destination channel Id
//
// @param: refresh
// Slow Refresh (1 bits)
//
// @param: Callback
// @default: NULL
// Custom function to be called on function failure or success
// NULL if callback is not needed
//
// @param: callbackArg
// Custom argument to be passed to custom function to be called on function
// success or failure
//
znet_error_t ICACHE_FLASH_ATTR znet_cc_central_scene_configuration_set( uint8_t node_id,
                                                                        uint8_t channel_id,
                                                                        uint8_t refresh,
                                                                        ZFunction_Callback Callback,
                                                                        void* callbackArg,
                                                                        encap_type_t encap_type );

//----------------------------------------------------------------------------
// Central Scene Configuration Get Command
//
// @param: node_id
// Destination Node Id
//
// @param: channel_id
// Destination channel Id
//
// @param: Callback
// @default: NULL
// Custom function to be called on function failure or success
// NULL if callback is not needed
//
// @param: callbackArg
// Custom argument to be passed to custom function to be called on function
// success or failure
//
znet_error_t ICACHE_FLASH_ATTR znet_cc_central_scene_configuration_get( uint8_t node_id,
                                                                        uint8_t channel_id,
                                                                        ZFunction_Callback Callback,
                                                                        void* callbackArg,
                                                                        encap_type_t encap_type );

//----------------------------------------------------------------------------
// Central Scene Configuration Report    TODO!!! Check support in the znet_lib_cc.c file
void ICACHE_FLASH_ATTR znet_cc_central_scene_configuration_report( const ZFunction func,
                                                                   uint8_t node_id,
                                                                   int cc_data_len,
                                                                   const uint8_t* cc_data );

//////////////////////////////////////////////////////////////////////////////
// Command Class Thermostat Operating State

//----------------------------------------------------------------------------
// Thermostat Operating State Get Command
//
// @param: node_id
// Destination Node Id
//
// @param: channel_id
// Destination channel Id
//
// @param: Callback
// @default: NULL
// Custom function to be called on function failure
// NULL if callback is not needed
//
// @param: callbackArg
// Custom argument to be passed to custom function to be called on function
// success or failure
//
znet_error_t ICACHE_FLASH_ATTR znet_cc_thermostat_operating_state_get( uint8_t node_id,
                                                                       uint8_t channel_id,
                                                                       ZFunction_Callback Callback,
                                                                       void* callbackArg,
                                                                       encap_type_t encap_type );

//----------------------------------------------------------------------------
// Thermostat Operating State Report Command
//
void ICACHE_FLASH_ATTR znet_cc_thermostat_operating_state_report( const ZFunction func,
                                                                  uint8_t node_id,
                                                                  int cc_data_len,
                                                                  const uint8_t* cc_data );

//----------------------------------------------------------------------------
// Thermostat Operating State Logging Supported Get Command
//
// @param: node_id
// Destination Node Id
//
// @param: channel_id
// Destination channel Id
//
// @param: Callback
// @default: NULL
// Custom function to be called on function failure
// NULL if callback is not needed
//
// @param: callbackArg
// Custom argument to be passed to custom function to be called on function
// success or failure
//
znet_error_t ICACHE_FLASH_ATTR znet_cc_thermostat_operating_state_logging_supported_get( uint8_t node_id,
                                                                                         uint8_t channel_id,
                                                                                         ZFunction_Callback Callback,
                                                                                         void* callbackArg,
                                                                                         encap_type_t encap_type );

//----------------------------------------------------------------------------
// Thermostat Operating State Logging Supported Report Command
//
void ICACHE_FLASH_ATTR znet_cc_thermostat_operating_state_logging_supported_report( const ZFunction func,
                                                                                    uint8_t node_id,
                                                                                    int cc_data_len,
                                                                                    const uint8_t* cc_data );

//----------------------------------------------------------------------------
// Thermostat Operating State Logging Get Command
//
// @param: node_id
// Destination Node Id
//
// @param: channel_id
// Destination channel Id
//
// @param: Callback
// @default: NULL
// Custom function to be called on function failure
// NULL if callback is not needed
//
// @param: callbackArg
// Custom argument to be passed to custom function to be called on function
// success or failure
//
znet_error_t ICACHE_FLASH_ATTR znet_cc_thermostat_operating_state_logging_get( uint8_t node_id,
                                                                               uint8_t channel_id,
                                                                               uint8_t cmd_data_length,
                                                                               const uint8_t* cmd_data,
                                                                               ZFunction_Callback Callback,
                                                                               void* callbackArg,
                                                                               encap_type_t encap_type );

//----------------------------------------------------------------------------
// Thermostat Operating State Logging Report Command
//
void ICACHE_FLASH_ATTR znet_cc_thermostat_operating_state_logging_report( const ZFunction func,
                                                                          uint8_t node_id,
                                                                          int cc_data_len,
                                                                          const uint8_t* cc_data );

//////////////////////////////////////////////////////////////////////////////
// Command Class Schedule

//----------------------------------------------------------------------------
// Schedule Support Get Command
//
// @param: node_id
// Destination Node Id
//
// @param: block_id_available
//
// @param: block_id
// Schedule ID Block
//
// @param: Callback
// @default: NULL
// Custom function to be called on function failure or success
// NULL if callback is not needed
//
// @param: callbackArg
// Custom argument to be passed to custom function to be called on function
// success or failure
//
znet_error_t ICACHE_FLASH_ATTR znet_cc_shedule_support_get( uint8_t node_id,
                                                            bool block_id_available,
                                                            uint8_t block_id,
                                                            ZFunction_Callback Callback,
                                                            void* callbackArg,
                                                            encap_type_t encap_type );

//----------------------------------------------------------------------------
// Schedule Set Command
//
// @param: node_id
// Destination Node Id
//
// @param: cmd_data_length
//
// @param: cmd_data
//
// @param: Callback
// @default: NULL
// Custom function to be called on function failure or success
// NULL if callback is not needed
//
// @param: callbackArg
// Custom argument to be passed to custom function to be called on function
// success or failure
//
znet_error_t ICACHE_FLASH_ATTR znet_cc_shedule_set( uint8_t node_id,
                                                    uint8_t cmd_data_length,
                                                    const uint8_t* cmd_data,
                                                    ZFunction_Callback Callback,
                                                    void* callbackArg,
                                                    encap_type_t encap_type );

//----------------------------------------------------------------------------
// Schedule Get Command
//
// @param: node_id
// Destination Node Id
//
// @param: schedule_id
// Schedule ID
//
// @param: block_id_available
//
// @param: block_id
// Schedule ID Block
//
// @param: aid_ro_ctl_available
//
// @param: aid_ro_ctl
// AID_RO_CTL (1 bits)
//
// @param: Callback
// @default: NULL
// Custom function to be called on function failure or success
// NULL if callback is not needed
//
// @param: callbackArg
// Custom argument to be passed to custom function to be called on function
// success or failure
//
znet_error_t ICACHE_FLASH_ATTR znet_cc_shedule_get( uint8_t node_id,
                                                    uint8_t schedule_id,
                                                    bool block_id_available,
                                                    uint8_t block_id,
                                                    bool aid_ro_ctl_available,
                                                    bool aid_ro_ctl,
                                                    ZFunction_Callback Callback,
                                                    void* callbackArg,
                                                    encap_type_t encap_type );

//----------------------------------------------------------------------------
// Schedule Remove Command
//
// @param: node_id
// Destination Node Id
//
// @param: schedule_id
// Schedule ID
//
// @param: block_id_available
//
// @param: block_id
// Schedule ID Block
//
// @param: Callback
// @default: NULL
// Custom function to be called on function failure or success
// NULL if callback is not needed
//
// @param: callbackArg
// Custom argument to be passed to custom function to be called on function
// success or failure
//
znet_error_t ICACHE_FLASH_ATTR znet_cc_shedule_remove( uint8_t node_id,
                                                       uint8_t schedule_id,
                                                       bool block_id_available,
                                                       uint8_t block_id,
                                                       ZFunction_Callback Callback,
                                                       void* callbackArg,
                                                       encap_type_t encap_type );

//----------------------------------------------------------------------------
// Schedule State Set Command
//
// @param: node_id
// Destination Node Id
//
// @param: schedule_id
// Schedule ID
//
// @param: state
// Schedule State
//
// @param: block_id_available
//
// @param: block_id
// Schedule ID Block
//
// @param: Callback
// @default: NULL
// Custom function to be called on function failure or success
// NULL if callback is not needed
//
// @param: callbackArg
// Custom argument to be passed to custom function to be called on function
// success or failure
//
znet_error_t ICACHE_FLASH_ATTR znet_cc_schedule_state_set( uint8_t node_id,
                                                           uint8_t schedule_id,
                                                           uint8_t state,
                                                           bool block_id_available,
                                                           uint8_t block_id,
                                                           ZFunction_Callback Callback,
                                                           void* callbackArg,
                                                           encap_type_t encap_type );

//----------------------------------------------------------------------------
// Schedule State Get Command
//
// @param: node_id
// Destination Node Id
//
// @param: block_id_available
//
// @param: block_id
// Schedule ID Block
//
// @param: Callback
// @default: NULL
// Custom function to be called on function failure or success
// NULL if callback is not needed
//
// @param: callbackArg
// Custom argument to be passed to custom function to be called on function
// success or failure
//
znet_error_t ICACHE_FLASH_ATTR znet_cc_schedule_state_get( uint8_t node_id,
                                                           bool block_id_available,
                                                           uint8_t block_id,
                                                           ZFunction_Callback Callback,
                                                           void* callbackArg,
                                                           encap_type_t encap_type );

//////////////////////////////////////////////////////////////////////////////
// Command Class Thermostat Fan Mode

//----------------------------------------------------------------------------
// Thermostat Fan Mode Get Command
//
// @param: node_id
// Destination Node Id
//
// @param: channel_id
// Destination channel Id
//
// @param: Callback
// @default: NULL
// Custom function to be called on function failure
// NULL if callback is not needed
//
// @param: callbackArg
// Custom argument to be passed to custom function to be called on function
// success or failure
//
znet_error_t ICACHE_FLASH_ATTR znet_cc_thermostat_fan_mode_get( uint8_t node_id,
                                                                uint8_t channel_id,
                                                                ZFunction_Callback Callback,
                                                                void* callbackArg,
                                                                encap_type_t encap_type );

//----------------------------------------------------------------------------
// Thermostat Fan Mode Set Command
//
// @param: node_id
// Destination Node Id
//
// @param: channel_id
// Destination channel Id
//
// @param: fan_mode
// Fan Mode (4 bits)
//
// @param: off_available
// Destination Node Id
//
// @param: off
// Off (1 bits)
//
// @param: Callback
// @default: NULL
// Custom function to be called on function failure
// NULL if callback is not needed
//
// @param: callbackArg
// Custom argument to be passed to custom function to be called on function
// success or failure
//
znet_error_t ICACHE_FLASH_ATTR znet_cc_thermostat_fan_mode_set( uint8_t node_id,
                                                                uint8_t channel_id,
                                                                uint8_t fan_mode,
                                                                bool off_available,
                                                                bool off,
                                                                ZFunction_Callback Callback,
                                                                void* callbackArg,
                                                                encap_type_t encap_type );

//----------------------------------------------------------------------------
// Thermostat Fan Mode Report Command
//
void ICACHE_FLASH_ATTR znet_cc_thermostat_fan_mode_report( const ZFunction func,
                                                           uint8_t node_id,
                                                           int cc_data_len,
                                                           const uint8_t* cc_data );

//----------------------------------------------------------------------------
// Thermostat Fan Mode Supported Get Command
//
// @param: node_id
// Destination Node Id
//
// @param: channel_id
// Destination channel Id
//
// @param: Callback
// @default: NULL
// Custom function to be called on function failure
// NULL if callback is not needed
//
// @param: callbackArg
// Custom argument to be passed to custom function to be called on function
// success or failure
//
znet_error_t ICACHE_FLASH_ATTR znet_cc_thermostat_fan_mode_supported_get( uint8_t node_id,
                                                                          uint8_t channel_id,
                                                                          ZFunction_Callback Callback,
                                                                          void* callbackArg,
                                                                          encap_type_t encap_type );

//----------------------------------------------------------------------------
// Thermostat Fan Mode Supported Report Command
//
void ICACHE_FLASH_ATTR znet_cc_thermostat_fan_mode_supported_report( const ZFunction func,
                                                                     uint8_t node_id,
                                                                     int cc_data_len,
                                                                     const uint8_t* cc_data );

//////////////////////////////////////////////////////////////////////////////
// Command Class Clock

//----------------------------------------------------------------------------
// Clock Get Command
//
// @param: node_id
// Destination Node Id
//
// @param: Callback
// @default: NULL
// Custom function to be called on function failure
// NULL if callback is not needed
//
// @param: callbackArg
// Custom argument to be passed to custom function to be called on function
// success or failure
//
znet_error_t ICACHE_FLASH_ATTR znet_cc_clock_get( uint8_t node_id,
                                                  uint8_t channel_id,
                                                  ZFunction_Callback Callback,
                                                  void* callbackArg,
                                                  encap_type_t encap_type );

//----------------------------------------------------------------------------
// Clock Set Command
//
// @param: node_id
// Destination Node Id
//
// @param: hour
// Hour (5 bits)
//
// @param: weekday
// Weekday (3 bits)
//
// @param: minute
// Minute
//
// @param: Callback
// @default: NULL
// Custom function to be called on function failure or success
// NULL if callback is not needed
//
// @param: callbackArg
// Custom argument to be passed to custom function to be called on function
// success or failure
//
znet_error_t ICACHE_FLASH_ATTR znet_cc_clock_set( uint8_t node_id,
                                                  uint8_t channel_id,
                                                  uint8_t hour,
                                                  uint8_t minute,
                                                  uint8_t weekday,
                                                  ZFunction_Callback Callback,
                                                  void* callbackArg,
                                                  encap_type_t encap_type );

//----------------------------------------------------------------------------
// Clock Report Command
//
void ICACHE_FLASH_ATTR znet_cc_clock_report( const ZFunction func,
                                             uint8_t node_id,
                                             int cc_data_len,
                                             const uint8_t* cc_data );

//////////////////////////////////////////////////////////////////////////////
// Command Class User Code

//----------------------------------------------------------------------------
// User Code Get Command
//
// @param: node_id
// Destination Node Id
//
// @param: channel_id
// Destination channel Id
//
// @param: user_id
// User Identifier
//
// @param: Callback
// @default: NULL
// Custom function to be called on function failure or success
// NULL if callback is not needed
//
// @param: callbackArg
// Custom argument to be passed to custom function to be called on function
// success or failure
//
// @param: encap_type
// Defines which Transport Service use to send commands
//
znet_error_t ICACHE_FLASH_ATTR znet_cc_user_code_get( uint8_t node_id,
                                                      uint8_t channel_id,
                                                      uint8_t user_id,
                                                      ZFunction_Callback Callback,
                                                      void* callbackArg,
                                                      encap_type_t encap_type );

//----------------------------------------------------------------------------
// User Code Set Command
//
// @param: node_id
// Destination Node Id
//
// @param: channel_id
// Destination channel Id
//
// @param: user_id
// User Identifier
//
// @param: user_id_status
// User ID Status
//
// @param: user_code_size
// Size user_code
//
// @param: user_code_size
// user_code
//
// @param: Callback
// @default: NULL
// Custom function to be called on function failure or success
// NULL if callback is not needed
//
// @param: callbackArg
// Custom argument to be passed to custom function to be called on function
// success or failure
//
// @param: encap_type
// Defines which Transport Service use to send commands
//
znet_error_t ICACHE_FLASH_ATTR znet_cc_user_code_set( uint8_t node_id,
                                                      uint8_t channel_id,
                                                      uint8_t user_id,
                                                      uint8_t user_id_status,
                                                      uint8_t user_code_size,
                                                      const uint8_t* user_code,
                                                      ZFunction_Callback Callback,
                                                      void* callbackArg,
                                                      encap_type_t encap_type );

//----------------------------------------------------------------------------
// User Code Report CB
//
void ICACHE_FLASH_ATTR znet_cc_user_code_report( const ZFunction func,
                                                 uint8_t node_id,
                                                 int cc_data_len,
                                                 const uint8_t* cc_data );

//----------------------------------------------------------------------------
// Users Number Get Command
//
// @param: node_id
// Destination Node Id
//
// @param: channel_id
// Destination channel Id
//
// @param: Callback
// @default: NULL
// Custom function to be called on function failure or success
// NULL if callback is not needed
//
// @param: callbackArg
// Custom argument to be passed to custom function to be called on function
// success or failure
//
// @param: encap_type
// Defines which Transport Service use to send commands
//
znet_error_t ICACHE_FLASH_ATTR znet_cc_users_number_get( uint8_t node_id,
                                                         uint8_t channel_id,
                                                         ZFunction_Callback Callback,
                                                         void* callbackArg,
                                                         encap_type_t encap_type );

//----------------------------------------------------------------------------
// Users Number Report CB
//
void ICACHE_FLASH_ATTR znet_cc_users_number_report( const ZFunction func,
                                                    uint8_t node_id,
                                                    int cc_data_len,
                                                    const uint8_t* cc_data );

//----------------------------------------------------------------------------
// User Code capabilities (V2)
//
znet_error_t ICACHE_FLASH_ATTR znet_cc_user_code_capabilities_get( uint8_t node_id,
                                                                   uint8_t channel_id,
                                                                   ZFunction_Callback Callback,
                                                                   void* callbackArg,
                                                                   encap_type_t encap_type );

void ICACHE_FLASH_ATTR znet_cc_user_code_capabilities_report( const ZFunction func,
                                                              uint8_t node_id,
                                                              int cc_data_len,
                                                              const uint8_t* cc_data );

//----------------------------------------------------------------------------
// User Code Keypad Mode (V2)
//
znet_error_t ICACHE_FLASH_ATTR znet_cc_user_code_keypad_mode_set( uint8_t node_id,
                                                                  uint8_t channel_id,
                                                                  uint8_t keypad_mode,
                                                                  ZFunction_Callback Callback,
                                                                  void* callbackArg,
                                                                  encap_type_t encap_type );

znet_error_t ICACHE_FLASH_ATTR znet_cc_user_code_keypad_mode_get( uint8_t node_id,
                                                                  uint8_t channel_id,
                                                                  ZFunction_Callback Callback,
                                                                  void* callbackArg,
                                                                  encap_type_t encap_type );

void ICACHE_FLASH_ATTR znet_cc_user_code_keypad_mode_report( const ZFunction func,
                                                             uint8_t node_id,
                                                             int cc_data_len,
                                                             const uint8_t* cc_data );

//----------------------------------------------------------------------------
// Extended User Code (V2)
//
znet_error_t ICACHE_FLASH_ATTR znet_cc_extended_user_code_get( uint8_t node_id,
                                                               uint8_t channel_id,
                                                               uint16_t user_id,
                                                               bool should_report_more,
                                                               ZFunction_Callback Callback,
                                                               void* callbackArg,
                                                               encap_type_t encap_type );

znet_error_t ICACHE_FLASH_ATTR znet_cc_extended_user_code_set( uint8_t node_id,
                                                               uint8_t channel_id,
                                                               uint8_t user_codes_count,
                                                               uint8_t data_size,
                                                               const uint8_t* data,
                                                               ZFunction_Callback Callback,
                                                               void* callbackArg,
                                                               encap_type_t encap_type );

void ICACHE_FLASH_ATTR znet_cc_extended_user_code_report( const ZFunction func,
                                                          uint8_t node_id,
                                                          int cc_data_len,
                                                          const uint8_t* cc_data );

//----------------------------------------------------------------------------
// Master Code (V2)
//

znet_error_t ICACHE_FLASH_ATTR znet_cc_master_code_set( uint8_t node_id,
                                                        uint8_t channel_id,
                                                        uint8_t master_code_length,
                                                        const uint8_t* master_code,
                                                        ZFunction_Callback Callback,
                                                        void* callbackArg,
                                                        encap_type_t encap_type );

znet_error_t ICACHE_FLASH_ATTR znet_cc_master_code_get( uint8_t node_id,
                                                        uint8_t channel_id,
                                                        ZFunction_Callback Callback,
                                                        void* callbackArg,
                                                        encap_type_t encap_type );

void ICACHE_FLASH_ATTR znet_cc_master_code_report( const ZFunction func,
                                                   uint8_t node_id,
                                                   int cc_data_len,
                                                   const uint8_t* cc_data );

//----------------------------------------------------------------------------
// User Code Checksum (V2)
//

znet_error_t ICACHE_FLASH_ATTR znet_cc_user_code_checksum_get( uint8_t node_id,
                                                               uint8_t channel_id,
                                                               ZFunction_Callback Callback,
                                                               void* callbackArg,
                                                               encap_type_t encap_type );

void ICACHE_FLASH_ATTR znet_cc_user_code_checksum_report( const ZFunction func,
                                                          uint8_t node_id,
                                                          int cc_data_len,
                                                          const uint8_t* cc_data );

//////////////////////////////////////////////////////////////////////////////
// Command Class Thermostat Mode

//----------------------------------------------------------------------------
// Thermostat Mode Get Command
//
// @param: node_id
// Destination Node Id
//
// @param: channel_id
// Destination channel Id
//
// @param: Callback
// @default: NULL
// Custom function to be called on function failure
// NULL if callback is not needed
//
// @param: callbackArg
// Custom argument to be passed to custom function to be called on function
// success or failure
//
znet_error_t ICACHE_FLASH_ATTR znet_cc_thermostat_mode_get( uint8_t node_id,
                                                            uint8_t channel_id,
                                                            ZFunction_Callback Callback,
                                                            void* callbackArg,
                                                            encap_type_t encap_type );

//----------------------------------------------------------------------------
// Thermostat Mode Set Command
//
// @param: node_id
// Destination Node Id
//
// @param: channel_id
// Destination channel Id
//
// @param: mode
// Mode (5 bits)
//
// @param: next_data_available
//
// @param: data_size
// No of Manufacturer Data fields (3 bits)
//
// @param: man_data
// Manufacturer Data
//
// @param: Callback
// @default: NULL
// Custom function to be called on function failure
// NULL if callback is not needed
//
// @param: callbackArg
// Custom argument to be passed to custom function to be called on function
// success or failure
//
znet_error_t ICACHE_FLASH_ATTR znet_cc_thermostat_mode_set( uint8_t node_id,
                                                            uint8_t channel_id,
                                                            uint8_t mode,
                                                            bool next_data_available,
                                                            uint8_t data_size,
                                                            const uint8_t* man_data,
                                                            ZFunction_Callback Callback,
                                                            void* callbackArg,
                                                            encap_type_t encap_type );

//----------------------------------------------------------------------------
// Thermostat Mode Report Command
//
void ICACHE_FLASH_ATTR znet_cc_thermostat_mode_report( const ZFunction func,
                                                       uint8_t node_id,
                                                       int cc_data_len,
                                                       const uint8_t* cc_data );

//----------------------------------------------------------------------------
// Thermostat Mode Supported Get Command
//
// @param: node_id
// Destination Node Id
//
// @param: channel_id
// Destination channel Id
//
// @param: Callback
// @default: NULL
// Custom function to be called on function failure
// NULL if callback is not needed
//
// @param: callbackArg
// Custom argument to be passed to custom function to be called on function
// success or failure
//
znet_error_t ICACHE_FLASH_ATTR znet_cc_thermostat_mode_supported_get( uint8_t node_id,
                                                                      uint8_t channel_id,
                                                                      ZFunction_Callback Callback,
                                                                      void* callbackArg,
                                                                      encap_type_t encap_type );

//----------------------------------------------------------------------------
// Thermostat Mode Supported Report Command
//
void ICACHE_FLASH_ATTR znet_cc_thermostat_mode_supported_report( const ZFunction func,
                                                                 uint8_t node_id,
                                                                 int cc_data_len,
                                                                 const uint8_t* cc_data );

//////////////////////////////////////////////////////////////////////////////

// Command Class Door Lock

//----------------------------------------------------------------------------
// Door Lock Configuration Get Command
//
// @param: node_id
// Destination Node Id
//
// @param: channel_id
// Destination channel Id
//
// @param: Callback
// @default: NULL
// Custom function to be called on function failure
// NULL if callback is not needed
//
// @param: callbackArg
// Custom argument to be passed to custom function to be called on function
// success or failure
//
znet_error_t ICACHE_FLASH_ATTR znet_cc_door_lock_configuration_get( uint8_t node_id,
                                                                    uint8_t channel_id,
                                                                    ZFunction_Callback Callback,
                                                                    void* callbackArg,
                                                                    encap_type_t encap_type );

//----------------------------------------------------------------------------
// Door Lock Configuration Set Command
//
// @param: node_id
// Destination Node Id
//
// @param: channel_id
// Destination channel Id
//
// @param: type
// Operation Type
//
// @param: inside_state
// Inside Door Handles State (4 bits)
//
// @param: outside_state
// Outside Door Handles State (4 bits)
//
// @param: lock_min
// Lock Timeout Minutes
//
// @param: lock_sec
// Lock Timeout Seconds
//
// @param: Callback
// @default: NULL
// Custom function to be called on function failure
// NULL if callback is not needed
//
// @param: callbackArg
// Custom argument to be passed to custom function to be called on function
// success or failure
//

znet_error_t ICACHE_FLASH_ATTR znet_cc_door_lock_configuration_set( uint8_t node_id,
                                                                    uint8_t channel_id,
                                                                    uint8_t type,
                                                                    uint8_t inside_state,
                                                                    uint8_t outside_state,
                                                                    uint8_t lock_min,
                                                                    uint8_t lock_sec,
                                                                    uint16_t auto_relock_timeout,
                                                                    uint16_t hold_and_release_timeout,
                                                                    bool block_to_block_enabled,
                                                                    bool twist_assist_enabled,
                                                                    ZFunction_Callback Callback,
                                                                    void* callbackArg,
                                                                    encap_type_t encap_type );

//----------------------------------------------------------------------------
// Door Lock Configuration Report CB
//
void ICACHE_FLASH_ATTR znet_cc_door_lock_configuration_report( const ZFunction func,
                                                               uint8_t node_id,
                                                               int cc_data_len,
                                                               const uint8_t* cc_data );

//----------------------------------------------------------------------------
// Door Lock Operation Get Command
//
// @param: node_id
// Destination Node Id
//
// @param: channel_id
// Destination channel Id
//
// @param: Callback
// @default: NULL
// Custom function to be called on function failure
// NULL if callback is not needed
//
// @param: callbackArg
// Custom argument to be passed to custom function to be called on function
// success or failure
//
znet_error_t ICACHE_FLASH_ATTR znet_cc_door_lock_operation_get( uint8_t node_id,
                                                                uint8_t channel_id,
                                                                ZFunction_Callback Callback,
                                                                void* callbackArg,
                                                                encap_type_t encap_type );

//----------------------------------------------------------------------------
// Door Lock Operation Set Command
//
// @param: node_id
// Destination Node Id
//
// @param: channel_id
// Destination channel Id
//
// @param: mode
// Door Lock Mode
//
// @param: Callback
// @default: NULL
// Custom function to be called on function failure
// NULL if callback is not needed
//
// @param: callbackArg
// Custom argument to be passed to custom function to be called on function
// success or failure
//
znet_error_t ICACHE_FLASH_ATTR znet_cc_door_lock_operation_set( uint8_t node_id,
                                                                uint8_t channel_id,
                                                                uint8_t mode,
                                                                ZFunction_Callback Callback,
                                                                void* callbackArg,
                                                                encap_type_t encap_type );

//----------------------------------------------------------------------------
// Door Lock Operation Report CB
//
void ICACHE_FLASH_ATTR znet_cc_door_lock_operation_report( const ZFunction func,
                                                           uint8_t node_id,
                                                           int cc_data_len,
                                                           const uint8_t* cc_data );

//----------------------------------------------------------------------------
// Door Lock Capabilities
//
znet_error_t ICACHE_FLASH_ATTR znet_cc_door_lock_capabilities_get( uint8_t node_id,
                                                                   uint8_t channel_id,
                                                                   ZFunction_Callback Callback,
                                                                   void* callbackArg,
                                                                   encap_type_t encap_type );

void ICACHE_FLASH_ATTR znet_cc_door_lock_capabilities_report( const ZFunction func,
                                                              uint8_t node_id,
                                                              int cc_data_len,
                                                              const uint8_t* cc_data );

//////////////////////////////////////////////////////////////////////////////
// Command Class Thermostat Setpoint

//----------------------------------------------------------------------------
// Thermostat Setpoint Get Command
//
// @param: node_id
// Destination Node Id
//
// @param: channel_id
// Destination channel Id
//
// @param: type
// Setpoint Type (4 bits)
//
// @param: Callback
// @default: NULL
// Custom function to be called on function failure
// NULL if callback is not needed
//
// @param: callbackArg
// Custom argument to be passed to custom function to be called on function
// success or failure
//
znet_error_t ICACHE_FLASH_ATTR znet_cc_thermostat_setpoint_get( uint8_t node_id,
                                                                uint8_t channel_id,
                                                                uint8_t type,
                                                                ZFunction_Callback Callback,
                                                                void* callbackArg,
                                                                encap_type_t encap_type );

//----------------------------------------------------------------------------
// Thermostat Setpoint Set Command
//
// @param: node_id
// Destination Node Id
//
// @param: channel_id
// Destination channel Id
//
// @param: type
// Setpoint Type (4 bits)
//
// @param: size
// Size (3 bits)
//
// @param: scale
// Scale (2 bits)
//
// @param: precision
// Precision (3 bits)
//
// @param: value
// Value
//
// @param: Callback
// @default: NULL
// Custom function to be called on function failure
// NULL if callback is not needed
//
// @param: callbackArg
// Custom argument to be passed to custom function to be called on function
// success or failure
//
znet_error_t ICACHE_FLASH_ATTR znet_cc_thermostat_setpoint_set( uint8_t node_id,
                                                                uint8_t channel_id,
                                                                uint8_t type,
                                                                uint8_t size,
                                                                uint8_t scale,
                                                                uint8_t precision,
                                                                const uint8_t* value,
                                                                ZFunction_Callback Callback,
                                                                void* callbackArg,
                                                                encap_type_t encap_type );

//----------------------------------------------------------------------------
// Thermostat Setpoint Report Command
//
void ICACHE_FLASH_ATTR znet_cc_thermostat_setpoint_report( const ZFunction func,
                                                           uint8_t node_id,
                                                           int cc_data_len,
                                                           const uint8_t* cc_data );

//----------------------------------------------------------------------------
// Thermostat Setpoint Supported Get Command
//
// @param: node_id
// Destination Node Id
//
// @param: channel_id
// Destination channel Id
//
// @param: Callback
// @default: NULL
// Custom function to be called on function failure
// NULL if callback is not needed
//
// @param: callbackArg
// Custom argument to be passed to custom function to be called on function
// success or failure
//
znet_error_t ICACHE_FLASH_ATTR znet_cc_thermostat_setpoint_supported_get( uint8_t node_id,
                                                                          uint8_t channel_id,
                                                                          ZFunction_Callback Callback,
                                                                          void* callbackArg,
                                                                          encap_type_t encap_type );

//----------------------------------------------------------------------------
// Thermostat Setpoint Supported Report Command
//
void ICACHE_FLASH_ATTR znet_cc_thermostat_setpoint_supported_report( const ZFunction func,
                                                                     uint8_t node_id,
                                                                     int cc_data_len,
                                                                     const uint8_t* cc_data );

//----------------------------------------------------------------------------
// Thermostat Setpoint Capabilities Get Command
//
// @param: node_id
// Destination Node Id
//
// @param: channel_id
// Destination channel Id
//
// @param: type
// Setpoint Type (4 bits)
//
// @param: Callback
// @default: NULL
// Custom function to be called on function failure
// NULL if callback is not needed
//
// @param: callbackArg
// Custom argument to be passed to custom function to be called on function
// success or failure
//
znet_error_t ICACHE_FLASH_ATTR znet_cc_thermostat_setpoint_capabilities_get( uint8_t node_id,
                                                                             uint8_t channel_id,
                                                                             uint8_t type,
                                                                             ZFunction_Callback Callback,
                                                                             void* callbackArg,
                                                                             encap_type_t encap_type );

//----------------------------------------------------------------------------
// Thermostat Setpoint Capabilities Report Command
//
void ICACHE_FLASH_ATTR znet_cc_thermostat_setpoint_capabilities_report( const ZFunction func,
                                                                        uint8_t node_id,
                                                                        int cc_data_len,
                                                                        const uint8_t* cc_data );

//----------------------------------------------------------------------------
// Indicator Get Command
//
// @param: node_id
// Destination Node Id
//
// @param: id_available
//
// @param: id
// Indicator ID
//
// @param: version
// Indicator CC version
//
// @param: Callback
// @default: NULL
// Custom function to be called on function failure or success
// NULL if callback is not needed
//
// @param: callbackArg
// Custom argument to be passed to custom function to be called on function
// success or failure
//
znet_error_t ICACHE_FLASH_ATTR znet_cc_indicator_get( uint8_t node_id,
                                                      uint8_t channel_id,
                                                      uint8_t id,
                                                      uint8_t version,
                                                      ZFunction_Callback Callback,
                                                      void* callbackArg,
                                                      encap_type_t encap_type );

//----------------------------------------------------------------------------
// Indicator Set Command
//
// @param: node_id
// Destination Node Id
//
// @param: value
// Indicator 0 Value
//
// @param: object_available
//
// @param: object_count
// Indicator Object Count (5 bits)
//
// @param: object
// Indicator Object
//
// @param: Callback
// @default: NULL
// Custom function to be called on function failure or success
// NULL if callback is not needed
//
// @param: callbackArg
// Custom argument to be passed to custom function to be called on function
// success or failure
//
znet_error_t ICACHE_FLASH_ATTR znet_cc_indicator_set( uint8_t node_id,
                                                      uint8_t channel_id,
                                                      uint8_t value,
                                                      uint8_t object_count,
                                                      const uint8_t* object,
                                                      ZFunction_Callback Callback,
                                                      void* callbackArg,
                                                      encap_type_t encap_type );

//----------------------------------------------------------------------------
// Indicator Report Command
//
void ICACHE_FLASH_ATTR znet_cc_indicator_report( const ZFunction func,
                                                 uint8_t node_id,
                                                 int cc_data_len,
                                                 const uint8_t* cc_data );

//----------------------------------------------------------------------------
// Indicator Supported Get Command
//
// @param: node_id
// Destination Node Id
//
// @param: id
// Indicator ID
//
// @param: Callback
// @default: NULL
// Custom function to be called on function failure or success
// NULL if callback is not needed
//
// @param: callbackArg
// Custom argument to be passed to custom function to be called on function
// success or failure
//
znet_error_t ICACHE_FLASH_ATTR znet_cc_indicator_supported_get( uint8_t node_id,
                                                                uint8_t channel_id,
                                                                uint8_t id,
                                                                ZFunction_Callback Callback,
                                                                void* callbackArg,
                                                                encap_type_t encap_type );

//----------------------------------------------------------------------------
// Indicator Supported Report Command
//
void ICACHE_FLASH_ATTR znet_cc_indicator_supported_report( const ZFunction func,
                                                           uint8_t node_id,
                                                           int cc_data_len,
                                                           const uint8_t* cc_data );

//----------------------------------------------------------------------------
// Command Class Manufacturer proprietary //

// Send Manufacturer proprietary Set
//
// @param: node_id
// Destination Node Id
//
// @param: channel_id
// Destination channel Id
//
// @param: Callback
// @default: NULL
// Custom function to be called on function failure
// NULL if callback is not needed
//
// @param: callbackArg
// Custom argument to be passed to custom function to be called on function
// success or failure
//
znet_error_t ICACHE_FLASH_ATTR znet_cc_manufacturer_proprietary_set( uint8_t node_id,
                                                                     uint8_t channel_id,
                                                                     uint8_t cmd_data_length,
                                                                     const uint8_t* cmd_data,
                                                                     ZFunction_Callback Callback,
                                                                     void* callbackArg,
                                                                     encap_type_t encap_type );

//----------------------------------------------------------------------------
// Hail Report Command
//
void ICACHE_FLASH_ATTR znet_cc_hail_report( const ZFunction func,
                                            uint8_t node_id,
                                            int cc_data_len,
                                            const uint8_t* cc_data );

znet_error_t ICACHE_FLASH_ATTR znet_cc_anti_theft_set( uint8_t node_id,
                                                       uint8_t channel_id,
                                                       bool enabled,
                                                       uint8_t magic_code_size,
                                                       const uint8_t* magic_code,
                                                       uint16_t manufacturer_id,
                                                       uint8_t hint_size,
                                                       const uint8_t* hint,
                                                       uint16_t zwave_locking_id,
                                                       ZFunction_Callback Callback,
                                                       void* callbackArg,
                                                       encap_type_t encap_type );

znet_error_t ICACHE_FLASH_ATTR znet_cc_anti_theft_get( uint8_t node_id,
                                                       uint8_t channel_id,
                                                       ZFunction_Callback Callback,
                                                       void* callbackArg,
                                                       encap_type_t encap_type );

void ICACHE_FLASH_ATTR znet_cc_anti_theft_report( const ZFunction func,
                                                  uint8_t node_id,
                                                  int cc_data_len,
                                                  const uint8_t* cc_data );

znet_error_t ICACHE_FLASH_ATTR znet_cc_anti_theft_unlock_set( uint8_t node_id,
                                                              uint8_t channel_id,
                                                              uint8_t magic_code_size,
                                                              const uint8_t* magic_code,
                                                              ZFunction_Callback Callback,
                                                              void* callbackArg,
                                                              encap_type_t encap_type );

znet_error_t ICACHE_FLASH_ATTR znet_cc_anti_theft_unlock_state_get( uint8_t node_id,
                                                                    uint8_t channel_id,
                                                                    ZFunction_Callback Callback,
                                                                    void* callbackArg,
                                                                    encap_type_t encap_type );

void ICACHE_FLASH_ATTR znet_cc_anti_theft_unlock_state_report( const ZFunction func,
                                                               uint8_t node_id,
                                                               int cc_data_len,
                                                               const uint8_t* cc_data );

//////////////////////////////////////////////////////////////////////////////
// Command Class Simple AV Control //

void ICACHE_FLASH_ATTR znet_cc_simple_av_set_report( const ZFunction func,
                                                     uint8_t node_id,
                                                     int cc_data_len,
                                                     const uint8_t* cc_data );

#endif  // #ifndef __ZNET_LIB_CC_APPLICATION_H__
