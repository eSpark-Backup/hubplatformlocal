
#ifndef __ZNET_LIB_CC_MANAGMENT_H__
#define __ZNET_LIB_CC_MANAGMENT_H__

#include "znet.h"

#include "ZFunctionClass.h"

#include "znet_lib_cc_codes.h"

//////////////////////////////////////////////////////////////////////////////
// Command Class Application Status

//----------------------------------------------------------------------------
// Application Busy Command
//
// @param: node_id
// Destination Node Id
//
// @param: status
// Status
//
// @param: wait_time
// Wait Time
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
znet_error_t ICACHE_FLASH_ATTR znet_cc_application_busy( uint8_t node_id,
                                                         uint8_t status,
                                                         uint8_t wait_time,
                                                         ZFunction_Callback Callback,
                                                         void* callbackArg );

//----------------------------------------------------------------------------
// Application Rejected Request Command
//
// @param: node_id
// Destination Node Id
//
// @param: status
// Status
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
znet_error_t ICACHE_FLASH_ATTR znet_cc_application_rejected_request( uint8_t node_id,
                                                                     uint8_t status,
                                                                     ZFunction_Callback Callback,
                                                                     void* callbackArg );

//////////////////////////////////////////////////////////////////////////////
// Command Class Association //

// Send Association Get
//
// @param: znet
// ZNet object instance
//
// @param: node_id
// Destination Node Id
//
// @param: channel_id
// Destination channel Id
//
// @param: channel_id
// Destination channel Id
//
// @param: group_id
// @default: 0
// Group Id (from 1 to 255)
// 0 requests all groups
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
znet_error_t ICACHE_FLASH_ATTR znet_cc_association_get( uint8_t node_id,
                                                        uint8_t channel_id,
                                                        uint8_t grouping_id,
                                                        ZFunction_Callback Callback,
                                                        void* callbackArg,
                                                        encap_type_t encap_type );

// Send Association Set (Add)
//
// @param: znet
// ZNet object instance
//
// @param: node_id
// Destination Node Id
//
// @param: channel_id
// Destination channel Id
//
// @param: group_id
// Group Id (from 1 to 255)
//
// @param: node_count
// Count of the Nodes to be added to the group
//
// @param: nodes
// Nodes to be added to the group
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
znet_error_t ICACHE_FLASH_ATTR znet_cc_association_set( uint8_t node_id,
                                                        uint8_t channel_id,
                                                        uint8_t group_id,
                                                        int node_count,
                                                        const uint8_t* nodes,
                                                        ZFunction_Callback Callback,
                                                        void* callbackArg,
                                                        encap_type_t encap_type );

// Send Association Remove
//
// @param: znet
// ZNet object instance
//
// @param: node_id
// Destination Node Id
//
// @param: channel_id
// Destination channel Id
//
// @param: group_id
// Group Id (from 1 to 255)
//
// @param: node_count
// Count of the Nodes to be removed from the group
//
// @param: nodes
// Nodes to be removed from the group
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
znet_error_t ICACHE_FLASH_ATTR znet_cc_association_remove( uint8_t node_id,
                                                           uint8_t channel_id,
                                                           uint8_t group_id,
                                                           int node_count,
                                                           const uint8_t* nodes,
                                                           ZFunction_Callback Callback,
                                                           void* callbackArg,
                                                           encap_type_t encap_type );

// Association Report CB
//
void ICACHE_FLASH_ATTR znet_cc_association_report( const ZFunction func,
                                                   uint8_t node_id,
                                                   int cc_data_len,
                                                   const uint8_t* cc_data );

// Send Association Groupings Get
//
// @param: znet
// ZNet object instance
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
znet_error_t ICACHE_FLASH_ATTR znet_cc_association_grouping_get( uint8_t node_id,
                                                                 uint8_t channel_id,
                                                                 ZFunction_Callback Callback,
                                                                 void* callbackArg,
                                                                 encap_type_t encap_type );

// Association Groupings Report CB
//
void ICACHE_FLASH_ATTR znet_cc_association_groupings_report( const ZFunction func,
                                                             uint8_t node_id,
                                                             int cc_data_len,
                                                             const uint8_t* cc_data );

// Send Association Specific Group Get
//
// @param: znet
// ZNet object instance
//
// @param: node_id
// Destination Node Id
//
// @param: channel_id
// Destination channel Id
//
// @param: group_id
// Group Id (from 1 to 255)
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
znet_error_t ICACHE_FLASH_ATTR znet_cc_association_specific_group_get( uint8_t node_id,
                                                                       uint8_t channel_id,
                                                                       ZFunction_Callback Callback,
                                                                       void* callbackArg,
                                                                       encap_type_t encap_type );

// Association Specific Group Report CB
//
void ICACHE_FLASH_ATTR znet_cc_association_specific_group_report( const ZFunction func,
                                                                  uint8_t node_id,
                                                                  int cc_data_len,
                                                                  const uint8_t* cc_data );

// Association Receive
//
void ICACHE_FLASH_ATTR znet_cc_association_receive( const ZFunction func,
                                                    uint8_t node_id,
                                                    int cc_data_len,
                                                    const uint8_t* cc_data );

// Association Reset
//
void ICACHE_FLASH_ATTR association_reset( void );
void ICACHE_FLASH_ATTR association_remove_node( uint8_t node_id );
size_t ICACHE_FLASH_ATTR association_get_nodes_in_lifeline_group( const uint8_t** nodes );
void ICACHE_FLASH_ATTR associations_load( void );

//////////////////////////////////////////////////////////////////////////////
// Command Class Indicator

// Indicator SET Receive
//
void ICACHE_FLASH_ATTR znet_cc_indicator_set_receive( const ZFunction func,
                                                      uint8_t node_id,
                                                      int cc_data_len,
                                                      const uint8_t* cc_data );

// Indicator GET Receive
//
void ICACHE_FLASH_ATTR znet_cc_indicator_get_receive( const ZFunction func,
                                                      uint8_t node_id,
                                                      int cc_data_len,
                                                      const uint8_t* cc_data );

// Indicator Supported GET Receive
//
void ICACHE_FLASH_ATTR znet_cc_indicator_supported_get_receive( const ZFunction func,
                                                                uint8_t node_id,
                                                                int cc_data_len,
                                                                const uint8_t* cc_data );

//////////////////////////////////////////////////////////////////////////////
// Command Class Association Command Configuration

//----------------------------------------------------------------------------
// Command Configuration Get Command
//
// @param: node_id
// Destination Node Id
//
// @param: grouping_identifier
// Grouping identifier
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
znet_error_t ICACHE_FLASH_ATTR znet_cc_command_configuration_get( uint8_t node_id,
                                                                  uint8_t grouping_identifier,
                                                                  ZFunction_Callback Callback,
                                                                  void* callbackArg );

//----------------------------------------------------------------------------
// Command Configuration Set Command
//
// @param: node_id
// Destination Node Id
//
// @param: grouping_identifier
// Grouping identifier
//
// @param: cmd_length
// Command length
//
// @param: cmd_class_id
// Command Class identifier
//
// @param: cmd_id
// Command identifier
//
// @param: cmd_data_length
//
// @param: cmd_data
// Command byte
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
znet_error_t ICACHE_FLASH_ATTR znet_cc_command_configuration_set( uint8_t node_id,
                                                                  uint8_t grouping_identifier,
                                                                  uint8_t cmd_length,
                                                                  uint8_t cmd_class_id,
                                                                  uint8_t cmd_id,
                                                                  uint8_t cmd_data_length,
                                                                  const uint8_t* cmd_data,
                                                                  ZFunction_Callback Callback,
                                                                  void* callbackArg );

//----------------------------------------------------------------------------
// Command Records Supported Get Command
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
znet_error_t ICACHE_FLASH_ATTR znet_cc_command_records_supported_get( uint8_t node_id,
                                                                      ZFunction_Callback Callback,
                                                                      void* callbackArg );

//////////////////////////////////////////////////////////////////////////////
// Command Class Association Group Info

//----------------------------------------------------------------------------
// Association Group Name Get Command
//
// @param: znet
// ZNet object instance
//
// @param: node_id
// Destination Node Id
//
// @param: channel_id
// Destination channel Id
//
// @param: id
// Grouping Identifier
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
znet_error_t ICACHE_FLASH_ATTR znet_cc_association_group_name_get( uint8_t node_id,
                                                                   uint8_t channel_id,
                                                                   uint8_t id,
                                                                   ZFunction_Callback Callback,
                                                                   void* callbackArg,
                                                                   encap_type_t encap_type );

// Association Specific Group Report CB
//
void ICACHE_FLASH_ATTR znet_cc_association_group_name_report( const ZFunction func,
                                                              uint8_t node_id,
                                                              int cc_data_len,
                                                              const uint8_t* cc_data );

//----------------------------------------------------------------------------
// Association Group Info Get Command
//
// @param: znet
// ZNet object instance
//
// @param: node_id
// Destination Node Id
//
// @param: channel_id
// Destination channel Id
//
// @param: list_mode
// List Mode (1 bits)
//
// @param: refresh_cache
// Refresh cache (1 bits)
//
// @param: id
// Grouping Identifier
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
znet_error_t ICACHE_FLASH_ATTR znet_cc_association_group_info_get( uint8_t node_id,
                                                                   uint8_t channel_id,
                                                                   bool list_mode,
                                                                   bool refresh_cache,
                                                                   uint8_t id,
                                                                   ZFunction_Callback Callback,
                                                                   void* callbackArg,
                                                                   encap_type_t encap_type );

// Association Group Info Report CB
//
void ICACHE_FLASH_ATTR znet_cc_association_group_info_report( const ZFunction func,
                                                              uint8_t node_id,
                                                              int cc_data_len,
                                                              const uint8_t* cc_data );

//----------------------------------------------------------------------------
// Association Group Command List Get Command
//
// @param: znet
// ZNet object instance
//
// @param: node_id
// Destination Node Id
//
// @param: channel_id
// Destination channel Id
//
// @param: allow_cache
// Refresh cache (1 bits)
//
// @param: id
// Grouping Identifier
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
znet_error_t ICACHE_FLASH_ATTR znet_cc_association_group_command_list_get( uint8_t node_id,
                                                                           uint8_t channel_id,
                                                                           bool allow_cache,
                                                                           uint8_t id,
                                                                           ZFunction_Callback Callback,
                                                                           void* callbackArg,
                                                                           encap_type_t encap_type );

// Association Group Command List Report CB
//
void ICACHE_FLASH_ATTR znet_cc_association_group_command_list_report( const ZFunction func,
                                                                      uint8_t node_id,
                                                                      int cc_data_len,
                                                                      const uint8_t* cc_data );

//----------------------------------------------------------------------------
// Association Group Info Receive
void ICACHE_FLASH_ATTR znet_cc_association_group_info_receive( const ZFunction func,
                                                               uint8_t node_id,
                                                               int cc_data_len,
                                                               const uint8_t* cc_data );

//////////////////////////////////////////////////////////////////////////////
// Command Class Battery //

// Send Battery Get
//
// @param: znet
// ZNet object instance
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
znet_error_t ICACHE_FLASH_ATTR znet_cc_battery_get( uint8_t node_id,
                                                    uint8_t channel_id,
                                                    ZFunction_Callback Callback,
                                                    void* callbackArg,
                                                    encap_type_t encap_type );

// Association Report CB
//
void ICACHE_FLASH_ATTR znet_cc_battery_report( const ZFunction func,
                                               uint8_t node_id,
                                               int cc_data_len,
                                               const uint8_t* cc_data );

//////////////////////////////////////////////////////////////////////////////
// Command Class Device Reset Localy//

//----------------------------------------------------------------------------
// Device Reset Locally Notification Command
//
// @param: znet
// ZNet object instance
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
void ICACHE_FLASH_ATTR znet_cc_device_reset_localy_report( const ZFunction func,
                                                           uint8_t node_id,
                                                           int cc_data_len,
                                                           const uint8_t* cc_data );

void ICACHE_FLASH_ATTR znet_cc_device_reset_localy_report_lib( const ZFunction func,
                                                               uint8_t node_id,
                                                               int cc_data_len,
                                                               const uint8_t* cc_data );

//----------------------------------------------------------------------------
// Device Reset Locally Notification Command
//
// @param: znet
// ZNet object instance
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
znet_error_t ICACHE_FLASH_ATTR znet_cc_device_reset_localy_notification( uint8_t node_id,
                                                                         uint8_t channel_id,
                                                                         ZFunction_Callback Callback,
                                                                         void* callbackArg,
                                                                         encap_type_t encap_type );
//////////////////////////////////////////////////////////////////////////////
// Command Class Grouping Name//

//----------------------------------------------------------------------------
// Grouping Name Get Command
//
// @param: node_id
// Destination Node Id
//
// @param: grouping_id
// Grouping identifier
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
znet_error_t ICACHE_FLASH_ATTR znet_cc_grouping_name_get( uint8_t node_id,
                                                          uint8_t grouping_id,
                                                          ZFunction_Callback Callback,
                                                          void* callbackArg );

//----------------------------------------------------------------------------
// Grouping Name Set Command
//
// @param: node_id
// Destination Node Id
//
// @param: grouping_id
// Grouping identifier
//
// @param: char_present
// Char. Presentation (3 bits)
//
// @param: grouping_name
// Grouping Name
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
znet_error_t ICACHE_FLASH_ATTR znet_cc_grouping_name_set( uint8_t node_id,
                                                          uint8_t grouping_id,
                                                          uint8_t char_present,
                                                          const uint8_t* grouping_name,
                                                          ZFunction_Callback Callback,
                                                          void* callbackArg );

//////////////////////////////////////////////////////////////////////////////
// Command Class Hail

//----------------------------------------------------------------------------
// Hail Command
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
znet_error_t ICACHE_FLASH_ATTR znet_cc_hail( uint8_t node_id, ZFunction_Callback Callback, void* callbackArg );

//////////////////////////////////////////////////////////////////////////////
// Command Class ManufacturerSpecific //

// Send ManufacturerSpecific Get
//
// @param: znet
// ZNet object instance
//
// @param: node_id
// Destination Node Id
//
// @param: channel_id
// Destination channel Id
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

znet_error_t ICACHE_FLASH_ATTR znet_cc_manufacturer_specific_get( uint8_t node_id,
                                                                  uint8_t channel_id,
                                                                  ZFunction_Callback Callback,
                                                                  void* callbackArg,
                                                                  encap_type_t encap_type );

// ManufacturerSpecific Report CB
//
void ICACHE_FLASH_ATTR znet_cc_manufacturer_specific_report( const ZFunction func,
                                                             uint8_t node_id,
                                                             int cc_data_len,
                                                             const uint8_t* cc_data );

znet_error_t ICACHE_FLASH_ATTR znet_cc_device_specific_get( uint8_t node_id,
                                                            uint8_t channel_id,
                                                            uint8_t device_type_id,
                                                            ZFunction_Callback Callback,
                                                            void* callbackArg,
                                                            encap_type_t encap );

//----------------------------------------------------------------------------
void ICACHE_FLASH_ATTR znet_cc_manufacturer_specific_fill( const ZFunction func,
                                                           ZW_MANUFACTURER_SPECIFIC_REPORT_FRAME* data );

//----------------------------------------------------------------------------
// DeviceSpecific Request CB
//
void ICACHE_FLASH_ATTR znet_cc_manufacturer_specific_requested( const ZFunction func,
                                                                uint8_t node_id,
                                                                int cc_data_len,
                                                                const uint8_t* cc_data );

// DeviceSpecific Report CB
//
void ICACHE_FLASH_ATTR znet_cc_device_specific_report( const ZFunction func,
                                                       uint8_t node_id,
                                                       int cc_data_len,
                                                       const uint8_t* cc_data );

//////////////////////////////////////////////////////////////////////////////
// Command Class MultiChannelAssociation //

// Send MultiChannelAssociation Get
//
// @param: znet
// ZNet object instance
//
// @param: node_id
// Destination Node Id
//
// @param: channel_id
// Destination channel Id
//
// @param: group_id
// @default: 0
// Group Id (from 1 to 255)
// 0 requests all groups
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
znet_error_t ICACHE_FLASH_ATTR znet_cc_multichannel_association_get( uint8_t node_id,
                                                                     uint8_t channel_id,
                                                                     uint8_t group_id,
                                                                     ZFunction_Callback Callback,
                                                                     void* callbackArg,
                                                                     encap_type_t encap_type );

// Send MultiChannelAssociation Set (Add)
//
// @param: znet
// ZNet object instance
//
// @param: node_id
// Destination Node Id
//
// @param: channel_id
// Destination channel Id
//
// @param: group_id
// Group Id (from 1 to 255)
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
znet_error_t ICACHE_FLASH_ATTR znet_cc_multichannel_association_set( uint8_t node_id,
                                                                     uint8_t channel_id,
                                                                     uint8_t group_id,
                                                                     uint8_t nodes_count,
                                                                     const uint8_t* nodes,
                                                                     uint8_t multiChannelNodes_count,
                                                                     const uint16_t* multiChannelNodes,
                                                                     ZFunction_Callback Callback,
                                                                     void* callbackArg,
                                                                     encap_type_t encap_type );

// Send MultiChannelAssociation Remove
//
// @param: znet
// ZNet object instance
//
// @param: node_id
// Destination Node Id
//
// @param: channel_id
// Destination channel Id
//
// @param: group_id
// Group Id (from 1 to 255)
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
znet_error_t ICACHE_FLASH_ATTR znet_cc_multichannel_association_remove( uint8_t node_id,
                                                                        uint8_t channel_id,
                                                                        uint8_t group_id,
                                                                        uint8_t nodes_count,
                                                                        const uint8_t* nodes,
                                                                        uint8_t multiChannelNodes_count,
                                                                        const uint16_t* multiChannelNodes,
                                                                        ZFunction_Callback Callback,
                                                                        void* callbackArg,
                                                                        encap_type_t encap_type );

//----------------------------------------------------------------------------
// Multi Channel Association Report Command
//
void ICACHE_FLASH_ATTR znet_cc_multichannel_association_report( const ZFunction func,
                                                                uint8_t node_id,
                                                                int cc_data_len,
                                                                const uint8_t* );

// Send MultiChannelAssociation GroupingsGet
//
// @param: znet
// ZNet object instance
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
znet_error_t ICACHE_FLASH_ATTR znet_cc_multichannel_association_groupings_get( uint8_t node_id,
                                                                               uint8_t channel_id,
                                                                               ZFunction_Callback Callback,
                                                                               void* callbackArg,
                                                                               encap_type_t encap_type );

//----------------------------------------------------------------------------
// Multi Channel Association Groupings Report Command
//
void ICACHE_FLASH_ATTR znet_cc_multichannel_association_groupings_report( const ZFunction func,
                                                                          uint8_t node_id,
                                                                          int cc_data_len,
                                                                          const uint8_t* cc_data );

//----------------------------------------------------------------------------
// Multi Channel Association Receive
//
void ICACHE_FLASH_ATTR znet_cc_multichannel_association_receive( const ZFunction func,
                                                                 uint8_t node_id,
                                                                 int cc_data_len,
                                                                 const uint8_t* cc_data );

//----------------------------------------------------------------------------
//
void ICACHE_FLASH_ATTR multichannel_associations_load( void );

//////////////////////////////////////////////////////////////////////////////
// Command Class Node Naming //

// Send NodeNaming GetLocation
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
znet_error_t ICACHE_FLASH_ATTR znet_cc_node_naming_location_get( uint8_t node_id,
                                                                 ZFunction_Callback Callback,
                                                                 void* callbackArg );

// Send NodeNaming SetLocation
//
// @param: node_id
// Destination Node Id
//
// @param: char_presentation
//
// @param: location
// Value
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
znet_error_t ICACHE_FLASH_ATTR znet_cc_node_naming_location_set( uint8_t node_id,
                                                                 uint8_t char_presentation,
                                                                 const uint8_t* location,
                                                                 ZFunction_Callback Callback,
                                                                 void* callbackArg );

//----------------------------------------------------------------------------
// Node Naming Node Location  Report Command
//
void ICACHE_FLASH_ATTR znet_cc_node_naming_location_report( const ZFunction func,
                                                            uint8_t node_id,
                                                            int cc_data_len,
                                                            const uint8_t* cc_data );

// Send NodeNaming GetName
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
znet_error_t ICACHE_FLASH_ATTR znet_cc_node_naming_name_get( uint8_t node_id,
                                                             ZFunction_Callback Callback,
                                                             void* callbackArg );

// Send NodeNaming SetName
//
// @param: node_id
// Destination Node Id
//
// @param: char_presentation
//
// @param: name
// Value
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
znet_error_t ICACHE_FLASH_ATTR znet_cc_node_naming_name_set( uint8_t node_id,
                                                             uint8_t char_presentation,
                                                             const uint8_t* name,
                                                             ZFunction_Callback Callback,
                                                             void* callbackArg );

//----------------------------------------------------------------------------
// Node Naming Node Name Report Command
//
void ICACHE_FLASH_ATTR znet_cc_node_naming_name_report( const ZFunction func,
                                                        uint8_t node_id,
                                                        int cc_data_len,
                                                        const uint8_t* cc_data );

//////////////////////////////////////////////////////////////////////////////
// Command Class Remote Association Activate//

//----------------------------------------------------------------------------
// Remote Association Configuration Get Command
//
// @param: node_id
// Destination Node Id
//
// @param: local_grouping_id
// Local Grouping identifier
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
znet_error_t ICACHE_FLASH_ATTR znet_cc_remote_association_config_get( uint8_t node_id,
                                                                      uint8_t local_grouping_id,
                                                                      ZFunction_Callback Callback,
                                                                      void* callbackArg );

//----------------------------------------------------------------------------
// Remote Association Configuration Set Command
//
// @param: node_id
// Node Id
//
// @param: local_grouping_id
// Local Grouping identifier
//
// @param: remote_node_id
// Remote Node ID
//
// @param: remoute_grouping_id
// Remote Grouping identifie
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
znet_error_t ICACHE_FLASH_ATTR znet_cc_remote_association_config_set( uint8_t node_id,
                                                                      uint8_t local_grouping_id,
                                                                      uint8_t remote_node_id,
                                                                      uint8_t remoute_grouping_id,
                                                                      ZFunction_Callback Callback,
                                                                      void* callbackArg );

//////////////////////////////////////////////////////////////////////////////
// Command Class Remote Association Activate//

//----------------------------------------------------------------------------
// Remote Association Activate Command
//
// @param: node_id
// Destination Node Id
//
// @param: grouping_id
// Grouping Id
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
znet_error_t ICACHE_FLASH_ATTR znet_cc_remote_association_activate( uint8_t node_id,
                                                                    uint8_t grouping_id,
                                                                    ZFunction_Callback Callback,
                                                                    void* callbackArg );

//////////////////////////////////////////////////////////////////////////////

// Command Class Time Parameters

//----------------------------------------------------------------------------
// Time Parameters Get Command
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
znet_error_t ICACHE_FLASH_ATTR znet_cc_time_parameters_get( uint8_t node_id,
                                                            uint8_t channel_id,
                                                            ZFunction_Callback Callback,
                                                            void* callbackArg,
                                                            encap_type_t encap_type );

//----------------------------------------------------------------------------
// Time Parameters Set Command
//
// @param: node_id
// Destination Node Id
//
// @param: year
// year
//
// @param: month
// month
//
// @param: day
// day
//
// @param: hour_utc
// Hour UTC
//
// @param: minute_utc
// Minute UTC
//
// @param: second_utc
// Second UTC
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

znet_error_t ICACHE_FLASH_ATTR znet_cc_time_parameters_set( uint8_t node_id,
                                                            uint8_t channel_id,
                                                            uint16_t year,
                                                            uint8_t month,
                                                            uint8_t day,
                                                            uint8_t hour_utc,
                                                            uint8_t minute_utc,
                                                            uint8_t second_utc,
                                                            ZFunction_Callback Callback,
                                                            void* callbackArg,
                                                            encap_type_t encap_type );

// Time Parameters Report CB
//
void ICACHE_FLASH_ATTR znet_cc_time_parameters_report( const ZFunction func,
                                                       uint8_t node_id,
                                                       int cc_data_len,
                                                       const uint8_t* cc_data );

//////////////////////////////////////////////////////////////////////////////
// Command Class Version //

//----------------------------------------------------------------------------
znet_error_t ICACHE_FLASH_ATTR znet_cc_version_get( uint8_t node_id,
                                                    uint8_t channel_id,
                                                    ZFunction_Callback Callback,
                                                    void* callbackArg,
                                                    encap_type_t encap_type );

// Version Report CB
//
void ICACHE_FLASH_ATTR znet_cc_version_report( const ZFunction func,
                                               uint8_t node_id,
                                               int cc_data_len,
                                               const uint8_t* cc_data );

znet_error_t ICACHE_FLASH_ATTR znet_cc_version_command_class_get( uint8_t node_id,
                                                                  uint8_t channel_id,
                                                                  uint8_t cc_id,
                                                                  ZFunction_Callback Callback,
                                                                  void* callbackArg,
                                                                  encap_type_t encap_type );

// Version Command Class Report CB
//
void ICACHE_FLASH_ATTR znet_cc_version_command_class_report( const ZFunction func,
                                                             uint8_t node_id,
                                                             int cc_data_len,
                                                             const uint8_t* cc_data );

znet_error_t ICACHE_FLASH_ATTR znet_cc_version_capabilities_get( uint8_t node_id,
                                                                 uint8_t channel_id,
                                                                 ZFunction_Callback Callback,
                                                                 void* callbackArg,
                                                                 encap_type_t encap_type );

// Version Capabilities Report CB
//
void ICACHE_FLASH_ATTR znet_cc_version_capabilities_report( const ZFunction func,
                                                            uint8_t node_id,
                                                            int cc_data_len,
                                                            const uint8_t* cc_data );

znet_error_t ICACHE_FLASH_ATTR znet_cc_version_software_get( uint8_t node_id,
                                                             uint8_t channel_id,
                                                             ZFunction_Callback Callback,
                                                             void* callbackArg,
                                                             encap_type_t encap_type );

// Version Software Report CB
//
void ICACHE_FLASH_ATTR znet_cc_version_software_report( const ZFunction func,
                                                        uint8_t node_id,
                                                        int cc_data_len,
                                                        const uint8_t* cc_data );

//----------------------------------------------------------------------------
// Version Request CB
//
void ICACHE_FLASH_ATTR znet_cc_version_request( const ZFunction func,
                                                uint8_t node_id,
                                                int cc_data_len,
                                                const uint8_t* cc_data );

void ICACHE_FLASH_ATTR znet_cc_command_class_version_request( const ZFunction func,
                                                              uint8_t node_id,
                                                              int cc_data_len,
                                                              const uint8_t* cc_data );

//----------------------------------------------------------------------------
void ICACHE_FLASH_ATTR znet_cc_version_fill( const ZFunction func, ZW_VERSION_REPORT_V2_FRAME* frame );

//////////////////////////////////////////////////////////////////////////////
// Command Class Wakeup //

//----------------------------------------------------------------------------
// Send Wakeup Get
//
// @param: znet
// ZNet object instance
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
znet_error_t ICACHE_FLASH_ATTR znet_cc_wakeup_get( uint8_t node_id,
                                                   uint8_t channel_id,
                                                   ZFunction_Callback Callback,
                                                   void* callbackArg,
                                                   encap_type_t encap_type );

//----------------------------------------------------------------------------
// Send Wakeup CapabilityGet
//
// @param: znet
// ZNet object instance
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
znet_error_t ICACHE_FLASH_ATTR znet_cc_wakeup_capabilities_get( uint8_t node_id,
                                                                uint8_t channel_id,
                                                                ZFunction_Callback Callback,
                                                                void* callbackArg,
                                                                encap_type_t encap_type );

//----------------------------------------------------------------------------
// Wake Up Interval Capabilities Report Command
//
void ICACHE_FLASH_ATTR znet_cc_wakeup_capabilities_report( const ZFunction func,
                                                           uint8_t node_id,
                                                           int cc_data_len,
                                                           const uint8_t* cc_data );

//----------------------------------------------------------------------------
// Wake Up Receive
//
void ICACHE_FLASH_ATTR znet_cc_wakeup_receive( const ZFunction func,
                                               uint8_t node_id,
                                               int cc_data_len,
                                               const uint8_t* cc_data );

//----------------------------------------------------------------------------
// Send Wakeup NoMoreInformation (Sleep)
//
// @param: znet
// ZNet object instance
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
znet_error_t ICACHE_FLASH_ATTR znet_cc_wakeup_sleep( uint8_t node_id,
                                                     uint8_t channel_id,
                                                     ZFunction_Callback Callback,
                                                     void* callbackArg,
                                                     encap_type_t encap_type );

//----------------------------------------------------------------------------
// Wake Up Notification Command
//
// @param: znet
// ZNet object instance
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
void ICACHE_FLASH_ATTR znet_cc_wakeup_notification( const ZFunction func,
                                                    uint8_t node_id,
                                                    int cc_data_len,
                                                    const uint8_t* cc_data );

// Lib CB
void ICACHE_FLASH_ATTR znet_cc_wakeup_notification_lib( const ZFunction func,
                                                        uint8_t node_id,
                                                        int cc_data_len,
                                                        const uint8_t* cc_data );

//----------------------------------------------------------------------------
// Send Wakeup Set
//
// @param: znet
// ZNet object instance
//
// @param: node_id
// Destination Node Id
//
// @param: channel_id
// Destination channel Id
//
// @param: interval
// Wakeup interval in seconds
//
// @param: notification_node_id
// Node Id to be notified about wakeup
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
znet_error_t ICACHE_FLASH_ATTR znet_cc_wakeup_set( uint8_t node_id,
                                                   uint8_t channel_id,
                                                   uint32_t interval_sec,
                                                   uint8_t notification_node_id,
                                                   ZFunction_Callback Callback,
                                                   void* callbackArg,
                                                   encap_type_t encap_type );

//----------------------------------------------------------------------------
// Wake Up Interval Report Command
//
void ICACHE_FLASH_ATTR znet_cc_wakeup_report( const ZFunction func,
                                              uint8_t node_id,
                                              int cc_data_len,
                                              const uint8_t* cc_data );

//////////////////////////////////////////////////////////////////////////////
// Command Class ZWavePlus Info //

//----------------------------------------------------------------------------
// Send ZWave+ Info Get
//
// @param: znet
// ZNet object instance
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
znet_error_t ICACHE_FLASH_ATTR znet_cc_zwave_plus_info_get( uint8_t node_id,
                                                            uint8_t channel_id,
                                                            ZFunction_Callback Callback,
                                                            void* callbackArg,
                                                            encap_type_t encap_type );

//----------------------------------------------------------------------------
// Z-Wave+ Info Report Command
void ICACHE_FLASH_ATTR znet_cc_zwave_plus_info_report( const ZFunction func,
                                                       uint8_t,
                                                       int cc_data_len,
                                                       const uint8_t* cc_data );

//----------------------------------------------------------------------------
// Z-Wave+ Info Report Command
void ICACHE_FLASH_ATTR znet_cc_zwave_plus_info_fill( const ZFunction func, ZW_ZWAVEPLUS_INFO_REPORT_V2_FRAME* data );

//----------------------------------------------------------------------------
// Z-Wave+ Info Request Command
void ICACHE_FLASH_ATTR znet_cc_zwave_plus_info_request( const ZFunction func,
                                                        uint8_t node_id,
                                                        int cc_data_len,
                                                        const uint8_t* cc_data );

//////////////////////////////////////////////////////////////////////////////
// Command Class Time

// Handles local time request
void znet_cc_local_time_requested( const ZFunction func, uint8_t node_id, int cc_data_len, const uint8_t* cc_data );

// Handles local date request
void znet_cc_local_date_requested( const ZFunction func, uint8_t node_id, int cc_data_len, const uint8_t* cc_data );

// Handles time offset request
void znet_cc_time_zone_requested( const ZFunction func, uint8_t node_id, int cc_data_len, const uint8_t* cc_data );

// Handles time zone settings receiving
void znet_cc_time_zone_received( const ZFunction func, uint8_t node_id, int cc_data_len, const uint8_t* cc_data );

// Handles time zone settings report
void znet_cc_time_zone_report( const ZFunction func, uint8_t node_id, int cc_data_len, const uint8_t* cc_data );

// Handles local time receiving
void znet_cc_local_time_received( const ZFunction func, uint8_t node_id, int cc_data_len, const uint8_t* cc_data );

// Handles local date receiving
void znet_cc_local_date_received( const ZFunction func, uint8_t node_id, int cc_data_len, const uint8_t* cc_data );

// Respond about actual local time
znet_error_t znet_cc_local_time_respond( uint8_t node_id,
                                         uint8_t channel_id,
                                         uint8_t hour,
                                         uint8_t minute,
                                         uint8_t second,
                                         ZFunction_Callback callback,
                                         void* callback_arg,
                                         encap_type_t encap_type );

// Respond about actual local date
znet_error_t znet_cc_local_date_respond( uint8_t node_id,
                                         uint8_t channel_id,
                                         uint16_t year,
                                         uint8_t month,
                                         uint8_t day,
                                         ZFunction_Callback callback,
                                         void* callback_arg,
                                         encap_type_t encap_type );

// Set time zone settings for node
znet_error_t znet_cc_time_zone_set( uint8_t node_id,
                                    uint8_t channel_id,
                                    bool is_tzo_negative,
                                    uint8_t hour_tzo,
                                    uint8_t minute_tzo,
                                    int8_t minute_dst,
                                    uint8_t month_start_dst,
                                    uint8_t month_end_dst,
                                    uint8_t day_start_dst,
                                    uint8_t day_end_dst,
                                    uint8_t hour_start_dst,
                                    uint8_t hour_end_dst,
                                    ZFunction_Callback callback,
                                    void* callback_arg,
                                    encap_type_t encap_type );

// Report time zone settings to node
znet_error_t znet_cc_time_zone_respond( uint8_t node_id,
                                        uint8_t channel_id,
                                        bool is_tzo_negative,
                                        uint8_t hour_tzo,
                                        uint8_t minute_tzo,
                                        int8_t minute_dst,
                                        uint8_t month_start_dst,
                                        uint8_t month_end_dst,
                                        uint8_t day_start_dst,
                                        uint8_t day_end_dst,
                                        uint8_t hour_start_dst,
                                        uint8_t hour_end_dst,
                                        ZFunction_Callback callback,
                                        void* callback_arg,
                                        encap_type_t encap_type );

// Get local date from node
znet_error_t znet_cc_local_date_get( uint8_t node_id,
                                     uint8_t channel_id,
                                     ZFunction_Callback callback,
                                     void* callback_arg,
                                     encap_type_t encap_type );

// Get local time from node
znet_error_t znet_cc_local_time_get( uint8_t node_id,
                                     uint8_t channel_id,
                                     ZFunction_Callback callback,
                                     void* callback_arg,
                                     encap_type_t encap_type );

// Get time zone settings from node
znet_error_t znet_cc_time_zone_get( uint8_t node_id,
                                    uint8_t channel_id,
                                    ZFunction_Callback callback,
                                    void* callback_arg,
                                    encap_type_t encap_type );

znet_error_t znet_cc_firmware_md_get( uint8_t node_id,
                                      uint8_t channel_id,
                                      ZFunction_Callback callback,
                                      void* callback_arg,
                                      encap_type_t encap_type );

znet_error_t znet_cc_firmware_update_md_request_get( uint8_t node_id,
                                                     uint8_t channel_id,
                                                     uint16_t manufacturer_id,
                                                     uint16_t firmware_id,
                                                     uint16_t firmware_checksum,
                                                     uint8_t firmware_target,
                                                     uint16_t fragment_size,
                                                     bool is_postponed_activation_allowed,
                                                     uint8_t hardware_revision,
                                                     uint8_t cc_version,
                                                     ZFunction_Callback callback,
                                                     void* callback_arg,
                                                     encap_type_t encap_type );

znet_error_t znet_cc_firmware_update_md_get( uint8_t node_id,
                                             uint8_t channel_id,
                                             uint8_t reports_count,
                                             uint16_t report_number,
                                             ZFunction_Callback callback,
                                             void* callback_arg,
                                             encap_type_t encap_type );

znet_error_t znet_cc_firmware_update_md_report( uint8_t node_id,
                                                uint8_t channel_id,
                                                uint16_t report_number,
                                                bool is_last_report,
                                                uint8_t data_size,
                                                const uint8_t* data,
                                                bool use_checksum,
                                                ZFunction_Callback callback,
                                                void* callback_arg,
                                                encap_type_t encap_type );

znet_error_t znet_cc_firmware_update_activation_set( uint8_t node_id,
                                                     uint8_t channel_id,
                                                     uint16_t manufacturer_id,
                                                     uint16_t firmware_id,
                                                     uint16_t firmware_checksum,
                                                     uint8_t firmware_target,
                                                     uint8_t hardware_version,
                                                     uint8_t cc_version,
                                                     ZFunction_Callback callback,
                                                     void* callback_arg,
                                                     encap_type_t encap_type );

znet_error_t znet_cc_firmware_update_md_prepare_get( uint8_t node_id,
                                                     uint8_t channel_id,
                                                     uint16_t manufacturer_id,
                                                     uint16_t firmware_id,
                                                     uint8_t firmware_target,
                                                     uint16_t fragment_size,
                                                     uint8_t hardware_version,
                                                     ZFunction_Callback callback,
                                                     void* callback_arg,
                                                     encap_type_t encap_type );

znet_error_t znet_cc_firmware_update_info_report( uint8_t node_id,
                                                  uint8_t channel_id,
                                                  // v1
                                                  uint16_t manufacturer_id,
                                                  uint16_t firmware_id,
                                                  uint16_t checksum,
                                                  // v3
                                                  bool firmware_upgradable,
                                                  uint16_t max_fragment_size,
                                                  uint8_t targets_count,
                                                  const uint16_t* targets,
                                                  // v5
                                                  uint8_t hardware_revision,
                                                  // callback
                                                  ZFunction_Callback callback,
                                                  void* callback_arg,
                                                  encap_type_t encap_type );

znet_error_t znet_cc_firmware_update_md_request_report( uint8_t node_id,
                                                        uint8_t channel_id,
                                                        uint8_t status,
                                                        ZFunction_Callback callback,
                                                        void* callback_arg,
                                                        encap_type_t encap_type );

znet_error_t znet_cc_firmware_update_md_prepare_report( uint8_t node_id,
                                                        uint8_t channel_id,
                                                        uint8_t status,
                                                        uint16_t checksum,
                                                        ZFunction_Callback callback,
                                                        void* callback_arg,
                                                        encap_type_t encap_type );

void znet_cc_firmware_md_requested( const ZFunction func, uint8_t node_id, int cc_data_len, const uint8_t* cc_data );

void znet_cc_firmware_md_received( const ZFunction func, uint8_t node_id, int cc_data_len, const uint8_t* cc_data );

void znet_cc_firmware_update_md_request_received( const ZFunction func,
                                                  uint8_t node_id,
                                                  int cc_data_len,
                                                  const uint8_t* cc_data );

void znet_cc_firmware_update_md_request_report_received( const ZFunction func,
                                                         uint8_t node_id,
                                                         int cc_data_len,
                                                         const uint8_t* cc_data );
void znet_cc_firmware_update_md_requested( const ZFunction func,
                                           uint8_t node_id,
                                           int cc_data_len,
                                           const uint8_t* cc_data );
void znet_cc_firmware_update_md_received( const ZFunction func,
                                          uint8_t node_id,
                                          int cc_data_len,
                                          const uint8_t* cc_data );
void znet_cc_firmware_update_md_status_received( const ZFunction func,
                                                 uint8_t node_id,
                                                 int cc_data_len,
                                                 const uint8_t* cc_data );
void znet_cc_firmware_update_md_activation_status_received( const ZFunction func,
                                                            uint8_t node_id,
                                                            int cc_data_len,
                                                            const uint8_t* cc_data );
void znet_cc_firmware_update_md_prepare_report_received( const ZFunction func,
                                                         uint8_t node_id,
                                                         int cc_data_len,
                                                         const uint8_t* cc_data );

void znet_cc_firmware_update_md_prepare_requested( const ZFunction func,
                                                   uint8_t node_id,
                                                   int cc_data_len,
                                                   const uint8_t* cc_data );

#endif  // #ifndef __ZNET_LIB_CC_MANAGMENT_H__
