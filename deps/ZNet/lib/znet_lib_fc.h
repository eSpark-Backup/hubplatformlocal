
#ifndef __ZNET_LIB_FC_H__
#define __ZNET_LIB_FC_H__

#include "ZFunctionClass.h"
#include "znet/znet_errors.h"

/* Mode parameters to ZW_SetLearnMode */
#define ZW_SET_LEARN_MODE_DISABLE 0x00
#define ZW_SET_LEARN_MODE_CLASSIC 0x01
#define ZW_SET_LEARN_MODE_NWI 0x02
#define ZW_SET_LEARN_MODE_NWE 0x03

#define NODE_STATUS_STARTED 0x01
#define NODE_STATUS_DONE 0x06
#define NODE_STATUS_FAILED 0x07

#define ZNET_ADD_NODE_STOP 0
#define ZNET_ADD_NODE_START 1
#define ZNET_ADD_NODE_SMART_START 2
#define ZNET_ADD_NODE_DSK 3

#define ADD_NODE_ANY 0x01          // initiate inclusion (add) of new node of any type
#define ADD_NODE_STOP 0x05         // abort the inclusion (add) process
#define ADD_NODE_HOME_ID 0x08      // invoke the inclusion of a specific SMART START node
#define ADD_NODE_SMART_START 0x09  // enter SMART START Listen mode

#define ADD_NODE_OPTION_NETWORK_WIDE 0x40  // Option flag to enable normal inclusion range
#define ADD_NODE_OPTION_NORMAL_POWER 0x80  // Option flag to enable NetworkWide Inclusion (NWI)

#define NVM_OPERATION_OPEN 0
#define NVM_OPERATION_READ 1
#define NVM_OPERATION_WRITE 2
#define NVM_OPERATION_CLOSE 3

/* ZW_LIBRARY_TYPEs one of these defines are returned when requesting */
/* Library type */
#define ZW_LIB_CONTROLLER_STATIC 0x01
#define ZW_LIB_CONTROLLER 0x02
#define ZW_LIB_SLAVE_ENHANCED 0x03
#define ZW_LIB_SLAVE 0x04
#define ZW_LIB_INSTALLER 0x05
#define ZW_LIB_SLAVE_ROUTING 0x06
#define ZW_LIB_CONTROLLER_BRIDGE 0x07
#define ZW_LIB_DUT 0x08
#define ZW_LIB_AVREMOTE 0x0A
#define ZW_LIB_AVDEVICE 0x0B

// Request Serial API capabilities
//
// @param: znet
// ZNet object instance
//
// @param: successCallback
// @default: NULL
// Custom function to be called on function success
// NULL if callback is not needed
//
// @param: failureCallback
// @default: NULL
// Custom function to be called on function failure
// NULL if callback is not needed
//
// @param: callbackArg
// Custom argument to be passed to custom function to be called on function
// success or failure
//
// znet_error_t ICACHE_FLASH_ATTR znet_fc_get_serial_api_capabilities(
// ZFunction_Callback Callback, void*
// callbackArg );

// Set Serial API timeouts
//
// @param: znet
// ZNet object instance
//
// @param: ackTimeout
// Time for the stick to wait for ACK (in 10ms units)
//
// @param: byteTimeout
// Time for the stick to assemble a full packet (in 10ms units)
//
// @param: successCallback
// @default: NULL
// Custom function to be called on function success
// NULL if callback is not needed
//
// @param: failureCallback
// @default: NULL
// Custom function to be called on function failure
// NULL if callback is not needed
//
// @param: callbackArg
// Custom argument to be passed to custom function to be called on function
// success or failure
//
// znet_error_t ICACHE_FLASH_ATTR znet_fc_serial_api_set_timeouts( uint8_t
// ackTimeout, uint8_t byteTimeout,
// ZFunction_Callback Callback, void* callbackArg );

// Request initial information about devices in network
//
// @param: znet
// ZNet object instance
//
// @param: successCallback
// @default: NULL
// Custom function to be called on function success
// NULL if callback is not needed
//
// @param: failureCallback
// @default: NULL
// Custom function to be called on function failure
// NULL if callback is not needed
//
// @param: callbackArg
// Custom argument to be passed to custom function to be called on function
// success or failure
//
znet_error_t ICACHE_FLASH_ATTR znet_fc_serial_api_get_init_data( ZFunction_Callback Callback, void* callbackArg );

// Set controlle_r node information
//
// @param: znet
// ZNet object instance
//
// @param: listening
// Listening flag
//
// @param: optional
// Optional flag (set if device supports more CCes than described as mandatory
// for it's Device Type)
//
// @param: flirs1000
// FLiRS 1000 flag (hardware have to be based on FLiRS library to support it)
//
// @param: flirs250
// FLiRS 250 flag (hardware have to be based on FLiRS library to support it)
//
// @param: generic_class
// Generic Device Type
//
// @param: specific_class
// Specific Device Type
//
// @param: nif_size
// Size of the new NIF
//
// @param: nif
// New NIF
//
// @param: successCallback
// @default: NULL
// Custom function to be called on function success
// NULL if callback is not needed
//
// @param: failureCallback
// @default: NULL
// Custom function to be called on function failure
// NULL if callback is not needed
//
// @param: callbackArg
// Custom argument to be passed to custom function to be called on function
// success or failure
//
znet_error_t ICACHE_FLASH_ATTR znet_fc_serial_api_application_node_info( bool security,
                                                                         ZFunction_Callback Callback,
                                                                         void* callbackArg );

// Start WatchDog
//
// @param: znet
// ZNet object instance
//
// @param: successCallback
// @default: NULL
// Custom function to be called on function success
// NULL if callback is not needed
//
// @param: failureCallback
// @default: NULL
// Custom function to be called on function failure
// NULL if callback is not needed
//
// @param: callbackArg
// Custom argument to be passed to custom function to be called on function
// success or failure
//
// znet_error_t ICACHE_FLASH_ATTR znet_fc_watchdog_start( ZFunction_Callback Callback, void* callbackArg );

// Stop WatchDog
//
// @param: znet
// ZNet object instance
//
// @param: successCallback
// @default: NULL
// Custom function to be called on function success
// NULL if callback is not needed
//
// @param: failureCallback
// @default: NULL
// Custom function to be called on function failure
// NULL if callback is not needed
//
// @param: callbackArg
// Custom argument to be passed to custom function to be called on function
// success or failure
//
// znet_error_t ICACHE_FLASH_ATTR znet_fc_watchdog_stop( ZFunction_Callback Callback, void* callbackArg );

// Request Home Id and controlle_r Node Id
//
// @param: znet
// ZNet object instance
//
// @param: successCallback
// @default: NULL
// Custom function to be called on function success
// NULL if callback is not needed
//
// @param: failureCallback
// @default: NULL
// Custom function to be called on function failure
// NULL if callback is not needed
//
// @param: callbackArg
// Custom argument to be passed to custom function to be called on function
// success or failure
//
znet_error_t ICACHE_FLASH_ATTR znet_fc_get_home_id( ZFunction_Callback Callback, void* callbackArg );

// Request Home Id and controlle_r Node Id
//
// @param: znet
// ZNet object instance
//
// @param: successCallback
// @default: NULL
// Custom function to be called on function success
// NULL if callback is not needed
//
// @param: failureCallback
// @default: NULL
// Custom function to be called on function failure
// NULL if callback is not needed
//
// @param: callbackArg
// Custom argument to be passed to custom function to be called on function
// success or failure
//
znet_error_t ICACHE_FLASH_ATTR znet_fc_store_home_id( uint8_t* home_id,
                                                      uint8_t bNodeID,
                                                      ZFunction_Callback Callback,
                                                      void* callbackArg );

// Request controlle_r capabilities (primary role, SUC/SIS availability)
//
// @param: znet
// ZNet object instance
//
// @param: successCallback
// @default: NULL
// Custom function to be called on function success
// NULL if callback is not needed
//
// @param: failureCallback
// @default: NULL
// Custom function to be called on function failure
// NULL if callback is not needed
//
// @param: callbackArg
// Custom argument to be passed to custom function to be called on function
// success or failure
//
// znet_error_t ICACHE_FLASH_ATTR znet_fc_get_controller_capabilities(
// ZFunction_Callback Callback, void*
// callbackArg );

// Request controlle_r hardware version
//
// @param: znet
// ZNet object instance
//
// @param: successCallback
// @default: NULL
// Custom function to be called on function success
// NULL if callback is not needed
//
// @param: failureCallback
// @default: NULL
// Custom function to be called on function failure
// NULL if callback is not needed
//
// @param: callbackArg
// Custom argument to be passed to custom function to be called on function
// success or failure
//
// znet_error_t ICACHE_FLASH_ATTR znet_fc_get_version( ZFunction_Callback Callback, void* callbackArg );

// Request SUC Node Id
//
// @param: znet
// ZNet object instance
//
// @param: successCallback
// @default: NULL
// Custom function to be called on function success
// NULL if callback is not needed
//
// @param: failureCallback
// @default: NULL
// Custom function to be called on function failure
// NULL if callback is not needed
//
// @param: callbackArg
// Custom argument to be passed to custom function to be called on function
// success or failure
//
znet_error_t ICACHE_FLASH_ATTR znet_fc_get_suc_node_id( ZFunction_Callback Callback, void* callbackArg );

// Enable or disable SUC/SIS functionality of the controlle_r
//
// @param: znet
// ZNet object instance
//
// @param: enable
// True to enable functionality, False to disable
//
// @param: sis
// True to enable SIS functionality, False to enable SUC only
//
// @param: successCallback
// @default: NULL
// Custom function to be called on function success
// NULL if callback is not needed
//
// @param: failureCallback
// @default: NULL
// Custom function to be called on function failure
// NULL if callback is not needed
//
// @param: callbackArg
// Custom argument to be passed to custom function to be called on function
// success or failure
//
// znet_error_t ICACHE_FLASH_ATTR znet_fc_enable_suc( bool enable, bool sis,
// ZFunction_Callback Callback, void*
// callbackArg );

// Assign new SUC/SIS or disable existing
//
// @param: znet
// ZNet object instance
//
// @param: node_id
// Node Id to be assigned/disabled as SUC/SIS
//
// @param: enable
// True to enable, False to disable
//
// @param: sis
// True to assign SIS role, False to enable SUC role only
//
// @param: successCallback
// @default: NULL
// Custom function to be called on function success
// NULL if callback is not needed
//
// @param: failureCallback
// @default: NULL
// Custom function to be called on function failure
// NULL if callback is not needed
//
// @param: callbackArg
// Custom argument to be passed to custom function to be called on function
// success or failure
//
// znet_error_t ICACHE_FLASH_ATTR znet_fc_set_suc_node_id( uint8_t node_id,
// bool enable, bool sis, ZFunction_Callback
// Callback, void* callbackArg );
znet_error_t ICACHE_FLASH_ATTR znet_fc_set_suc_node_id( uint8_t node_id,
                                                        ZFunction_Callback Callback,
                                                        void* callbackArg );

// Read single byte from EEPROM
//
// @param: znet
// ZNet object instance
//
// @param: offset
// Offset in application memory in EEPROM
//
// @param: successCallback
// @default: NULL
// Custom function to be called on function success
// NULL if callback is not needed
//
// @param: failureCallback
// @default: NULL
// Custom function to be called on function failure
// NULL if callback is not needed
//
// @param: callbackArg
// Custom argument to be passed to custom function to be called on function
// success or failure
//
// znet_error_t ICACHE_FLASH_ATTR znet_fc_memory_get_byte( unsigned short
// offset, ZFunction_Callback Callback, void*
// callbackArg );

// Read multiple bytes from EEPROM
//
// @param: znet
// ZNet object instance
//
// @param: offset
// Offset in application memory in EEPROM
//
// @param: length
// Number of byte to be read
//
// @param: successCallback
// @default: NULL
// Custom function to be called on function success
// NULL if callback is not needed
//
// @param: failureCallback
// @default: NULL
// Custom function to be called on function failure
// NULL if callback is not needed
//
// @param: callbackArg
// Custom argument to be passed to custom function to be called on function
// success or failure
//
// znet_error_t ICACHE_FLASH_ATTR znet_fc_memory_get_buffer( unsigned short
// offset, uint8_t length, ZFunction_Callback
// Callback, void* callbackArg );

// Write single byte to EEPROM
//
// @param: znet
// ZNet object instance
//
// @param: offset
// Offset in application memory in EEPROM
//
// @param: data
// Byte to be written
//
// @param: successCallback
// @default: NULL
// Custom function to be called on function success
// NULL if callback is not needed
//
// @param: failureCallback
// @default: NULL
// Custom function to be called on function failure
// NULL if callback is not needed
//
// @param: callbackArg
// Custom argument to be passed to custom function to be called on function
// success or failure
//
// znet_error_t ICACHE_FLASH_ATTR znet_fc_memory_put_byte( unsigned short
// offset, uint8_t data, ZFunction_Callback
// Callback, void* callbackArg );

// Write multiple bytes to EEPROM
//
// @param: znet
// ZNet object instance
//
// @param: offset
// Offset in application memory in EEPROM
//
// @param: length
// Length of byte sequence to be written
//
// @param: data
// Bytes to be written
//
// @param: successCallback
// @default: NULL
// Custom function to be called on function success
// NULL if callback is not needed
//
// @param: failureCallback
// @default: NULL
// Custom function to be called on function failure
// NULL if callback is not needed
//
// @param: callbackArg
// Custom argument to be passed to custom function to be called on function
// success or failure
//
// znet_error_t ICACHE_FLASH_ATTR znet_fc_memory_put_buffer( unsigned short
// offset, uint8_t length, const uint8_t* data,
// ZFunction_Callback Callback, void* callbackArg );

// Checks if node is failed
//
// @param: znet
// ZNet object instance
//
// @param: node_id
// Node Id to be checked
//
// @param: successCallback
// @default: NULL
// Custom function to be called on function success
// NULL if callback is not needed
//
// @param: failureCallback
// @default: NULL
// Custom function to be called on function failure
// NULL if callback is not needed
//
// @param: callbackArg
// Custom argument to be passed to custom function to be called on function
// success or failure
//
znet_error_t ICACHE_FLASH_ATTR znet_fc_is_failed_node( uint8_t node_id,
                                                       ZFunction_Callback Callback,
                                                       void* callbackArg );

// Abort send data
// Note that this function works unpredictably in multi callback environment !
//
// @param: znet
// ZNet object instance
//
// @param: successCallback
// @default: NULL
// Custom function to be called on function success
// NULL if callback is not needed
//
// @param: failureCallback
// @default: NULL
// Custom function to be called on function failure
// NULL if callback is not needed
//
// @param: callbackArg
// Custom argument to be passed to custom function to be called on function
// success or failure
//
// znet_error_t ICACHE_FLASH_ATTR znet_fc_send_data_abort(
// ZFunction_Callback Callback, void*
// callbackArg );

// Soft reset
// Restarts Z-Wave chip
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
znet_error_t ICACHE_FLASH_ATTR znet_fc_serial_api_soft_reset( ZFunction_Callback Callback, void* callbackArg );

// Send data
// Packets are sent in AUTO_ROUTE mode with EXPLRER_FRAME enabled for listening
// devices (ignored if not supported by the hardware [based on 5.0x branch])
//
// @param: znet
// ZNet object instance
//
// @param: node_id
// Destination Node Id (NODE_BROADCAST to send non-routed broadcast packet)
//
// @param: length
// Packet size
//
// @param: data
// Paket payload
//
// @param: description
// @default: NULL
// Packet description for queu_e inspector and logging
//
// @param: successCallback
// @default: NULL
// Custom function to be called on function success
// NULL if callback is not needed
//
// @param: failureCallback
// @default: NULL
// Custom function to be called on function failure
// NULL if callback is not needed
//
// @param: callbackArg
// Custom argument to be passed to custom function to be called on function
// success or failure
//
typedef znet_error_t( ICACHE_FLASH_ATTR* znet_fc_send_data_t )( uint8_t node_id,
                                                                uint8_t channel_id,
                                                                uint8_t length,
                                                                const uint8_t* data,
                                                                ZFunction_Callback Callback,
                                                                void* callbackArg,
                                                                encap_type_t encap_type );

extern znet_fc_send_data_t znet_fc_send_data;

// Send data multicast
// Transmit the data buffer to a list of Z-Wave Nodes
//
// @param: numberNodes
// Number the destination node
//
// @param: pNodeIDList
// List of destination node ID's
//
// @param: length
// Packet size
//
// @param: data
// Paket payload
//
// @param: Callback
// @default: NULL
// Custom function to be called on function success or failure
//
// @param: callbackArg
// Custom argument to be passed to custom function to be called on function
// success or failure
//
znet_error_t ICACHE_FLASH_ATTR znet_fc_send_data_multi( uint8_t nodeQuantity,
                                                        const uint8_t* listNodeId,
                                                        uint8_t length,
                                                        const uint8_t* data,
                                                        bool sendSingleCasts,
                                                        ZFunction_Callback Callback,
                                                        void* callbackArg );

// Get node protocol info
//
// @param: znet
// ZNet object instance
//
// @param: node_id
// Node Id of the device in question
//
// @param: successCallback
// @default: NULL
// Custom function to be called on function success
// NULL if callback is not needed
//
// @param: failureCallback
// @default: NULL
// Custom function to be called on function failure
// NULL if callback is not needed
//
// @param: callbackArg
// Custom argument to be passed to custom function to be called on function
// success or failure
//
znet_error_t ICACHE_FLASH_ATTR znet_fc_get_node_protocol_info( uint8_t node_id,
                                                               ZFunction_Callback Callback,
                                                               void* callbackArg );

// Get routing table line
//
// @param: znet
// ZNet object instance
//
// @param: node_id
// Node Id of the device in question
//
// @param: remove_bad
// @default: FALSE
// Exclude failed nodes from the listing
//
// @param: remove_repeaters
// @default: FALSE
// Exclude repeater nodes from the listing
//
// @param: successCallback
// @default: NULL
// Custom function to be called on function success
// NULL if callback is not needed
//
// @param: failureCallback
// @default: NULL
// Custom function to be called on function failure
// NULL if callback is not needed
//
// @param: callbackArg
// Custom argument to be passed to custom function to be called on function
// success or failure
//
// znet_error_t ICACHE_FLASH_ATTR znet_fc_get_routing_table_line( uint8_t
// node_id, bool remove_bad, bool remove_repeaters,
// ZFunction_Callback Callback, void* callbackArg );

// Assign return route to specified node
// Get Serial API capabilities
//
// @param: znet
// ZNet object instance
//
// @param: node_id
// Node Id of the device that have to store new route
//
// @param: dest_id
// Destination Node Id of the route
//
// @param: successCallback
// @default: NULL
// Custom function to be called on function success
// NULL if callback is not needed
//
// @param: failureCallback
// @default: NULL
// Custom function to be called on function failure
// NULL if callback is not needed
//
// @param: callbackArg
// Custom argument to be passed to custom function to be called on function
// success or failure
//
// znet_error_t ICACHE_FLASH_ATTR znet_fc_assign_return_route( uint8_t
// node_id, uint8_t dest_id, ZFunction_Callback
// Callback, void* callbackArg );

// Assign return route to SUC
//
// @param: znet
// ZNet object instance
//
// @param: node_id
// Node Id of the device that have to store route to SUC
//
// @param: successCallback
// @default: NULL
// Custom function to be called on function success
// NULL if callback is not needed
//
// @param: failureCallback
// @default: NULL
// Custom function to be called on function failure
// NULL if callback is not needed
//
// @param: callbackArg
// Custom argument to be passed to custom function to be called on function
// success or failure
//
// znet_error_t ICACHE_FLASH_ATTR znet_fc_assign_suc_return_route( uint8_t
// node_id, ZFunction_Callback Callback, void*
// callbackArg );

// Delete return route
//
// @param: znet
// ZNet object instance
//
// @param: node_id
// Node Id of the device that have to delete all assigned return routes
//
// @param: successCallback
// @default: NULL
// Custom function to be called on function success
// NULL if callback is not needed
//
// @param: failureCallback
// @default: NULL
// Custom function to be called on function failure
// NULL if callback is not needed
//
// @param: callbackArg
// Custom argument to be passed to custom function to be called on function
// success or failure
//
// znet_error_t ICACHE_FLASH_ATTR znet_fc_delete_return_route( uint8_t
// node_id, ZFunction_Callback Callback, void*
// callbackArg );

// Delete return route to SUC
//
// @param: znet
// ZNet object instance
//
// @param: node_id
// Node Id of the device that have to delete route to SUC
//
// @param: successCallback
// @default: NULL
// Custom function to be called on function success
// NULL if callback is not needed
//
// @param: failureCallback
// @default: NULL
// Custom function to be called on function failure
// NULL if callback is not needed
//
// @param: callbackArg
// Custom argument to be passed to custom function to be called on function
// success or failure
//
// znet_error_t ICACHE_FLASH_ATTR znet_fc_delete_suc_return_route( uint8_t
// node_id, ZFunction_Callback Callback, void*
// callbackArg );

// Reset the controlle_r
// Note: this function will delete ALL data from the Z-Wave chip and restore it
// to factory default ! Sticks based on 4.5x and 6.x SDKs will also generate a
// new Home Id.
//
// @param: znet
// ZNet object instance
//
// @param: successCallback
// @default: NULL
// Custom function to be called on function success
// NULL if callback is not needed
//
// @param: failureCallback
// @default: NULL
// Custom function to be called on function failure
// NULL if callback is not needed
//
// @param: callbackArg
// Custom argument to be passed to custom function to be called on function
// success or failure
//
znet_error_t ICACHE_FLASH_ATTR znet_fc_set_default( ZFunction_Callback Callback, void* callbackArg );

// Send SUC Node Id
// Informs portable and static controlle_rs about new or deleted SUC/SIS
//
// @param: znet
// ZNet object instance
//
// @param: node_id
// Node Id of the device that have to be informed about new or deleted SIC/SIS
//
// @param: successCallback
// @default: NULL
// Custom function to be called on function success
// NULL if callback is not needed
//
// @param: failureCallback
// @default: NULL
// Custom function to be called on function failure
// NULL if callback is not needed
//
// @param: callbackArg
// Custom argument to be passed to custom function to be called on function
// success or failure
//
znet_error_t ICACHE_FLASH_ATTR znet_fc_send_suc_node_id( uint8_t node_id,
                                                         ZFunction_Callback Callback,
                                                         void* callbackArg );

// Send NIF of the stick
//
// @param: znet
// ZNet object instance
//
// @param: node_id
// Destination Node Id (NODE_BROADCAST to send non-routed broadcast packet)
//
// @param: successCallback
// @default: NULL
// Custom function to be called on function success
// NULL if callback is not needed
//
// @param: failureCallback
// @default: NULL
// Custom function to be called on function failure
// NULL if callback is not needed
//
// @param: callbackArg
// Custom argument to be passed to custom function to be called on function
// success or failure
//
znet_error_t ICACHE_FLASH_ATTR znet_fc_send_node_information( bool startStop,
                                                              ZFunction_Callback Callback,
                                                              void* callbackArg );

// Request NIF of a device
//
// @param: znet
// ZNet object instance
//
// @param: node_id
// Node Id to be requested for a NIF
//
// @param: successCallback
// @default: NULL
// Custom function to be called on function success
// NULL if callback is not needed
//
// @param: failureCallback
// @default: NULL
// Custom function to be called on function failure
// NULL if callback is not needed
//
// @param: callbackArg
// Custom argument to be passed to custom function to be called on function
// success or failure
//
znet_error_t ICACHE_FLASH_ATTR znet_fc_request_node_information( uint8_t node_id,
                                                                 ZFunction_Callback Callback,
                                                                 void* callbackArg );

// Request node neighbor for devices
//
// @param: znet
// ZNet object instance
//
// @param: node_id
// Node Id to be requested for a NIF
//
// @param: successCallback
// @default: NULL
// Custom function to be called on function success
// NULL if callback is not needed
//
// @param: failureCallback
// @default: NULL
// Custom function to be called on function failure
// NULL if callback is not needed
//
// @param: callbackArg
// Custom argument to be passed to custom function to be called on function
// success or failure
//
znet_error_t ICACHE_FLASH_ATTR znet_fc_request_node_neighbor_update( uint8_t node_id,
                                                                     ZFunction_Callback Callback,
                                                                     void* callbackArg );
// Remove failed node from network
// Before removing SDK will check that the device is really unreachable
//
// @param: znet
// ZNet object instance
//
// @param: node_id
// Node Id to be removed from network
//
// @param: successCallback
// @default: NULL
// Custom function to be called on function success
// NULL if callback is not needed
//
// @param: failureCallback
// @default: NULL
// Custom function to be called on function failure
// NULL if callback is not needed
//
// @param: callbackArg
// Custom argument to be passed to custom function to be called on function
// success or failure
//
znet_error_t ICACHE_FLASH_ATTR znet_fc_remove_failed_node( uint8_t node_id,
                                                           ZFunction_Callback Callback,
                                                           void* callbackArg );

// Replace failed node with a new one
// Be ware that a failed node can be replaced by a node of another type. This
// can lead to probles! Always request device NIF and force re-interview after
// successful replace process.
//
// @param: znet
// ZNet object instance
//
// @param: node_id
// Node Id to be replaced by new one
//
// @param: successCallback
// @default: NULL
// Custom function to be called on function success
// NULL if callback is not needed
//
// @param: failureCallback
// @default: NULL
// Custom function to be called on function failure
// NULL if callback is not needed
//
// @param: callbackArg
// Custom argument to be passed to custom function to be called on function
// success or failure
//
znet_error_t ICACHE_FLASH_ATTR znet_fc_replace_failed_node( uint8_t node_id,
                                                            ZFunction_Callback Callback,
                                                            void* callbackArg );

// Request network topology update from SUC/SIS
// Note that this process may also fail due more than 64 changes from last sync.
// In this case a re-inclusion of the controlle_r (self) is required.
//
// @param: znet
// ZNet object instance
//
// @param: successCallback
// @default: NULL
// Custom function to be called on function success
// NULL if callback is not needed
//
// @param: failureCallback
// @default: NULL
// Custom function to be called on function failure
// NULL if callback is not needed
//
// @param: callbackArg
// Custom argument to be passed to custom function to be called on function
// success or failure
//
znet_error_t ICACHE_FLASH_ATTR znet_fc_request_network_update( ZFunction_Callback Callback, void* callbackArg );

// Request the most recent background RSSI levels detected.
//
// @param: Callback
// @default: NULL
// Custom function to be called on function finish
// NULL if callback is not needed
//
// @param: callbackArg
// Custom argument to be passed to custom function to be called on function
// success or failure
znet_error_t ICACHE_FLASH_ATTR znet_fc_get_background_RSSI( ZFunction_Callback Callback, void* callbackArg );

// Request neighbours update for specific node
//
// @param: znet
// ZNet object instance
//
// @param: node_id
// Node Id to be requested for it's neighbours
//
// @param: successCallback
// @default: NULL
// Custom function to be called on function success
// NULL if callback is not needed
//
// @param: failureCallback
// @default: NULL
// Custom function to be called on function failure
// NULL if callback is not needed
//
// @param: callbackArg
// Custom argument to be passed to custom function to be called on function
// success or failure
//
// znet_error_t ICACHE_FLASH_ATTR znet_fc_request_node_neighbour_update(
// uint8_t node_id, ZFunction_Callback
// Callback, void* callbackArg );

// Set/stop Learn mode
//
// @param: znet
// ZNet object instance
//
// @param: startStop
// Start Learn mode if True, stop if False
//
// @param: successCallback
// @default: NULL
// Custom function to be called on function success
// NULL if callback is not needed
//
// @param: failureCallback
// @default: NULL
// Custom function to be called on function failure
// NULL if callback is not needed
//
// @param: callbackArg
// Custom argument to be passed to custom function to be called on function
// success or failure
//
znet_error_t ICACHE_FLASH_ATTR znet_fc_set_learn_mode( uint8_t learnMode,
                                                       ZFunction_Callback Callback,
                                                       void* callbackArg );

// Start/stop Inclusion of a new node
// Available on primary and inclusion controlle_rs
//
// @param: znet
// ZNet object instance
//
// @param: mode
// Function mode:
//    ZNET_MODE_ADD_NODE_STOP - Abort an active inclusion process.
//    ZNET_MODE_ADD_NODE_START - Initiate inclusion of new node of any type.
//    ZNET_MODE_ADD_NODE_SMART_START - Activate Smart Start listen mode.
//    ZNET_MODE_ADD_NODE_DSK - Initiate the SMART START inclusion of a specific SMART START node.
//
// @param: Callback
// @default: NULL
// Custom function to be called on function success or failure
// NULL if callback is not needed
//
// @param: callbackArg
// Custom argument to be passed to custom function to be called on function
// success or failure
//
znet_error_t ICACHE_FLASH_ATTR znet_fc_add_node_to_network( uint8_t mode,
                                                            ZFunction_Callback Callback,
                                                            void* callbackArg );

// The AddNodeDskToNetwork function MAY be called by a primary controller application to invoke the
// inclusion (add) of new nodes in a Z-Wave network.
//
// @param: znet
// ZNet object instance
//
// @param: DSK
// DSK
//
// @param: Callback
// @default: NULL
// Custom function to be called on function success or failure
// NULL if callback is not needed
//
znet_error_t ICACHE_FLASH_ATTR znet_fc_add_node_dsk_to_network( const uint8_t* dsk,
                                                                ZFunction_Callback Callback,
                                                                void* callbackArg );

// Start/stop exclusion of a node
// Note that this function can be used to exclude a device from previous network
// before including in ours. Available on primary and inclusion controlle_rs
//
// @param: znet
// ZNet object instance
//
// @param: startStop
// Start exclusion mode if True, stop if False
//
// @param: highPower
// @default: FALSE
// Use full power during this operation if True. On False use low power mode.
//
// @param: successCallback
// @default: NULL
// Custom function to be called on function success
// NULL if callback is not needed
//
// @param: failureCallback
// @default: NULL
// Custom function to be called on function failure
// NULL if callback is not needed
//
// @param: callbackArg
// Custom argument to be passed to custom function to be called on function
// success or failure
//
znet_error_t ICACHE_FLASH_ATTR znet_fc_remove_node_from_network( bool startStop,
                                                                 ZFunction_Callback Callback,
                                                                 void* callbackArg );

// Set new primary controlle_r (also known as Controller Shift)
// Same as Inclusion, but the newly included device will get the role of
// primary. Available only on primary controlle_r.
//
// @param: znet
// ZNet object instance
//
// @param: startStop
// Start controlle_r shift mode if True, stop if False
//
// @param: highPower
// @default: TRUE
// Use full power during this operation if True. On False use low power mode.
//
// @param: successCallback
// @default: NULL
// Custom function to be called on function success
// NULL if callback is not needed
//
// @param: failureCallback
// @default: NULL
// Custom function to be called on function failure
// NULL if callback is not needed
//
// @param: callbackArg
// Custom argument to be passed to custom function to be called on function
// success or failure
//
// znet_error_t ICACHE_FLASH_ATTR znet_fc_controller_change( bool startStop,
// bool highPower, ZFunction_Callback Callback,
// void* callbackArg );

// Create new primary controlle_r by SUC controlle_r
// Same as Inclusion, but the newly included device will get the role of
// primary. Available only on SUC. Be careful not to create two primary
// controlle_rs! This can lead to network malfunction!
//
// @param: znet
// ZNet object instance
//
// @param: startStop
// Start create new primary mode if True, stop if False
//
// @param: successCallback
// @default: NULL
// Custom function to be called on function success
// NULL if callback is not needed
//
// @param: failureCallback
// @default: NULL
// Custom function to be called on function failure
// NULL if callback is not needed
//
// @param: callbackArg
// Custom argument to be passed to custom function to be called on function
// success or failure
//
// znet_error_t ICACHE_FLASH_ATTR znet_fc_create_new_primary( bool startStop,
// ZFunction_Callback Callback, void*
// callbackArg );

// Change Z-Wave.Me Z-Stick 4 frequency
// This function is specific for Z-Wave.Me hardware
//
// @param: znet
// ZNet object instance
//
// @param: freq
// 0 for EU, 1 for RU
//
// @param: successCallback
// @default: NULL
// Custom function to be called on function success
// NULL if callback is not needed
//
// @param: failureCallback
// @default: NULL
// Custom function to be called on function failure
// NULL if callback is not needed
//
// @param: callbackArg
// Custom argument to be passed to custom function to be called on function
// success or failure
//
// znet_error_t ICACHE_FLASH_ATTR znet_fc_zme_freq_change( uint8_t freq,
// ZFunction_Callback Callback, void*
// callbackArg );

znet_error_t ICACHE_FLASH_ATTR znet_fc_send_test_frame( uint8_t node_id,
                                                        uint8_t powerLevel,
                                                        ZFunction_Callback Callback,
                                                        void* callbackArg );

znet_error_t ICACHE_FLASH_ATTR znet_fc_library_version( ZFunction_Callback Callback, void* callbackArg );

znet_error_t ICACHE_FLASH_ATTR znet_fc_type_library( ZFunction_Callback Callback, void* callbackArg );

znet_error_t ICACHE_FLASH_ATTR znet_fc_power_level_get( ZFunction_Callback Callback, void* callbackArg );

znet_error_t ICACHE_FLASH_ATTR znet_fc_power_level_set( uint8_t powerlevel,
                                                        ZFunction_Callback Callback,
                                                        void* callbackArg );

znet_error_t ICACHE_FLASH_ATTR znet_fc_assign_suc_return_route( uint8_t src_node,
                                                                ZFunction_Callback Callback,
                                                                void* callbackArg );

// @param: znet
// ZNet object instance
//
// @param: Callback
// @default: NULL
// Custom function to be called on function success or failure
// NULL if callback is not needed
//
// @param: callbackArg
// Custom argument to be passed to custom function to be called on function
// success or failure
//
znet_error_t ICACHE_FLASH_ATTR znet_fc_explore_request_inclusion( ZFunction_Callback Callback, void* callbackArg );

znet_error_t ICACHE_FLASH_ATTR znet_fc_set_routing_max( uint8_t routingMax,
                                                        ZFunction_Callback Callback,
                                                        void* callbackArg );
// @param: znet
// ZNet object instance
//
// @param: offset
// Offset of the NVR value as given by the NVR_FLASH_STRUCT
//
// @param: length
// Length of the NVR value that should be read
//
// @param: Callback
// @default: NULL
// Custom function to be called on function success or failure
// NULL if callback is not needed
//
// @param: callbackArg
// Custom argument to be passed to custom function to be called on function
// success or failure
//
znet_error_t ICACHE_FLASH_ATTR znet_fc_nvr_get_value( uint8_t offset,
                                                      uint8_t length,
                                                      ZFunction_Callback Callback,
                                                      void* callbackArg );

// This function sets the “Listen Before Talk” threshold that controlles at what RSSI level a Z-Wave node
// will refuse to transmit because of noise.
//
// @param: chanel_id
//  Channel number the Threshold should be set for. Valid channel numbers are 0,1 and 2
//
// @param: threshold
//   The threshold the RSSI should use. Valid threshold range is from 34(dec) to 78(dec).
//
// @param: Callback
// @default: NULL
// Custom function to be called on function finish
// NULL if callback is not needed
//
// @param: callbackArg
// Custom argument to be passed to custom function to be called on function
// success or failure
//
znet_error_t ICACHE_FLASH_ATTR znet_fc_set_threshold( uint8_t chanel_id,
                                                      uint8_t threshold,
                                                      ZFunction_Callback Callback,
                                                      void* callbackArg );

// @param: cmd
// Operation nvm_backup_restore {OPEN, READ, WRITE, CLOSE}
//
// @param: length
// Length Read/Write bytes. Max 64
//
// @param: offsetM
// Offset MSB
//
// @param: offsetL
// Offset LSB
//
// @param: buff
// Buffer for write
//
// @param: Callback
// @default: NULL
// Custom function to be called on function success or failure
// NULL if callback is not needed
//
// @param: callbackArg
// Return structure
// uint8 Return_Value {OK - 0x00, Error - 0x01, End of File - 0xff}
// uint8 Length - actual length of read/write buffer
// uint16 offset (MSB,LSB) - offset in the NVM when the write or read was done / When operation OPEN - return length NVM
// uint8 *buffer - read buffer containing the data that was read from NVM when backing up NVM
//
znet_error_t ICACHE_FLASH_ATTR znet_fc_nvm_backup_restore( uint8_t cmd,
                                                           uint8_t length,
                                                           uint8_t offsetM,
                                                           uint8_t offsetL,
                                                           const uint8_t* buff,
                                                           ZFunction_Callback Callback,
                                                           void* callbackArg );

#endif  // #ifndef __ZNET_LIB_FC_H__
