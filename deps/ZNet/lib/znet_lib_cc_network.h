
#ifndef __ZNET_LIB_CC_NETWORK_H__
#define __ZNET_LIB_CC_NETWORK_H__

#include "znet.h"

#include "ZFunctionClass.h"

#include "znet_lib_cc_codes.h"

//////////////////////////////////////////////////////////////////////////////
// Command Class IP Configuration//

//----------------------------------------------------------------------------
// Ip Configuration Get Command
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
znet_error_t ICACHE_FLASH_ATTR znet_cc_ip_configuration_get( uint8_t node_id,
                                                             ZFunction_Callback Callback,
                                                             void* callbackArg );

//----------------------------------------------------------------------------
// Ip Configuration Release Command
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
znet_error_t ICACHE_FLASH_ATTR znet_cc_ip_configuration_release( uint8_t node_id,
                                                                 ZFunction_Callback Callback,
                                                                 void* callbackArg );

//----------------------------------------------------------------------------
// Ip Configuration Renew Command
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
znet_error_t ICACHE_FLASH_ATTR znet_cc_ip_configuration_renew( uint8_t node_id,
                                                               ZFunction_Callback Callback,
                                                               void* callbackArg );

//----------------------------------------------------------------------------
// Ip Configuration Set Command
//
// @param: node_id
// Destination Node Id
//
// @param: auto_dns
// Auto DNS (1 bits)
//
// @param: auto_ip
// Auto IP (1 bits)
//
// @param: ip_addr
// IP Address
//
// @param: subnet_mask
// Subnet Mask
//
// @param: gateway
// Gateway
//
// @param: dns1
// DNS1
//
// @param: dns2
// DNS2
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
znet_error_t ICACHE_FLASH_ATTR znet_cc_ip_configuration_set( uint8_t node_id,
                                                             bool auto_dns,
                                                             bool auto_ip,
                                                             uint32_t ip_addr,
                                                             uint32_t subnet_mask,
                                                             uint32_t gateway,
                                                             uint32_t dns1,
                                                             uint32_t dns2,
                                                             ZFunction_Callback Callback,
                                                             void* callbackArg );

//////////////////////////////////////////////////////////////////////////////
// Command Class Mailbox //

//----------------------------------------------------------------------------
// Mailbox Configuration Get Command
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
znet_error_t ICACHE_FLASH_ATTR znet_cc_mailbox_configuration_get( uint8_t node_id,
                                                                  ZFunction_Callback Callback,
                                                                  void* callbackArg );

//----------------------------------------------------------------------------
// Mailbox Configuration Set Command
//
// @param: node_id
// Destination Node Id
//
// @param: mode
// Mode
//
// @param: ipv6_dest_addr
// Forwarding Destination IPv6 Address
//
// @param: udp_port_number
// Forwarding UDP Port Number
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
znet_error_t ICACHE_FLASH_ATTR znet_cc_mailbox_configuration_set( uint8_t node_id,
                                                                  uint8_t mode,
                                                                  void* ipv6_dest_addr,
                                                                  uint16_t udp_port_number,
                                                                  ZFunction_Callback Callback,
                                                                  void* callbackArg );

//----------------------------------------------------------------------------
// Mailbox Queue Command
//
// @param: node_id
// Destination Node Id
//
// @param: seq_number
// Sequence Number
//
// @param: last
// Last (1 bits)
//
// @param: mode
// Mode (2 bits)
//
// @param: queue_handle
// Queue Handle
//
// @param: count_mailbox
//
// @param: mailbox_entry
// Mailbox Entry
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
znet_error_t ICACHE_FLASH_ATTR znet_cc_mailbox_queue( uint8_t node_id,
                                                      uint8_t seq_number,
                                                      bool last,
                                                      uint8_t mode,
                                                      uint8_t queue_handle,
                                                      uint8_t count_mailbox,
                                                      const uint8_t* mailbox_entry,
                                                      ZFunction_Callback Callback,
                                                      void* callbackArg );

//----------------------------------------------------------------------------
// Mailbox Wakeup Notification Command
//
// @param: node_id
// Destination Node Id
//
// @param: queue_handle
// Queue Handle
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
znet_error_t ICACHE_FLASH_ATTR znet_cc_mailbox_wakeup_notification( uint8_t node_id,
                                                                    uint8_t queue_handle,
                                                                    ZFunction_Callback Callback,
                                                                    void* callbackArg );

//----------------------------------------------------------------------------
// Mailbox Failing Node Command
//
// @param: node_id
// Destination Node Id
//
// @param: queue_handle
// Queue Handle
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
znet_error_t ICACHE_FLASH_ATTR znet_cc_mailbox_node_failing( uint8_t node_id,
                                                             uint8_t queue_handle,
                                                             ZFunction_Callback Callback,
                                                             void* callbackArg );

//////////////////////////////////////////////////////////////////////////////
// Command Class Network Management Basic//

//----------------------------------------------------------------------------
// Learn Mode Set Command
//
// @param: node_id
// Destination Node Id
//
// @param: seq_n
// Seq. No
//
// @param: mode
// Mode
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
znet_error_t ICACHE_FLASH_ATTR
znet_cc_learn_mode_set( uint8_t node_id, uint8_t seq_n, uint8_t mode, ZFunction_Callback Callback, void* callbackArg );

//----------------------------------------------------------------------------
// Learn Mode Set Status Command
//
// @param: node_id
// Destination Node Id
//
// @param: seq_n
// Seq. No
//
// @param: status
// Status
//
// @param: new_node_id
// New Node ID
//
// @param: next_data_available
// Available granted_keys and kex_fail_type
//
// @param: granted_keys
// Granted Keys
//
// @param: dsk
// DSK
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
znet_error_t ICACHE_FLASH_ATTR znet_cc_learn_mode_set_status( uint8_t node_id,
                                                              uint8_t seq_n,
                                                              uint8_t status,
                                                              uint8_t new_node_id,
                                                              bool next_data_available,
                                                              uint8_t granted_keys,
                                                              const uint8_t* dsk,
                                                              ZFunction_Callback Callback,
                                                              void* callbackArg );

//----------------------------------------------------------------------------
// Node Information Send Command
//
// @param: znet
// ZNet object instance
//
// @param: node_id
// Destination Node Id
//
// @param: seq_n
// Seq. No
//
// @param: dest_node_id
// Destination Node ID
//
// @param: tx_options
// tx Options
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
znet_error_t ICACHE_FLASH_ATTR znet_cc_node_information_send( uint8_t node_id,
                                                              uint8_t seq_n,
                                                              uint8_t dest_node_id,
                                                              uint8_t tx_options,
                                                              ZFunction_Callback Callback,
                                                              void* callbackArg );

//----------------------------------------------------------------------------
// Network Update Request Command
//
// @param: node_id
// Destination Node Id
//
// @param: seq_n
// Seq. No
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
znet_error_t ICACHE_FLASH_ATTR znet_cc_network_update_request( uint8_t node_id,
                                                               uint8_t seq_n,
                                                               ZFunction_Callback Callback,
                                                               void* callbackArg );

//----------------------------------------------------------------------------
// Network Update Request Status Command
//
// @param: node_id
// Destination Node Id
//
// @param: seq_n
// Seq. No
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
znet_error_t ICACHE_FLASH_ATTR znet_cc_network_update_request_status( uint8_t node_id,
                                                                      uint8_t seq_n,
                                                                      uint8_t status,
                                                                      ZFunction_Callback Callback,
                                                                      void* callbackArg );

//----------------------------------------------------------------------------
// Default Set Command
//
// @param: node_id
// Destination Node Id
//
// @param: seq_n
// Seq. No
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
znet_error_t ICACHE_FLASH_ATTR znet_cc_default_set( uint8_t node_id,
                                                    uint8_t seq_n,
                                                    ZFunction_Callback Callback,
                                                    void* callbackArg );

//----------------------------------------------------------------------------
// Default Set Complete Command
//
// @param: node_id
// Destination Node Id
//
// @param: seq_n
// Seq. No
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
znet_error_t ICACHE_FLASH_ATTR znet_cc_default_set_complete( uint8_t node_id,
                                                             uint8_t seq_n,
                                                             uint8_t status,
                                                             ZFunction_Callback Callback,
                                                             void* callbackArg );

//----------------------------------------------------------------------------
// DSK Get Command
//
// @param: node_id
// Destination Node Id
//
// @param: seq_n
// Seq. No
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
znet_error_t ICACHE_FLASH_ATTR znet_cc_dsk_get( uint8_t node_id,
                                                uint8_t seq_n,
                                                ZFunction_Callback Callback,
                                                void* callbackArg );

//////////////////////////////////////////////////////////////////////////////
// Command Class Network Management Inclusion//

//----------------------------------------------------------------------------
// Failed Node Remove Command
//
// @param: node_id
// Destination Node Id
//
// @param: seq_n
// Seq. No
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
znet_error_t ICACHE_FLASH_ATTR znet_cc_failed_node_remove( uint8_t node_id,
                                                           uint8_t seq_n,
                                                           ZFunction_Callback Callback,
                                                           void* callbackArg );

//----------------------------------------------------------------------------
// Failed Node Remove Status Command
//
// @param: node_id
// Destination Node Id
//
// @param: seq_n
// Seq. No
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
znet_error_t ICACHE_FLASH_ATTR znet_cc_failed_node_remove_status( uint8_t node_id,
                                                                  uint8_t seq_n,
                                                                  uint8_t status,
                                                                  ZFunction_Callback Callback,
                                                                  void* callbackArg );

//----------------------------------------------------------------------------
// Node Add Command
//
// @param: node_id
// Destination Node Id
//
// @param: seq_n
// Seq. No
//
// @param: mode
// Mode
//
// @param: tx_options
// tx Options
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
znet_error_t ICACHE_FLASH_ATTR znet_cc_node_add( uint8_t node_id,
                                                 uint8_t seq_n,
                                                 uint8_t mode,
                                                 uint8_t tx_options,
                                                 ZFunction_Callback Callback,
                                                 void* callbackArg );

//----------------------------------------------------------------------------
// Node Add Status Command
//
// @param: node_id
// Destination Node Id
//
// @param: status
// Status
//
// @param: new_node_id
// New Node ID
//
// @param: node_info_lenght
// Node Info Length
//
// @param: capability
// Capability (7 bits)
//
// @param: listen
// Listening (1 bits)
//
// @param: security
// Security (7 bits)
//
// @param: opt
// Opt (1 bits)
//
// @param: basic
// Basic Device Class
//
// @param: generic
// Generic Device Class
//
// @param: specific
// Specific Device Class
//
// @param: non_secure_size
//
// @param: non_secure
// Non-Secure Command Class
//
// @param: mark
// Security MARK (16 bits)
//
// @param: secure_size
//
// @param: secure
// Secure Command Class
//
// @param: granted_keys
// Granted Keys
//
// @param: kex_fail_type
// KEX Fail Type
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
znet_error_t ICACHE_FLASH_ATTR znet_cc_node_add_status( uint8_t node_id,
                                                        uint8_t seq_n,
                                                        uint8_t status,
                                                        uint8_t new_node_id,
                                                        uint8_t node_info_lenght,
                                                        uint8_t capability,
                                                        bool listen,
                                                        uint8_t security,
                                                        bool opt,
                                                        uint8_t basic,
                                                        uint8_t generic,
                                                        uint8_t specific,
                                                        uint8_t non_secure_size,
                                                        const uint8_t* non_secure,
                                                        uint16_t mark,
                                                        uint8_t secure_size,
                                                        const uint8_t* secure,
                                                        uint8_t granted_keys,
                                                        uint8_t kex_fail_type,
                                                        ZFunction_Callback Callback,
                                                        void* callbackArg );

//----------------------------------------------------------------------------
// Node Remove Command
//
// @param: node_id
// Destination Node Id
//
// @param: seq_n
// Seq. No
//
// @param: mode
// Mode
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
znet_error_t ICACHE_FLASH_ATTR
znet_cc_node_remove( uint8_t node_id, uint8_t seq_n, uint8_t mode, ZFunction_Callback Callback, void* callbackArg );

//----------------------------------------------------------------------------
// Node Remove Status Command
//
// @param: node_id
// Destination Node Id
//
// @param: seq_n
// Seq. No
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
znet_error_t ICACHE_FLASH_ATTR znet_cc_node_remove_status( uint8_t node_id,
                                                           uint8_t seq_n,
                                                           uint8_t status,
                                                           ZFunction_Callback Callback,
                                                           void* callbackArg );

//----------------------------------------------------------------------------
// Failed Node Replace Command
//
// @param: node_id
// Destination Node Id
//
// @param: seq_n
// Seq. No
//
// @param: tx_options
// tx Options
//
// @param: mode
// Mode
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
znet_error_t ICACHE_FLASH_ATTR znet_cc_failed_node_replace( uint8_t node_id,
                                                            uint8_t seq_n,
                                                            uint8_t tx_options,
                                                            uint8_t mode,
                                                            ZFunction_Callback Callback,
                                                            void* callbackArg );

//----------------------------------------------------------------------------
// Failed Node Replace Status Command
//
// @param: node_id
// Destination Node Id
//
// @param: seq_n
// Seq. No
//
// @param: status
// Status
//
// @param: next_data_available
// Available granted_keys and kex_fail_type
//
// @param: granted_keys
// Granted Keys
//
// @param: kex_fail_type
// KEX Fail Type
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
znet_error_t ICACHE_FLASH_ATTR znet_cc_failed_node_replace_status( uint8_t node_id,
                                                                   uint8_t seq_n,
                                                                   uint8_t status,
                                                                   bool next_data_available,
                                                                   uint8_t granted_keys,
                                                                   uint8_t kex_fail_type,
                                                                   ZFunction_Callback Callback,
                                                                   void* callbackArg );

//----------------------------------------------------------------------------
// Node Neighbor Update Request Command
//
// @param: node_id
// Destination Node Id
//
// @param: seq_n
// Seq. No
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
znet_error_t ICACHE_FLASH_ATTR znet_cc_node_neighbor_update_request( uint8_t node_id,
                                                                     uint8_t seq_n,
                                                                     ZFunction_Callback Callback,
                                                                     void* callbackArg );

//----------------------------------------------------------------------------
// Node Neighbor Update Status Command
//
// @param: node_id
// Destination Node Id
//
// @param: seq_n
// Seq. No
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
znet_error_t ICACHE_FLASH_ATTR znet_cc_node_neighbor_update_status( uint8_t node_id,
                                                                    uint8_t seq_n,
                                                                    uint8_t status,
                                                                    ZFunction_Callback Callback,
                                                                    void* callbackArg );

//----------------------------------------------------------------------------
// Return Route Assign Command
//
// @param: node_id
// Destination Node Id
//
// @param: seq_n
// Seq. No
//
// @param: dest_node_id
// Destination Node ID
//
// @param: source_node_id
// Destination Node ID
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
znet_error_t ICACHE_FLASH_ATTR znet_cc_return_route_assign( uint8_t node_id,
                                                            uint8_t seq_n,
                                                            uint8_t dest_node_id,
                                                            uint8_t source_node_id,
                                                            ZFunction_Callback Callback,
                                                            void* callbackArg );

//----------------------------------------------------------------------------
// Return Route Assign Complete Command
//
// @param: node_id
// Destination Node Id
//
// @param: seq_n
// Seq. No
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
znet_error_t ICACHE_FLASH_ATTR znet_cc_return_route_assign_status( uint8_t node_id,
                                                                   uint8_t seq_n,
                                                                   uint8_t status,
                                                                   ZFunction_Callback Callback,
                                                                   void* callbackArg );

//----------------------------------------------------------------------------
// Return Route Delete Command
//
// @param: node_id
// Destination Node Id
//
// @param: seq_n
// Seq. No
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
znet_error_t ICACHE_FLASH_ATTR znet_cc_return_route_delete( uint8_t node_id,
                                                            uint8_t seq_n,
                                                            ZFunction_Callback Callback,
                                                            void* callbackArg );

//----------------------------------------------------------------------------
// Return Route Delete Complete Command
//
// @param: node_id
// Destination Node Id
//
// @param: seq_n
// Seq. No
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
znet_error_t ICACHE_FLASH_ATTR znet_cc_return_route_delete_complete( uint8_t node_id,
                                                                     uint8_t seq_n,
                                                                     uint8_t status,
                                                                     ZFunction_Callback Callback,
                                                                     void* callbackArg );

//----------------------------------------------------------------------------
// Node Add Keys Set Command
//
// @param: node_id
// Destination Node Id
//
// @param: seq_n
// Seq. No
//
// @param: accept
// Accept (1 bits)
//
// @param: authentication
// Request Client-Side Authentication (1 bits)
//
// @param: granted_keys
// Granted Keys
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
znet_error_t ICACHE_FLASH_ATTR znet_cc_node_add_keys_set( uint8_t node_id,
                                                          uint8_t seq_n,
                                                          bool accept,
                                                          bool authentication,
                                                          uint8_t granted_keys,
                                                          ZFunction_Callback Callback,
                                                          void* callbackArg );

//----------------------------------------------------------------------------
// Node Add DSK Set Command
//
// @param: node_id
// Destination Node Id
//
// @param: seq_n
// Seq. No
//
// @param: dsk_length
// Input DSK Length (4 bits)
//
// @param: accept
// Accept (1 bits)
//
// @param: input_dsk
// Input DSK
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
znet_error_t ICACHE_FLASH_ATTR znet_cc_node_add_dsk_set( uint8_t node_id,
                                                         uint8_t seq_n,
                                                         uint8_t dsk_length,
                                                         bool accept,
                                                         void* input_dsk,
                                                         ZFunction_Callback Callback,
                                                         void* callbackArg );

//////////////////////////////////////////////////////////////////////////////
// Command Class Network Management Ptimary//

//----------------------------------------------------------------------------
// Controller Change Command
//
// @param: node_id
// Destination Node Id
//
// @param: seq_n
// Seq. No
//
// @param: mode
// Mode
//
// @param: tx_options
// tx Options
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
znet_error_t ICACHE_FLASH_ATTR znet_cc_controller_change( uint8_t node_id,
                                                          uint8_t seq_n,
                                                          uint8_t mode,
                                                          uint8_t tx_options,
                                                          ZFunction_Callback Callback,
                                                          void* callbackArg );

//----------------------------------------------------------------------------
// Controller Change Status Command
//
// @param: node_id
// Destination Node Id
//
// @param: seq_n
// Seq. No
//
// @param: status
// Status
//
// @param: new_node_id
// New Node ID
//
// @param: listening
// Listening (1 bits)
//
// @param: capability
// Capability (7 bits)
//
// @param: opt
// Opt (1 bits)
//
// @param: security
// Security (7 bits)
//
// @param: basic_dev_class
// Basic Device Class
//
// @param: generic_dev_class
// Generic  Device Class
//
// @param: specific_dev_class
// Specific  Device Class
//
// @param: count_command_class
//
// @param: command_class
// Command Class
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
znet_error_t ICACHE_FLASH_ATTR znet_cc_controller_change_status( uint8_t node_id,
                                                                 uint8_t seq_n,
                                                                 uint8_t status,
                                                                 uint8_t new_node_id,
                                                                 uint8_t node_info_length,
                                                                 bool listening,
                                                                 uint8_t capability,
                                                                 bool opt,
                                                                 uint8_t security,
                                                                 uint8_t basic_dev_class,
                                                                 uint8_t generic_dev_class,
                                                                 uint8_t specific_dev_class,
                                                                 uint8_t count_command_class,
                                                                 const uint8_t* command_class,
                                                                 ZFunction_Callback Callback,
                                                                 void* callbackArg );

//////////////////////////////////////////////////////////////////////////////
// Command Class Network Managment Proxy//

//----------------------------------------------------------------------------
// Node Info Cached Get Command
//
// @param: node_id
// Destination Node Id
//
// @param: seq_n
// Seq. No
//
// @param: max_age
// Max Age (4 bits)
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
znet_error_t ICACHE_FLASH_ATTR znet_cc_node_info_cached_get( uint8_t node_id,
                                                             uint8_t seq_n,
                                                             uint8_t max_age,
                                                             ZFunction_Callback Callback,
                                                             void* callbackArg );

//----------------------------------------------------------------------------
// Node List Get Command
//
// @param: node_id
// Destination Node Id
//
// @param: seq_n
// Seq. No
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
znet_error_t ICACHE_FLASH_ATTR znet_cc_node_list_get( uint8_t node_id,
                                                      uint8_t seq_n,
                                                      ZFunction_Callback Callback,
                                                      void* callbackArg );

//////////////////////////////////////////////////////////////////////////////
// Command Class PowerLevel //

// Send PowerLevel Get
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
znet_error_t ICACHE_FLASH_ATTR znet_cc_powerlevel_get( uint8_t node_id,
                                                       ZFunction_Callback Callback,
                                                       void* callbackArg,
                                                       encap_type_t encap_type );

// Send PowerLevel Set
//
// @param: znet
// ZNet object instance
//
// @param: node_id
// Destination Node Id
//
// @param: level
// Power level to set (from 0 to 9)
//
// @param: timeout
// Timeout in seconds
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
znet_error_t ICACHE_FLASH_ATTR znet_cc_powerlevel_set( uint8_t node_id,
                                                       uint8_t level,
                                                       uint8_t timeout,
                                                       ZFunction_Callback Callback,
                                                       void* callbackArg,
                                                       encap_type_t encap_type );

// PowerLevel Report CB
//
void ICACHE_FLASH_ATTR znet_cc_powerlevel_report( const ZFunction func,
                                                  uint8_t node_id,
                                                  int cc_data_len,
                                                  const uint8_t* cc_data );

// Send PowerLevel Test Node Get
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
znet_error_t ICACHE_FLASH_ATTR znet_cc_powerlevel_test_node_get( uint8_t node_id,
                                                                 ZFunction_Callback Callback,
                                                                 void* callbackArg,
                                                                 encap_type_t encap_type );

// Send PowerLevel Test Node Set
//
// @param: znet
// ZNet object instance
//
// @param: node_id
// Destination Node Id
//
// @param: testNodeId
// Node to set test packets to
//
// @param: level
// Power level to use (from 0 to 9)
//
// @param: frameCount
// Number of test frames to send (from 1 to 65535)
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
znet_error_t ICACHE_FLASH_ATTR znet_cc_powerlevel_test_node_set( uint8_t node_id,
                                                                 uint8_t testNodeId,
                                                                 uint8_t level,
                                                                 uint16_t frameCount,
                                                                 ZFunction_Callback Callback,
                                                                 void* callbackArg,
                                                                 encap_type_t encap_type );

// PowerLevel Report CB
//
void ICACHE_FLASH_ATTR znet_cc_powerlevel_test_node_report( const ZFunction func,
                                                            uint8_t node_id,
                                                            int cc_data_len,
                                                            const uint8_t* cc_data );

// PowerLevel Receive
//
void ICACHE_FLASH_ATTR znet_cc_powerlevel_receive( const ZFunction func,
                                                   uint8_t node_id,
                                                   int cc_data_len,
                                                   const uint8_t* cc_data );

//////////////////////////////////////////////////////////////////////////////
// Command Class ZIP//

//----------------------------------------------------------------------------
// Zip Packet Command
//
// @param: node_id
// Destination Node Id
//
// @param: nack_opt_error
// NAck - Option Error (1 bits)
//
// @param: nack_queue_full
// NAck - Queue Full (1 bits)
//
// @param: nack_waiting
// NAck - Waiting (1 bits)
//
// @param: nack_resp
// NAck Response (1 bits)
//
// @param: ack_resp
// Ack Response (1 bits)
//
// @param: ack_req
// Ack Request (1 bits)
//
// @param: sec_origin
// Secure Origin (1 bits)
//
// @param: more_info
// More Information (1 bits)
//
// @param: cmd_include
// Z-Wave Cmd Included (1 bits)
//
// @param: ext_include
// Header ext. included (1 bits)
//
// @param: seq_n
// Seq No
//
// @param: srs_end_point
// Source End Point (7 bits)
//
// @param: dest_end_point
// Destination End Point (7 bits)
//
// @param: bit_addr
// Bit Address (1 bits)
//
// @param: header_length
// Header Length
//
// @param: header_ext
// Header extension
//
// @param: wave_cmd_size
//
// @param: wave_cmd
// Z-Wave command
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
znet_error_t ICACHE_FLASH_ATTR znet_cc_zip_packet( uint8_t node_id,
                                                   bool nack_opt_error,
                                                   bool nack_queue_full,
                                                   bool nack_waiting,
                                                   bool nack_resp,
                                                   bool ack_resp,
                                                   bool ack_req,
                                                   bool sec_origin,
                                                   bool more_info,
                                                   bool cmd_include,
                                                   bool ext_include,
                                                   uint8_t seq_n,
                                                   uint8_t srs_end_point,
                                                   uint8_t dest_end_point,
                                                   bool bit_addr,
                                                   uint8_t header_length,
                                                   void* header_ext,
                                                   uint8_t wave_cmd_size,
                                                   void* wave_cmd,
                                                   ZFunction_Callback Callback,
                                                   void* callbackArg );

//////////////////////////////////////////////////////////////////////////////
// Command Class ZIP Gateway//

//----------------------------------------------------------------------------
// Gateway Mode Set Command
//
// @param: node_id
// Destination Node Id
//
// @param: mode
// Mode
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
znet_error_t ICACHE_FLASH_ATTR znet_cc_gateway_mode_set( uint8_t node_id,
                                                         uint8_t mode,
                                                         ZFunction_Callback Callback,
                                                         void* callbackArg );

//----------------------------------------------------------------------------
// Gateway Mode Get Command
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
znet_error_t ICACHE_FLASH_ATTR znet_cc_gateway_mode_get( uint8_t node_id,
                                                         ZFunction_Callback Callback,
                                                         void* callbackArg );

//----------------------------------------------------------------------------
// Gateway Peer Set Command
//
// @param: node_id
// Destination Node Id
//
// @param: peer_profile
// Peer Profile
//
// @param: ipv6_addr
// IPv6 Address
//
// @param: port
// Port
//
// @param: peer_name_length
// Peer Name Length (6 bits)
//
// @param: peer_name
// Peer Name
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
znet_error_t ICACHE_FLASH_ATTR znet_cc_gateway_peer_set( uint8_t node_id,
                                                         uint8_t peer_profile,
                                                         void* ipv6_addr,
                                                         uint16_t port,
                                                         uint8_t peer_name_length,
                                                         void* peer_name,
                                                         ZFunction_Callback Callback,
                                                         void* callbackArg );

//----------------------------------------------------------------------------
// Gateway Peer Get Command
//
// @param: znet
// ZNet object instance
//
// @param: node_id
// Destination Node Id
//
// @param: peer_profile
// Peer Profile
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
znet_error_t ICACHE_FLASH_ATTR znet_cc_gateway_peer_get( uint8_t node_id,
                                                         uint8_t peer_profile,
                                                         ZFunction_Callback Callback,
                                                         void* callbackArg );

//----------------------------------------------------------------------------
// Gateway Lock Set Command
//
// @param: node_id
// Destination Node Id
//
// @param: lock
// Lock (1 bits)
//
// @param: show
// Show (1 bits)
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
znet_error_t ICACHE_FLASH_ATTR
znet_cc_gateway_lock_set( uint8_t node_id, bool lock, bool show, ZFunction_Callback Callback, void* callbackArg );

//----------------------------------------------------------------------------
// Unsolicited Destination Set Command
//
// @param: node_id
// Destination Node Id
//
// @param: dest_id
// Unsolicited IPv6 Destination
//
// @param: dest_port
// Unsolicited Destination Port
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
znet_error_t ICACHE_FLASH_ATTR znet_cc_unsolicited_destination_set( uint8_t node_id,
                                                                    void* dest_ip,
                                                                    uint16_t dest_port,
                                                                    ZFunction_Callback Callback,
                                                                    void* callbackArg );

//----------------------------------------------------------------------------
// Unsolicited Destination Get Command
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
znet_error_t ICACHE_FLASH_ATTR znet_cc_unsolicited_destination_get( uint8_t node_id,
                                                                    ZFunction_Callback Callback,
                                                                    void* callbackArg );

//----------------------------------------------------------------------------
// Application Node Info Set Command
//
// @param: node_id
// Destination Node Id
//
// @param: size_ns_command
//
// @param: ns_command_class
// Non-Secure Command Class
//
// @param: ss_mark
// Security Scheme 0 MARK
//
// @param: size_ss_command
//
// @param: ss_command
// Security Scheme 0 Command Class
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
znet_error_t ICACHE_FLASH_ATTR znet_cc_application_node_info_set( uint8_t node_id,
                                                                  uint8_t size_ns_command,
                                                                  void* ns_command_class,
                                                                  uint16_t ss_mark,
                                                                  uint8_t size_ss_command,
                                                                  void* ss_command,
                                                                  ZFunction_Callback Callback,
                                                                  void* callbackArg );

//----------------------------------------------------------------------------
// Application Node Info Get Command
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
znet_error_t ICACHE_FLASH_ATTR znet_cc_application_node_info_get( uint8_t node_id,
                                                                  ZFunction_Callback Callback,
                                                                  void* callbackArg );

//////////////////////////////////////////////////////////////////////////////
// Command Class ZIP NODE //

//----------------------------------------------------------------------------
// ZIP NODE SOLICITATION
//
// @param: node_id
// Destination Node Id
//
// @param: ipv6
// array of IPv6 adress
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
znet_error_t ICACHE_FLASH_ATTR znet_cc_zip_node_solicitation( ZNet,
                                                              uint8_t node_id,
                                                              const uint8_t* ipv6,
                                                              ZFunction_Callback Callback,
                                                              void* callbackArg );

//----------------------------------------------------------------------------
// ZIP INVERSE NODE SOLICITATION
//
// @param: local
// Destination Local (1bits)
//
// @param: node_id
// Node Id
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
znet_error_t ICACHE_FLASH_ATTR znet_cc_zip_inv_node_solicitation( bool local,
                                                                  uint8_t node_id,
                                                                  ZFunction_Callback Callback,
                                                                  void* callbackArg );

//----------------------------------------------------------------------------
// ZIP NODE ADVERTISEMENT
//
// @param: validity
// Destination Validity (2bits)
//
// @param: local
// Local (1bits)
//
// @param: node_id
// Node Id
//
// @param: ipv6
// array of IPv6 adress
//
// @param: home_id
// array of HOME_ID
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
znet_error_t ICACHE_FLASH_ATTR znet_cc_zip_node_advertisement( uint8_t validity,
                                                               bool local,
                                                               uint8_t node_id,
                                                               void* ipv6,
                                                               uint32_t home_id,
                                                               ZFunction_Callback Callback,
                                                               void* callbackArg );

//////////////////////////////////////////////////////////////////////////////
// Command Class ZIP Portal//

//----------------------------------------------------------------------------
// Gateway Configuration Set Command
//
// @param: node_id
// Destination Node Id
//
// @param: lan_ipv6_addr
// LAN IPv6 Address
//
// @param: lan_ipv6_prefix_length
// LAN IPv6 Prefix Length
//
// @param: portal_ipv6_prefix
// Portal IPv6 Prefix
//
// @param: portal_ipv6_prefix_length
// Portal IPv6 Prefix Length
//
// @param: default_getaway_ipv6
// Default Gateway IPv6 Address
//
// @param: pan_ipv6_prefix
// PAN IPv6 Prefix
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
znet_error_t ICACHE_FLASH_ATTR znet_cc_gateway_config_set( uint8_t node_id,
                                                           void* lan_ipv6_addr,
                                                           uint8_t lan_ipv6_prefix_length,
                                                           void* portal_ipv6_prefix,
                                                           uint8_t portal_ipv6_prefix_length,
                                                           void* default_getaway_ipv6,
                                                           void* pan_ipv6_prefix,
                                                           ZFunction_Callback Callback,
                                                           void* callbackArg );

//----------------------------------------------------------------------------
// Gateway Configuration Status Command
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
znet_error_t ICACHE_FLASH_ATTR znet_cc_gateway_config_status( uint8_t node_id,
                                                              uint8_t status,
                                                              ZFunction_Callback Callback,
                                                              void* callbackArg );

//----------------------------------------------------------------------------
// Gateway Configuration Get Command
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
znet_error_t ICACHE_FLASH_ATTR znet_cc_gateway_config_get( uint8_t node_id,
                                                           ZFunction_Callback Callback,
                                                           void* callbackArg );

//////////////////////////////////////////////////////////////////////////////
// Command Class Inclusion controller//

//----------------------------------------------------------------------------
// Initiate Command
//
// @param: znet
// ZNet object instance
//
// @param: node_id
// Destination Node Id
//
// @param: node_number
// Node ID display as: NODE_NUMBER
//
// @param: step_id
// Step ID
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
znet_error_t ICACHE_FLASH_ATTR znet_cc_inclusion_initiate( uint8_t node_id,
                                                           uint8_t node_number,
                                                           uint8_t step_id,
                                                           ZFunction_Callback Callback,
                                                           void* callbackArg,
                                                           encap_type_t encap_type );

//----------------------------------------------------------------------------
// Complete Command
//
// @param: this
// ZNet object instance
//
// @param: node_id
// Destination Node Id
//
// @param: step_id
// Step ID
//
// @param: status
// status
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
znet_error_t ICACHE_FLASH_ATTR znet_cc_inclusion_complete( uint8_t node_id,
                                                           uint8_t step_id,
                                                           uint8_t status,
                                                           ZFunction_Callback Callback,
                                                           void* callbackArg,
                                                           encap_type_t encap_type );

//----------------------------------------------------------------------------
// Inclusion receive  Command
void ICACHE_FLASH_ATTR znet_cc_inclusion_receive( const ZFunction func,
                                                  uint8_t node_id,
                                                  int cc_data_len,
                                                  const uint8_t* cc_data );

bool is_inclusion_controller_initiate_command( const uint8_t* data, uint8_t len );

//----------------------------------------------------------------------------
// Send No Operation
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
znet_error_t ICACHE_FLASH_ATTR znet_cc_no_operation( uint8_t node_id, ZFunction_Callback callback, void* callbackArg );

//////////////////////////////////////////////////////////////////////////////
// Command Class Node Provisioning//

void ICACHE_FLASH_ATTR znet_cc_node_provisioning_set_received( const ZFunction func,
                                                               uint8_t node_id,
                                                               int cc_data_len,
                                                               const uint8_t* cc_data );

void ICACHE_FLASH_ATTR znet_cc_node_provisioning_delete_received( const ZFunction func,
                                                                  uint8_t node_id,
                                                                  int cc_data_len,
                                                                  const uint8_t* cc_data );

void ICACHE_FLASH_ATTR znet_cc_node_provisioning_list_iteration_get_received( const ZFunction func,
                                                                              uint8_t node_id,
                                                                              int cc_data_len,
                                                                              const uint8_t* cc_data );

void ICACHE_FLASH_ATTR znet_cc_node_provisioning_get_received( const ZFunction func,
                                                               uint8_t node_id,
                                                               int cc_data_len,
                                                               const uint8_t* cc_data );

//----------------------------------------------------------------------------
// Send Node Provisioning Report
//
// @param: node_id
// Destination Node Id
//
// @param: channel_id
// Destination channel Id
//
// @param: sequence_number
// Sequence number
//
// @param: dsk
// DSK key
//
// @param: dsk_length
// DSK key length (0 or 16)
//
// @param: meta_data
// Meta data
//
// @param: metadata_length
// Meta data length
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
znet_error_t ICACHE_FLASH_ATTR znet_cc_node_provisioning_report( uint8_t node_id,
                                                                 uint8_t channel_id,
                                                                 uint8_t sequence_number,
                                                                 uint8_t* dsk,
                                                                 uint8_t dsk_length,
                                                                 uint8_t* meta_data,
                                                                 uint8_t metadata_length,
                                                                 ZFunction_Callback callback,
                                                                 void* callbackArg,
                                                                 encap_type_t encap_type );

//----------------------------------------------------------------------------
// Send Node Provisioning List Iteration Report
//
// @param: node_id
// Destination Node Id
//
// @param: channel_id
// Destination channel Id
//
// @param: sequence_number
// Sequence number
//
// @param: remaining_count
// Remaining count
//
// @param: dsk
// DSK key
//
// @param: dsk_length
// DSK key length (0 or 16)
//
// @param: meta_data
// Meta data
//
// @param: metadata_length
// Meta data length
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
znet_error_t ICACHE_FLASH_ATTR znet_cc_node_provisioning_list_iteration_report( uint8_t node_id,
                                                                                uint8_t channel_id,
                                                                                uint8_t sequence_number,
                                                                                uint8_t remaining_count,
                                                                                uint8_t* dsk,
                                                                                uint8_t dsk_length,
                                                                                uint8_t* meta_data,
                                                                                uint8_t metadata_length,
                                                                                ZFunction_Callback callback,
                                                                                void* callbackArg,
                                                                                encap_type_t encap_type );

#endif  // #ifndef __ZNET_LIB_CC_NETWORK_H__
