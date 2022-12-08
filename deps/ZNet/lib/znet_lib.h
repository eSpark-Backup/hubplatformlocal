
#ifndef __ZNET_LIB_H__
#define __ZNET_LIB_H__

//////////////////////////////////////////////////////////////////////////////

#include "znet.h"

//////////////////////////////////////////////////////////////////////////////

#define MATH_POW10_LEN 10
extern const int Math_pow10[MATH_POW10_LEN];

// Types of Device change events
typedef enum _ZWDeviceChangeType {
    DeviceAdded = 0x0001,
    DeviceRemoved = 0x0002,
    InstanceAdded = 0x0004,
    InstanceRemoved = 0x0008,
    CommandAdded = 0x0010,
    CommandRemoved = 0x0020
    //    ZDDXSaved       = 0x0100, // this callback notifies on ZDDX data
    //    change (to allow main program to purge buffers to disk/flash). For
    //    this event node_id = instance_id = command_id = 0
} ZWDeviceChangeTypes_e;
typedef int ZWDeviceChangeType;
// Use this type to store Device change event type
// Definition of device change callback function
typedef void ( *ZDeviceCallback )( const ZWDeviceChangeType type,
                                   uint8_t node_id,
                                   uint8_t instance_id,
                                   uint8_t command_id,
                                   void* arg );

// Initialize a ZNet object
//
// @param pznet:
// pointer that gets a ZNet object
// Note! ZNet structure must be filled with zeros first!
//
// @param port:
// name of port to connect to.
// either serial port (COM1 or /dev/ttyUSB0),
//
// @param level
// logging level
//
/*
ZWError ICACHE_FLASH_ATTR ZNet_Init( ZNet znet
#if ( defined( _WINDOWS ) || defined(__APPLE__) || defined( __linux__ ) )
                            , const char* serialPortName, Handshakings_e
handshaking, BaudRates_t baudRate, DataBits_e dataBits, Parities_e parity,
StopBits_e stopBits #else , SerialPortDevice_t device #endif
                 );
*/

// Attach callback function from Device change event
// Supress add of duplicate address of function callback
//
// @param znet:
// Z-Way object instance
//
// @param mask:
// Mask of which events to watch
//
// @param callback:
// Function to be attached to change event
//
// @param arg:
// Custom argument for callback function
//
ZWError ICACHE_FLASH_ATTR ZNet_AddCallback( ZWDeviceChangeType mask, ZDeviceCallback callback, void* arg );
// Attach callback function from Device change event
// Allows many callbac_ks with same address of callback function (but supress
// duplicate callbac_k/arg pair)
//
// @param znet:
// Z-Way object instance
//
// @param mask:
// Mask of which events to watch
//
// @param callback:
// Function to be attached to change event
//
// @param arg:
// Custom argument for callback function
//
ZWError ICACHE_FLASH_ATTR ZNet_AddCallbackEx( ZWDeviceChangeType mask, ZDeviceCallback callback, void* arg );
// Detach callback function from Device change event
// Delete all callbac_ks with same address of function callbac_k
//
// @param znet:
// Z-Way object instance
//
// @param callback:
// Function to be detached from change event
//
ZWError ICACHE_FLASH_ATTR ZNet_RemoveCallback( ZDeviceCallback callback );
// Detach callback function from Device change event
// Remove only once callback reffered as functio address and argument pair
//
// @param znet:
// Z-Way object instance
//
// @param callback:
// Function to be detached from change event
//
ZWError ICACHE_FLASH_ATTR ZNet_RemoveCallbackEx( ZDeviceCallback callback, void* arg );

// Start worker thread and open port
//
// @param znet:
// ZNet object instance
//
// @param termination_callback:
// function called on termintion of Z-Way worker thread (underlying device has
// gone) Syntax is (void) termination_callback(ZNet znet) NULL if callback is
// not needed Note, that the callback is called in Z-Way worker thread. Jobs are
// not processed anymore, but Data Holders are still available.
//
ZWError ICACHE_FLASH_ATTR ZNet_Start( ZTerminationCallback termination_callback );

// Discover Home ID, get network topology, version, capabilities etc.
// Function exits only then discovery finished (successfully or unsuccessfully)
//
// @param znet:
// ZNet object instance
//
ZWError ICACHE_FLASH_ATTR ZNet_Discover( ZNet znet );

// Stop processing of commands and terminate worker thread
//
// @param znet:
// ZNet object instance
//
ZWError ICACHE_FLASH_ATTR ZNet_Stop( ZNet znet );

// Save state, close all handles and deallocate a ZNet object
//
// @param pznet:
// pointer to ZNet object
// set to NULL on return
//
void ICACHE_FLASH_ATTR znet_terminate( ZNet* pznet );

// bool ZNet_DeInit( ZNet znet );

bool ICACHE_FLASH_ATTR ZNet_Free( ZNet znet );

// uint8_t ICACHE_FLASH_ATTR ZNet_FillNIFBuffer( const ZCommandClass const *
// nifCCes, uint8_t* buff, int buffSize );  ZDevice ICACHE_FLASH_ATTR
// ZNet_GetController( const ZNet znet );

void ICACHE_FLASH_ATTR ZNet_GetPendingNonces( const ZNet znet, uint8_t* sent_nonces );
// ZInstance ICACHE_FLASH_ATTR ZNet_GetInstance     ( const  uint8_t
// node_id, uint8_t instance_id );
void ICACHE_FLASH_ATTR ZNet_CallDeviceCB( ZWDeviceChangeType type,
                                          uint8_t node_id,
                                          uint8_t instance_id,
                                          uint8_t command_id );

void ICACHE_FLASH_ATTR ZNet_RemoveRequestNodeInfoJobs( const ZNet znet, bool success );

// finish devices initialization
ZWError ICACHE_FLASH_ATTR _znet_creation_finished( ZNet znet );

ZWError ICACHE_FLASH_ATTR znet_rediscover( ZNet znet );

// bool ICACHE_FLASH_ATTR ZNet_CancelDeviceJobs( const  const ZDevice
// device );

// bool ICACHE_FLASH_ATTR ZNet_AwakeDeviceJobs( const  const ZDevice
// device );

// bool ICACHE_FLASH_ATTR ZNet_ProcessSleepingDeviceJobs( const  const
// ZDevice device );

// Assigns a log file and logging level to a ZNet instance
//
// @param znet:
// ZNet object instance
//
// @param log:
// file object to log to
// may be either a real file (opened with fopen),
// or standard output device (stdout, stderr)
//
// @param level:
// minimal logging level
//
// ZWError ICACHE_FLASH_ATTR znet_set_log( FILE* log, ZWLogLevel
// level);

// Check if queue is empty (or has only jobs in state 'Done')
//
// @param znet:
// ZNet object instance
//
bool ICACHE_FLASH_ATTR znet_is_idle( ZNet znet );

// Send NoOperation to a node and wake up it's queue
//
// @param znet:
// ZNet object instance
//
// @param node_id:
// NodeId
//
ZWError ICACHE_FLASH_ATTR znet_device_send_nop( uint8_t node_id );

// Loads Device Description XML file for specified device
//
// @param znet:
// ZNet object instance
//
// @param node_id:
// NodeId
//
// @param file_name:
// XML file name
//
ZWError ICACHE_FLASH_ATTR znet_device_load_xml( const ZNet znet, uint8_t node_id, const char* file_name );

// ChooseXml enumerator
struct _ZGuessedProduct {
    int _score;
    char* _vendor;
    char* _product;
    char* _image_url;
    char* _file_name;
};
typedef struct _ZGuessedProduct* ZGuessedProduct;

// Returns a list of ZDDX files with match score for the desired devices.
// (NULL terminated array of ZGuessedProduct structures).
// The list is sorted in descending order.
// Full match is score = 10.
// Currently selected ZDDX record (if any) have 100 added to it's score.
//
// @param znet:
// ZNet object instance
//
// @param node_id:
// NodeId
//
ZGuessedProduct* ICACHE_FLASH_ATTR znet_device_guess( const ZNet znet, uint8_t node_id );

// Free memory allocated by znet_device_load_xml function.
//
// @param products
// Pointer to the array returned previously by znet_device_load_xml
//
void ICACHE_FLASH_ATTR znet_device_guess_free( ZGuessedProduct* products );

// Force queue wakeup for a device
//
// @param znet:
// ZNet object instance
//
// @param node_id:
// NodeId
//
void ICACHE_FLASH_ATTR znet_device_awake_queue( const ZNet znet, uint8_t node_id );

// Run Command Class Interview
//
// @param znet:
// Z-Way object instance
//
// @param device_id:
// Node Id
//
// @param instance_id:
// Instance Id
//
// @param cc_id:
// Command Class Id
//
ZWError ICACHE_FLASH_ATTR znet_command_interview( const ZNet znet,
                                                  uint8_t device_id,
                                                  uint8_t instance_id,
                                                  uint8_t cc_id );

// Force re-interview of the device
// Deletes Command Class and Instance structures,
// renders them from scratch based on NIF and mandatory Command Claasses and
// runs interview
//
// @param znet:
// Z-Way object instance
//
// @param device_id:
// Node Id
//
ZWError ICACHE_FLASH_ATTR znet_device_interview_force( const ZNet znet, uint8_t device_id );

// Check if device interview is done
//
// @param znet:
// Z-Way object instance
//
// @param device_id:
// Node Id
//
// @param instance_id:
// Instance Id
//
bool ICACHE_FLASH_ATTR znet_device_is_interview_done( const ZNet znet, uint8_t device_id );

// Assigns return route to a device
// Use this function instead of directly calling Function Class
// AssignReturnRoute since this function also handles controlle_r/slave
// difference
//
// @param znet:
// Z-Way object instance
//
// @param device_id:
// Node Id of the device saving the new route
//
// @param node_id:
// Node Id of the device to save the route to
//
ZWError ICACHE_FLASH_ATTR znet_device_assign_return_route( const ZNet znet, uint8_t device_id, uint8_t node_id );

// Deletes ALL return route in device
// Use this function instead of directly calling Function Class
// DeleteReturnRoute since this function also handles controlle_r/slave
// difference
//
// @param znet:
// Z-Way object instance
//
// @param device_id:
// Node Id of the device cleaning return routes
//
ZWError ICACHE_FLASH_ATTR znet_device_delete_return_route( const ZNet znet, uint8_t device_id );

// Assigns SUC return route to a device
// Use this function instead of directly calling Function Class
// AssignSUCReturnRoute since this function also handles controlle_r/slave
// difference
//
// @param znet:
// Z-Way object instance
//
// @param device_id:
// Node Id of the device saving the new route to SUC
//
ZWError ICACHE_FLASH_ATTR znet_device_assign_suc_return_route( const uint8_t device_id );

// Deletes SUC return route in device
// Use this function instead of directly calling Function Class
// DeleteSUCReturnRoute since this function also handles controlle_r/slave
// difference
//
// @param znet:
// Z-Way object instance
//
// @param device_id:
// Node Id of the device cleaning SUC return route
//
ZWError ICACHE_FLASH_ATTR znet_device_delete_suc_return_route( const uint8_t device_id );

// Set Static Update Controller (SUC) in the network and inform other devices
// about the assignment. Use this function instead of directly calling Function
// Class SetSUCNodeId since this function handles EnableSUC and
// SendSUCNodeId/AssignSUCReturnRoute calls
//
// @param znet:
// Z-Way object instance
//
// @param node_id:
// Node Id to be come the new SUC
//
ZWError ICACHE_FLASH_ATTR znet_controller_set_suc_node_id( uint8_t node_id );

// Set SUC ID Server (SIS) in the network and inform other devices about the
// assignment. Use this function instead of directly calling Function Class
// SetSUCNodeId since this function handles EnableSUC/EnableSIS and
// SendSUCNodeId/AssignSUCReturnRoute calls
//
// @param znet:
// Z-Way object instance
//
// @param node_id:
// Node Id to be come the new SIS
//
ZWError ICACHE_FLASH_ATTR znet_controller_set_sis_node_id( uint8_t node_id );

// Disable SUC/SIS in the network and inform other devices about the assignment.
// Use this function instead of directly calling Function Class SetSUCNodeId
// since this function handles DisableSUC and SendSUCNodeId/DeleteSUCReturnRoute
// calls
//
// @param znet:
// Z-Way object instance
//
// @param node_id:
// Node Id to revoke SUC/SIS role from
//
ZWError ICACHE_FLASH_ATTR znet_controller_disable_suc_node_id( uint8_t node_id );

// Set new primary controlle_r (also known as Controller Shift)
// Same as Inclusion, but the newly included device will get the role of
// primary. Available only on primary controlle_r.
//
// @param znet:
// Z-Way object instance
//
// @param startStop:
// Start create new primary mode if True, stop if False
//
ZWError ICACHE_FLASH_ATTR znet_controller_change( bool startStop );

// Start/stop Inclusion of a new node
// Available on primary and inclusion controlle_rs
//
// @param znet:
// Z-Way object instance
//
// @param startStop:
// Start inclusion mode if True, stop if False
//
ZWError ICACHE_FLASH_ATTR znet_controller_add_node_to_network( bool startStop );

// Start/stop exclusion of a node
// Note that this function can be used to exclude a device from previous network
// before including in ours. Available on primary and inclusion controlle_rs
//
// @param: znet
// Z-Way object instance
//
// @param: startStop
// Start exclusion mode if True, stop if False
//
ZWError ICACHE_FLASH_ATTR znet_controller_remove_node_from_network( bool startStop );

// Set/stop Learn mode
//
// @param: znet
// Z-Way object instance
//
// @param: startStop
// Start Learn mode if True, stop if False
//
ZWError ICACHE_FLASH_ATTR znet_controller_set_learn_mode( bool startStop );

// Reset the controlle_r
// Note: this function will delete ALL data from the Z-Wave chip and restore it
// to factory default ! Sticks based on 4.5x and 6.x SDKs will also generate a
// new Home Id.
//
// @param: znet
// Z-Way object instance
//
ZWError ICACHE_FLASH_ATTR znet_controller_set_default( ZNet znet );

// Saves controlle_r configuration, defaults and other needed files as tgz
// archive
//
// @param: znet
// Z-Way object instance
//
// @param: data
// Pointer to a variable that will receive an allocated data buffer
// It should be freed by caller when it is no longer needed
//
// @param: length
// Pointer to a variable that will receive data length
//
ZWError ICACHE_FLASH_ATTR znet_controller_config_save( uint8_t** data, int* length );

// Restores controlle_r configuration, defaults and other needed files from tgz
// archive Note: This function restores homeId, nodeId and information about
// nodes in network. The controlle_r would be PRIMARY after this restore!
//
// It has a lot of side effects! After this function consider:
// - run routing healing
// - change the SUC if it was not on the restored node
// - inform all nodes about new SUC/SIS
//
// @param: znet
// Z-Way object instance
//
// @param: data
// Data buffer with archive contents
//
// @param: length
// Data length
//
// @param: full
// Perform full restore (including home id and network topology)
//
ZWError ICACHE_FLASH_ATTR znet_controller_config_restore( const uint8_t* data, int length, bool full );

// Save all Z-Way data to the disc
// This function runs after each step of interview (configurable in
// Defaults.xml) and at Z-Wave library termination
//
// Run this function from time to time to have curent data for persistance.
//
// @param znet:
// Z-Way object instance
//
ZWError ICACHE_FLASH_ATTR zddx_save_to_xml( const uint8_t node_id, uint8_t step );

// Returns 0 terminated list of registered devices Node Id.
// NULL is returned if incorrect Z-Way object is given.
//
// @param znet:
// Z-Way object instance
//
/// ZWDevicesList ICACHE_FLASH_ATTR znet_devices_list(const ZNet znet);

// Free list of registered devices Node Id previously allocated by
// znet_devices_list.
//
// @param list:
// List to be freed
//
/// void ICACHE_FLASH_ATTR znet_devices_list_free(ZWDevicesList list);

// Returns 0 terminated list of registered instances Id for specified device.
// Note, that instance 0 is not included in the list (since it is zero
// terminated), but is ALWAYS present for a device NULL is returned if incorrect
// Z-Way object is given or device does not exist.
//
// @param znet:
// Z-Way object instance
//
// @param deviceId:
// Device in question
//
/// ZWInstancesList ICACHE_FLASH_ATTR znet_instances_list(const ZNet znet,
/// uint8_t deviceId);

// Free list of registered instances Id for specified device previously
// allocated by znet_instances_list.
//
// @param list:
// List to be freed
//
/// void ICACHE_FLASH_ATTR znet_instances_list_free(ZWInstancesList list);

// Returns 0 terminated list of registered Command Classes Id for specified
// instance of device NULL is returned if incorrect Z-Way object is given or
// device does not exist or instance does not exist.
//
// @param znet:
// Z-Way object instance
//
// @param deviceId:
// Device in question
//
// @param instanceId:
// Instance in question
//
/// ZWCommandClassesList ICACHE_FLASH_ATTR znet_command_classes_list(const ZNet
/// znet, uint8_t deviceId, uint8_t instanceId);

// Free list of registered Command Classes Id for specified instance of device
// previously allocated by znet_command_classes_list.
//
// @param list:
// List to be freed
//
/// void ICACHE_FLASH_ATTR znet_command_classes_list_free(ZWCommandClassesList
/// list);

// Returns if command class both exists for specified instance of device, and is
// rendered as supported
//
// @param znet:
// Z-Way object instance
//
// @param node_id:
// Device in question
//
// @param instance_id:
// Instance in question
//
// @param command_id:
// Command class in question
//
bool ICACHE_FLASH_ATTR znet_command_is_supported( const ZNet znet,
                                                  uint8_t node_id,
                                                  uint8_t instance_id,
                                                  uint8_t command_id );

extern uint8_t supervision_status;

void ZNet_CheckCallback( ZFunction func, ZFuncFailures_e reason, uint8_t* nodeId, uint8_t* endpoint );

#endif  // #ifndef __ZNET_LIB_H__
