
//#include "ZErrors.h"
#include "znet.h"

#include "ZPlatform.h"
//#include "znet_fc_private.h"
#include "znet_lib.h"
#include "znet_lib_fc.h"
#include "znet_lib_cc.h"
#include "ZW_transport_api.h"

extern uint8_t sec2_supported_get;
extern uint8_t sec2_security;
extern void S2_get_commands_supported( uint8_t lnode, uint8_t class_id, uint8_t** cmdClasses, uint8_t* length );
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
                                                                void* callbackArg ) {
    uint8_t header[] = { COMMAND_CLASS_SECURITY_2,
                         SECURITY_2_NONCE_REPORT,
                         sequence_number,
                         ( MOS ? 0x02 : 0x01 ) | ( SOS ? 0x01 : 0x00 ) };
    uint8_t header_len = sizeof header;
    uint8_t data_len = header_len + 16;
    uint8_t* data = (uint8_t*)Heap_Alloc( data_len );
    if( data == NULL )
        return ZNET_ERR_NOMEM;

    memcpy( &data[0], header, header_len );
    memcpy( &data[header_len], entropy, 16 );

    /// TODO: check channel support
    znet_error_t result = znet_fc_send_data( node_id, 0, data_len, data, Callback, callbackArg, Encapsulation_None );
    Heap_Free( data );

    return result;
}

void ICACHE_FLASH_ATTR znet_cc_security_2_capabilities_report( const ZFunction func,
                                                               uint8_t node_id,
                                                               int cc_data_len,
                                                               const uint8_t* cc_data ) {}

void ICACHE_FLASH_ATTR znet_cc_security_2_network_key_report( const ZFunction func,
                                                              uint8_t node_id,
                                                              int cc_data_len,
                                                              const uint8_t* cc_data ) {}

#include "S2_wrap.h"
#include "znet_fc_private.h"
#include "logger.h"
extern uint8_t req_key;
//----------------------------------------------------------------------------
// Security 2 Report CB
//

static void ICACHE_FLASH_ATTR znet_handle_security_2_command( uint8_t node_id,
                                                              uint8_t rx_flags,
                                                              int cc_data_len,
                                                              const uint8_t* cc_data ) {
    Log_Write( "%s\n", __func__ );

    ts_param_t* p = (ts_param_t*)Heap_Alloc( sizeof( ts_param_t ) );
    if( p == NULL )
        return;

    p->snode = node_id;
    p->dnode = MY_NODE_ID;
    p->tx_flags = TRANSMIT_OPTION_ACK | TRANSMIT_OPTION_AUTO_ROUTE;
    p->rx_flags = rx_flags;
    p->scheme = NO_SCHEME;

    if( cc_data[1] == KEX_REPORT )
        req_key = cc_data[5];

    security2_CommandHandler( p, (ZW_APPLICATION_TX_BUFFER*)cc_data, cc_data_len );
    Heap_Free( p );
}

void ICACHE_FLASH_ATTR znet_cc_security_2_receive( const ZFunction func,
                                                   uint8_t node_id,
                                                   int cc_data_len,
                                                   const uint8_t* cc_data ) {
    Buffer rxed_frame = &func->_znet->_frame;
    uint8_t* data = rxed_frame->_data;

    znet_handle_security_2_command( node_id, data[2], cc_data_len, cc_data );
}

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
                                                                              void* callbackArg ) {
    uint8_t data[] = { COMMAND_CLASS_SECURITY_2, SECURITY_2_MESSAGE_ENCAPSULATION /*, sequence_number*/ };
    /// TODO: check channel support
    return znet_fc_send_data( node_id, 0, sizeof data, data, Callback, callbackArg, Encapsulation_None );
}

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
                                                           void* callbackArg ) {
    uint8_t data[] = { COMMAND_CLASS_SECURITY_2, KEX_GET };
    /// TODO: check channel support
    return znet_fc_send_data( node_id, 0, sizeof data, data, Callback, callbackArg, Encapsulation_None );
}

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
                                                           void* callbackArg ) {
    uint8_t data[] = {
        COMMAND_CLASS_SECURITY_2, KEX_SET, ( CSA ? 0x02 : 0x00 ) | ( echo ? 0x01 : 0x00 ), scheme, ECDH, keys
    };
    /// TODO: check channel support
    return znet_fc_send_data( node_id, 0, sizeof data, data, Callback, callbackArg, Encapsulation_None );
}

uint8_t callback_arg[10];
uint8_t kex_get_node_id = 0;

void ICACHE_FLASH_ATTR
get_home_id_Callback( ZFunction func, void* arg, ZFuncFailures_e reason, const void* dataPtr, size_t dataSize ) {
    if( FUNC_OK == reason ) {
        extern uint8_t start_incl_timer;
        Log_Write( "get_home_id_Callback kex_get_node_id %d\r\n", kex_get_node_id );
        if( start_incl_timer ) {
            sec2_start_learn_mode( kex_get_node_id );
        }

        uint8_t data[] = { COMMAND_CLASS_SECURITY_2, KEX_GET };
        znet_handle_security_2_command( kex_get_node_id, 0, sizeof( data ), data );
    }
    else {
        Log_Write( "get_home_id_Callback\r\n" );
    }
}

//----------------------------------------------------------------------------
// Security 2 kex set
//
void ICACHE_FLASH_ATTR znet_cc_security_2_kex_get_received( const ZFunction func,
                                                            uint8_t node_id,
                                                            int UNUSED( cc_data_len ),
                                                            const uint8_t* UNUSED( cc_data ) ) {
    Buffer rxed_frame = &func->_znet->_frame;
    uint8_t* data = rxed_frame->_data;

    if( ( data[2] & RECEIVE_STATUS_TYPE_MASK ) == RECEIVE_STATUS_TYPE_BROAD )
        return;  // do not answer to KEX_GET if message is broadcast

    kex_get_node_id = node_id;
    znet_fc_get_home_id( get_home_id_Callback, callback_arg );
}

//----------------------------------------------------------------------------
// Public Key Report Command
//
// @param: this
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
                                                                         void* callbackArg ) {
    uint8_t header[] = { COMMAND_CLASS_SECURITY_2, PUBLIC_KEY_REPORT, ( incl_node ? 0x01 : 0x00 ) };
    uint8_t header_len = sizeof header;
    uint8_t data_len = header_len + 16;
    uint8_t* data = (uint8_t*)Heap_Alloc( data_len );
    if( data == NULL )
        return ZNET_ERR_NOMEM;

    memcpy( &data[0], header, header_len );
    memcpy( &data[header_len], public_key, 16 );

    /// TODO: check channel support
    znet_error_t result = znet_fc_send_data( node_id, 0, data_len, data, Callback, callbackArg, Encapsulation_None );
    Heap_Free( data );

    return result;
}

//----------------------------------------------------------------------------
// Security 2 Report CB
//
void ICACHE_FLASH_ATTR znet_cc_security_2_supported_report_lib( const ZFunction func,
                                                                uint8_t node_id,
                                                                int cc_data_len,
                                                                const uint8_t* cc_data ) {
    sec2_supported_get = 0;
    znet_cc_security_2_supported_report( func, node_id, cc_data_len, cc_data );
}

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
                                                                 encap_type_t encap ) {
    sec2_security = encap;
    sec2_supported_get = 1;
    uint8_t data[] = { COMMAND_CLASS_SECURITY_2, SECURITY_2_COMMANDS_SUPPORTED_GET };
    return znet_fc_send_data( node_id, channel_id, sizeof data, data, Callback, callbackArg, encap );
}

//----------------------------------------------------------------------------
// Security 2 Commands Supported Get
//
void ICACHE_FLASH_ATTR znet_cc_security_2_supported_get_receive( const ZFunction func,
                                                                 uint8_t node_id,
                                                                 int UNUSED( cc_data_len ),
                                                                 const uint8_t* UNUSED( cc_data ) ) {
    if( func->_security == Encapsulation_None || func->_security == Encapsulation_Sec_0 ) {
        return;
    }

    uint8_t sec = NO_SCHEME;
    uint8_t n_commands_supported = 0;
    uint8_t* classes;

    Log_Write( "znet_cc_security_2_supported_get_receive\r\n" );
    switch( func->_security ) {
    case Encapsulation_Sec_UNAUTHENTICATED:
        sec = SECURITY_SCHEME_2_UNAUTHENTICATED - 1;
        break;
    case Encapsulation_Sec_AUTHENTICATED:
        sec = SECURITY_SCHEME_2_AUTHENTICATED - 1;
        break;
    case Encapsulation_Sec_2:
        sec = SECURITY_SCHEME_2_ACCESS - 1;
        break;
    default:
        sec = NO_SCHEME;
        break;
    }
    S2_get_commands_supported( MY_NODE_ID, sec, &classes, &n_commands_supported );
    Log_Write( "n_commands_supported %d \r\n", n_commands_supported );

    uint8_t data[n_commands_supported + 2];
    data[0] = COMMAND_CLASS_SECURITY_2;
    data[1] = SECURITY_2_COMMANDS_SUPPORTED_REPORT;
    if( n_commands_supported != 0 )
        memcpy( &data[2], classes, n_commands_supported );

    /// TODO: check channel support
    znet_fc_send_data( node_id, 0, sizeof data, data, NULL, NULL, func->_security );
}
