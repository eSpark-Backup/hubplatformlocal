
#ifndef __ZNET_LIB_CC_TRANSPORT_H__
#define __ZNET_LIB_CC_TRANSPORT_H__

#include "znet.h"

#include "ZFunctionClass.h"

#include "znet_lib_cc_codes.h"

//////////////////////////////////////////////////////////////////////////////
// Command Class CRC16 Encap//

//----------------------------------------------------------------------------
// CRC16 Encap Command
//
// @param: znet
// ZNet object instance
//
// @param: node_id
// Destination Node Id
//
// @param: command_class
// Command Class
//
// @param: command
// Command
//
// @param: msg_length
//
// @param: cmd_data
// Data
//
// @param: checksum
// Checksum (16 bits)
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
znet_error_t ICACHE_FLASH_ATTR znet_cc_crc16_encap( uint8_t node_id,
                                                    uint8_t msg_length,
                                                    const uint8_t* cmd_data,
                                                    ZFunction_Callback Callback,
                                                    void* callbackArg );

//----------------------------------------------------------------------------
// CRC16 Encap Command Encrypt
void ICACHE_FLASH_ATTR znet_cc_crc16_encrypt( const ZFunction func,
                                              uint8_t node_id,
                                              int cc_data_len,
                                              const uint8_t* cc_data );

//////////////////////////////////////////////////////////////////////////////
// Command Class MultiChannel //

// Send MultiChannel CapabilitiesGet
// Request information about the specified endpoint
//
// @param: znet
// ZNet object instance
//
// @param: node_id
// Destination Node Id
//
// @param: endpoint
// Endpoint in question
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
znet_error_t ICACHE_FLASH_ATTR znet_cc_multichannel_capability_get( uint8_t node_id,
                                                                    uint8_t endpoint,
                                                                    ZFunction_Callback Callback,
                                                                    void* callbackArg,
                                                                    encap_type_t encap_type );

//----------------------------------------------------------------------------
// Multi Channel Capability Report Command
//
void ICACHE_FLASH_ATTR znet_cc_multichannel_capability_report( const ZFunction func,
                                                               uint8_t node_id,
                                                               int cc_data_len,
                                                               const uint8_t* cc_data );

znet_error_t ICACHE_FLASH_ATTR znet_cc_multichannel_cmd_encapsulate( uint8_t node_id,
                                                                     uint8_t src_endpoint,
                                                                     uint8_t dst_endpoint,
                                                                     uint8_t cmd_data_length,
                                                                     const uint8_t* cmd_data,
                                                                     ZFunction_Callback Callback,
                                                                     void* callbackArg,
                                                                     encap_type_t encap_type );

bool ICACHE_FLASH_ATTR znet_cc_multichannel_instance_cmd_encapsulate( uint8_t node_id,
                                                                      uint8_t instance,
                                                                      bool res,
                                                                      uint8_t cmd_data_length,
                                                                      const uint8_t* cmd_data,
                                                                      ZFunction_Callback Callback,
                                                                      void* callbackArg,
                                                                      encap_type_t encap_type );

// Send MultiChannel EndpointFind
// Note that MultiChannel EndpointFind Report is not supported as useless. But
// one can still trap the response packet in logs.
//
// @param: znet
// ZNet object instance
//
// @param: node_id
// Destination Node Id
//
// @param: generic
// Generic type in search
//
// @param: specific
// Specific type in search
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
znet_error_t ICACHE_FLASH_ATTR znet_cc_multichannel_endpoint_find( uint8_t node_id,
                                                                   uint8_t generic,
                                                                   uint8_t specific,
                                                                   ZFunction_Callback Callback,
                                                                   void* callbackArg,
                                                                   encap_type_t encap_type );

//----------------------------------------------------------------------------
// Multi Channel Endpoint Find Report Command
//
void ICACHE_FLASH_ATTR znet_cc_multichannel_endpoint_find_report( const ZFunction func,
                                                                  uint8_t node_id,
                                                                  int cc_data_len,
                                                                  const uint8_t* cc_data );

// Send MultiChannel EndpointGet
// Get the number of available endpoints
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
znet_error_t ICACHE_FLASH_ATTR znet_cc_multichannel_endpoint_get( uint8_t node_id,
                                                                  ZFunction_Callback Callback,
                                                                  void* callbackArg,
                                                                  encap_type_t encap_type );

//----------------------------------------------------------------------------
// Multi Channel Endpoint Report Command
//
void ICACHE_FLASH_ATTR znet_cc_multichannel_endpoint_report( const ZFunction func,
                                                             uint8_t node_id,
                                                             int cc_data_len,
                                                             const uint8_t* cc_data );

// Send MultiChannel Get (MultiInstance V1 command)
// Reports number of channels supporting a defined Command Class
// Depricated by MutliChannel V2 - needed for old devices only
//
// @param: znet
// ZNet object instance
//
// @param: node_id
// Destination Node Id
//
// @param: cc_id
// Command Class Id in question
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
znet_error_t ICACHE_FLASH_ATTR znet_cc_multichannel_instance_get( uint8_t node_id,
                                                                  uint8_t cc_id,
                                                                  ZFunction_Callback Callback,
                                                                  void* callbackArg,
                                                                  encap_type_t encap_type );

//----------------------------------------------------------------------------
// Multi Instance Report Command
//
void ICACHE_FLASH_ATTR znet_cc_multichannel_instance_report( const ZFunction func,
                                                             uint8_t node_id,
                                                             int cc_data_len,
                                                             const uint8_t* cc_data );

znet_error_t ICACHE_FLASH_ATTR znet_cc_multichannel_aggregated_members_get( uint8_t node_id,
                                                                            uint8_t aggregated_endpoint,
                                                                            ZFunction_Callback Callback,
                                                                            void* callbackArg,
                                                                            encap_type_t encap_type );

//----------------------------------------------------------------------------
// Multi Channel Aggregated Members Report Command
//
void ICACHE_FLASH_ATTR znet_cc_multichannel_aggregated_members_report( const ZFunction func,
                                                                       uint8_t node_id,
                                                                       int cc_data_len,
                                                                       const uint8_t* cc_data );

//----------------------------------------------------------------------------
void ICACHE_FLASH_ATTR znet_cc_multichannel_recieve( const ZFunction func,
                                                     uint8_t node_id,
                                                     int cc_data_len,
                                                     const uint8_t* cc_data );

//////////////////////////////////////////////////////////////////////////////
// Command Class Multi Cmd

//----------------------------------------------------------------------------
// Multi Cmd Encap Command
//
void ICACHE_FLASH_ATTR znet_cc_multi_cmd_receive( const ZFunction func,
                                                  uint8_t node_id,
                                                  int cc_data_len,
                                                  const uint8_t* cc_data );

//////////////////////////////////////////////////////////////////////////////
// SECURITY 2 Class

//----------------------------------------------------------------------------
// Nonce Report
//
// @param: znet
// ZNet object instance
//
// @param: node_id
// Destination Node Id
//
// @param: node_id
// Destination Node Id
//
// @param: MOS
// MOS (1 bits)
//
// @param: SOS
// SOS (1 bits)
//
// @param: entropy
// Receivers Entropy Input 16 byte
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
znet_error_t ICACHE_FLASH_ATTR znet_cc_security_2_nonce_report( uint8_t node_id,
                                                                uint8_t sequence_number,
                                                                bool MOS,
                                                                bool SOS,
                                                                const uint8_t* entropy,
                                                                ZFunction_Callback Callback,
                                                                void* callbackArg );

//----------------------------------------------------------------------------
// Security 2 Report CB
//
void ICACHE_FLASH_ATTR znet_cc_security_2_nonce_get( const ZFunction func,
                                                     uint8_t node_id,
                                                     int cc_data_len,
                                                     const uint8_t* cc_data );

//----------------------------------------------------------------------------
// Security 2 Report CB
//
void ICACHE_FLASH_ATTR znet_cc_security_2_receive( const ZFunction func,
                                                   uint8_t node_id,
                                                   int cc_data_len,
                                                   const uint8_t* cc_data );

//----------------------------------------------------------------------------
// Message Encapsulation //
//
// @param: znet
// ZNet object instance
//
// @param: node_id
// Destination Node Id
//
// @param: ???????????????
// ???????????????????????
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
// TODO : ADD NEED ARGUMENTS
znet_error_t ICACHE_FLASH_ATTR znet_cc_security_2_message_encapsulation_send( uint8_t node_id,
                                                                              ZFunction_Callback Callback,
                                                                              void* callbackArg );

//----------------------------------------------------------------------------
// Message Encapsulation
//
void ICACHE_FLASH_ATTR znet_cc_security_2_message_encapsulation( const ZFunction func,
                                                                 uint8_t node_id,
                                                                 int cc_data_len,
                                                                 const uint8_t* cc_data );

//----------------------------------------------------------------------------
// KEX Get
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
znet_error_t ICACHE_FLASH_ATTR znet_cc_security_2_kex_get( uint8_t node_id,
                                                           ZFunction_Callback Callback,
                                                           void* callbackArg );

//----------------------------------------------------------------------------
// KEX Set
//
// @param: znet
// ZNet object instance
//
// @param: node_id
// Destination Node Id
//
// @param: CSA
// Request CSA (1 bits)
//
// @param: echo
// Echo (1 bits)
//
// @param: scheme
// Selected KEX Scheme
//
// @param: ECDH
// Selected ECDH Profile
//
// @param: keys
// Granted Keys
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
znet_error_t ICACHE_FLASH_ATTR znet_cc_security_2_kex_set( uint8_t node_id,
                                                           bool CSA,
                                                           bool echo,
                                                           uint8_t scheme,
                                                           uint8_t ECDH,
                                                           uint8_t keys,
                                                           ZFunction_Callback Callback,
                                                           void* callbackArg );

//----------------------------------------------------------------------------
// Security 2 kex get
//
void ICACHE_FLASH_ATTR znet_cc_security_2_kex_get_received( const ZFunction func,
                                                            uint8_t node_id,
                                                            int cc_data_len,
                                                            const uint8_t* cc_data );

//----------------------------------------------------------------------------
// Security 2 Report CB
//
void ICACHE_FLASH_ATTR znet_cc_security_2_kex_report( const ZFunction func,
                                                      uint8_t node_id,
                                                      int cc_data_len,
                                                      const uint8_t* cc_data );

//----------------------------------------------------------------------------
// Security 2 Report CB
//
void ICACHE_FLASH_ATTR znet_cc_security_2_public_key_report( const ZFunction func,
                                                             uint8_t node_id,
                                                             int cc_data_len,
                                                             const uint8_t* cc_data );

//----------------------------------------------------------------------------
// Public Key Report Command
//
// @param: znet
// ZNet object instance
//
// @param: node_id
// Destination Node Id
//
// @param: incl_node
// Including Node (1 bits)
//
// @param: public_key
// ECDH Public Key (16bytes)
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
znet_error_t ICACHE_FLASH_ATTR znet_cc_security_2_public_key_report_set( uint8_t node_id,
                                                                         bool incl_node,
                                                                         const uint8_t* public_key,
                                                                         ZFunction_Callback Callback,
                                                                         void* callbackArg );

//----------------------------------------------------------------------------
// Security 2 Report CB
//
void ICACHE_FLASH_ATTR znet_cc_security_2_network_key_report( const ZFunction func,
                                                              uint8_t node_id,
                                                              int cc_data_len,
                                                              const uint8_t* cc_data );

//----------------------------------------------------------------------------
// S2 Commands Supported Get Command
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
znet_error_t ICACHE_FLASH_ATTR znet_cc_security_2_supported_get( uint8_t node_id,
                                                                 uint8_t channel_id,
                                                                 ZFunction_Callback Callback,
                                                                 void* callbackArg,
                                                                 encap_type_t encap );

//----------------------------------------------------------------------------
// Security 2 Report CB
//
void ICACHE_FLASH_ATTR znet_cc_security_2_supported_report( const ZFunction func,
                                                            uint8_t node_id,
                                                            int cc_data_len,
                                                            const uint8_t* cc_data );

//----------------------------------------------------------------------------
// Security 2 Report lib CB
//
void ICACHE_FLASH_ATTR znet_cc_security_2_supported_report_lib( const ZFunction func,
                                                                uint8_t node_id,
                                                                int cc_data_len,
                                                                const uint8_t* cc_data );

//----------------------------------------------------------------------------
// Security 2 Report CB
//
void ICACHE_FLASH_ATTR znet_cc_security_2_capabilities_report( const ZFunction func,
                                                               uint8_t node_id,
                                                               int cc_data_len,
                                                               const uint8_t* cc_data );

//----------------------------------------------------------------------------
// Security 2 Commands Supported Get
//
void ICACHE_FLASH_ATTR znet_cc_security_2_supported_get_receive( const ZFunction func,
                                                                 uint8_t node_id,
                                                                 int cc_data_len,
                                                                 const uint8_t* cc_data );

//////////////////////////////////////////////////////////////////////////////
// Command Class Security//

//----------------------------------------------------------------------------
// Security Report CB
//
void ICACHE_FLASH_ATTR znet_cc_security_receive( const ZFunction func,
                                                 uint8_t node_id,
                                                 int cc_data_len,
                                                 const uint8_t* cc_data );

//----------------------------------------------------------------------------
// Network Key Set Command
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
znet_error_t ICACHE_FLASH_ATTR znet_cc_network_key_set( uint8_t node_id,
                                                        ZFunction_Callback Callback,
                                                        void* callbackArg );

//----------------------------------------------------------------------------
// Security Scheme Get Command
//
// @param: znet
// ZNet object instance
//
// @param: node_id
// Destination Node Id
//
// @param: schemes
// Supported Security Schemes
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
znet_error_t ICACHE_FLASH_ATTR znet_cc_security_scheme_get( uint8_t node_id,
                                                            uint8_t schemes,
                                                            ZFunction_Callback Callback,
                                                            void* callbackArg );

//----------------------------------------------------------------------------
// Security Scheme Get CB
//
void ICACHE_FLASH_ATTR znet_cc_security_scheme_get_rec( const ZFunction func,
                                                        uint8_t node_id,
                                                        int cc_data_len,
                                                        const uint8_t* cc_data );

void ICACHE_FLASH_ATTR znet_cc_NETWORK_KEY_VERIFY( const ZFunction func,
                                                   uint8_t node_id,
                                                   int cc_data_len,
                                                   const uint8_t* cc_data );

//----------------------------------------------------------------------------
// Security Scheme Inherit
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
znet_error_t ICACHE_FLASH_ATTR znet_cc_security_scheme_inherit( uint8_t node_id,
                                                                uint8_t schemes,
                                                                ZFunction_Callback Callback,
                                                                void* callbackArg,
                                                                encap_type_t encap_type );

//----------------------------------------------------------------------------
// Security Supported Report CB
//
void ICACHE_FLASH_ATTR znet_cc_security_supported_report( const ZFunction func,
                                                          uint8_t node_id,
                                                          int cc_data_len,
                                                          const uint8_t* cc_data );

//----------------------------------------------------------------------------
// Security Supported Get Command
//
// @param: this
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
znet_error_t ICACHE_FLASH_ATTR znet_cc_security_supported_get( uint8_t node_id,
                                                               uint8_t channel_id,
                                                               ZFunction_Callback Callback,
                                                               void* callbackArg,
                                                               encap_type_t encap_type );

void ICACHE_FLASH_ATTR znet_cc_security_supported_get_recive( const ZFunction func,
                                                              uint8_t node_id,
                                                              int cc_data_len,
                                                              const uint8_t* cc_data );

//////////////////////////////////////////////////////////////////////////////
// Command Class Transport Service//

//----------------------------------------------------------------------------
// First Segment Command
//
// @param: znet
// ZNet object instance
//
// @param: node_id
// Destination Node Id
//
// @param: datagram_size_1
// datagram_size_1 (3 bits)
//
// @param: datagram_size_2
// datagram_size_2
//
// @param: ext_avaliable
//
// @param: ext
// Ext (1 bits)
//
// @param: session_id
// Session ID
//
// @param: seq_n
// Sequence No (4 bits)
//
// @param: header_ext_avaliable
//
// @param: header_ext_length
// Header Extension Length
//
// @param: header_extension
// Header Extension
//
// @param: size_payload
//
// @param: payload
// Payload
//
// @param: check
// Frame Check Sequence
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
znet_error_t ICACHE_FLASH_ATTR znet_cc_first_segment( uint8_t node_id,
                                                      uint8_t datagram_size_1,
                                                      uint8_t datagram_size_2,
                                                      bool ext_avaliable,
                                                      bool ext,
                                                      uint8_t session_id,
                                                      uint8_t seq_n,
                                                      bool header_ext_avaliable,
                                                      uint8_t header_ext_length,
                                                      const uint8_t* header_extension,
                                                      uint8_t size_peyload,
                                                      const uint8_t* payload,
                                                      uint16_t check,
                                                      ZFunction_Callback Callback,
                                                      void* callbackArg );

//----------------------------------------------------------------------------
// Segment Complete Command
//
// @param: znet
// ZNet object instance
//
// @param: node_id
// Destination Node Id
//
// @param: session_id
// Session ID
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
znet_error_t ICACHE_FLASH_ATTR znet_cc_segment_complete( uint8_t node_id,
                                                         uint8_t session_id,
                                                         ZFunction_Callback Callback,
                                                         void* callbackArg );

//----------------------------------------------------------------------------
// Segment Request Command
//
// @param: znet
// ZNet object instance
//
// @param: node_id
// Destination Node Id
//
// @param: session_id
// Session ID
//
// @param: datagram_offset_1
// datagram_offset_1
//
// @param: datagram_offset_2
// datagram_offset_2
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
znet_error_t ICACHE_FLASH_ATTR znet_cc_segment_request( uint8_t node_id,
                                                        uint8_t session_id,
                                                        uint8_t datagram_offset_1,
                                                        uint8_t datagram_offset_2,
                                                        ZFunction_Callback Callback,
                                                        void* callbackArg );

//----------------------------------------------------------------------------
// Segment Wait Command
//
// @param: znet
// ZNet object instance
//
// @param: node_id
// Destination Node Id
//
// @param: pending_fragments
// pending_fragments
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
znet_error_t ICACHE_FLASH_ATTR znet_cc_segment_wait( uint8_t node_id,
                                                     uint8_t pending_fragments,
                                                     ZFunction_Callback Callback,
                                                     void* callbackArg );

//----------------------------------------------------------------------------
// Subsequent Segment Command
//
// @param: znet
// ZNet object instance
//
// @param: node_id
// Destination Node Id
//
// @param: datagram_size_1
// datagram_size_1 (3 bits)
//
// @param: datagram_size_2
// datagram_size_2
//
// @param: ext
// Ext (1 bits)
//
// @param: sesion_id
// Session ID
//
// @param: seq_n
// Sequence No (4 bits)
//
// @param: session_id
// Session ID
//
// @param: header_ext_length
// Header Extension Length
//
// @param: header_extension
// Header Extension
//
// @param: size_payload
//
// @param: payload
// Payload
//
// @param: check
// Frame Check Sequence
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
znet_error_t ICACHE_FLASH_ATTR znet_cc_subsequent_segment( uint8_t node_id,
                                                           uint8_t datagram_size_1,
                                                           uint8_t datagram_size_2,
                                                           bool ext_avaliable,
                                                           bool ext,
                                                           uint8_t session_id,
                                                           uint8_t seq_n,
                                                           uint8_t datagram_offset_1,
                                                           uint8_t datagram_offser_2,
                                                           bool header_ext_avaliable,
                                                           uint8_t header_ext_length,
                                                           const uint8_t* header_extension,
                                                           uint8_t size_peyload,
                                                           const uint8_t* payload,
                                                           uint16_t check,
                                                           ZFunction_Callback Callback,
                                                           void* callbackArg );

//----------------------------------------------------------------------------
// Transport Recive
//
void ICACHE_FLASH_ATTR znet_cc_transport_receive( const ZFunction func,
                                                  uint8_t node_id,
                                                  int cc_data_len,
                                                  const uint8_t* cc_data );

//////////////////////////////////////////////////////////////////////////////
// Command Class Transport Service//

//----------------------------------------------------------------------------
// Supervision Report Command
void ICACHE_FLASH_ATTR znet_cc_supervision_report( const ZFunction func,
                                                   uint8_t node_id,
                                                   int cc_data_len,
                                                   const uint8_t* cc_data );

//----------------------------------------------------------------------------
// Supervision Report Command Send
znet_error_t ICACHE_FLASH_ATTR znet_cc_supervision_report_send( uint8_t node_id,
                                                                uint8_t channel_id,
                                                                uint8_t status,
                                                                uint8_t session_id,
                                                                uint8_t duration,
                                                                ZFunction_Callback Callback,
                                                                void* callbackArg,
                                                                encap_type_t encap_type );

//----------------------------------------------------------------------------
// Supervision Report Command
void ICACHE_FLASH_ATTR znet_cc_supervision_receive( const ZFunction func,
                                                    uint8_t node_id,
                                                    int cc_data_len,
                                                    const uint8_t* cc_data );
#endif  // #ifndef __ZNET_LIB_CC_TRANSPORT_H__
