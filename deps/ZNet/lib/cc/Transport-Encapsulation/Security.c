#include "typedefs.h"

//#include "ZErrors.h"
#include "znet.h"

#include "ZPlatform.h"
//#include "znet_fc_private.h"
#include "znet_lib.h"
#include "znet_lib_fc.h"
#include "znet_lib_cc.h"

#include "S2_wrap.h"
#include "znet_fc_private.h"
#include "logger.h"
#include "security_layer.h"

extern uint8_t networkKey[16]; /* The master key */
extern encap_type_t ICACHE_FLASH_ATTR get_security_by_id( uint8_t node_id );

//----------------------------------------------------------------------------
// Security Report CB
//
void ICACHE_FLASH_ATTR znet_cc_security_receive( const ZFunction func,
                                                 uint8_t node_id,
                                                 int cc_data_len,
                                                 const uint8_t* cc_data ) {
    Log_Write( "znet_cc_security_receive  \n" );
    ts_param_t* p = (ts_param_t*)Heap_Alloc( sizeof( ts_param_t ) );
    if( p == NULL )
        return;

    p->snode = node_id;
    p->dnode = MY_NODE_ID;
    p->tx_flags = TRANSMIT_OPTION_ACK | TRANSMIT_OPTION_AUTO_ROUTE;
    //  p->rx_flags = rx_value->bytes[0];
    p->rx_flags = 0;
    if( func->_security == Encapsulation_Sec_0 )
        p->scheme = SECURITY_SCHEME_0;
    else
        p->scheme = NO_SCHEME;

    security_CommandHandler( p, (ZW_APPLICATION_TX_BUFFER*)cc_data, cc_data_len );
    Heap_Free( p );
}

//----------------------------------------------------------------------------
// Network Key Set Command
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
znet_error_t ICACHE_FLASH_ATTR znet_cc_network_key_set( uint8_t node_id,
                                                        ZFunction_Callback Callback,
                                                        void* callbackArg ) {
    uint8_t header[] = { COMMAND_CLASS_SECURITY, NETWORK_KEY_SET };
    uint8_t header_len = sizeof header;

    uint8_t data_len = header_len + sizeof( networkKey );
    uint8_t* data = (uint8_t*)Heap_Alloc( data_len );
    if( data == NULL )
        return ZNET_ERR_NOMEM;

    memcpy( &data[0], header, header_len );
    memcpy( &data[header_len], networkKey, sizeof( networkKey ) );

    /// TODO: check channel support
    znet_error_t result = znet_fc_send_data( node_id, 0, data_len, data, Callback, callbackArg, Encapsulation_Sec_0 );
    Heap_Free( data );

    return result;
}

//----------------------------------------------------------------------------
// Security Scheme Get Command
//
// @param: this
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
                                                            void* callbackArg ) {
    uint8_t data[] = { COMMAND_CLASS_SECURITY, SECURITY_SCHEME_GET, schemes };
    /// TODO: check channel support
    return znet_fc_send_data( node_id, 0, sizeof data, data, Callback, callbackArg, Encapsulation_None );
}

uint8_t callback_arg_sec[10];
uint8_t scheme_get_node_id = 0;

void ICACHE_FLASH_ATTR
get_home_id_sec_Callback( ZFunction func, void* arg, ZFuncFailures_e reason, const void* dataPtr, size_t dataSize ) {
    if( FUNC_OK == reason ) {
        Log_Write( "get_home_id_sec_Callback  MY_NODE_ID = %d\n", MY_NODE_ID );

        uint8_t cc_data[] = { COMMAND_CLASS_SECURITY, SECURITY_SCHEME_GET, 0 };
        znet_cc_security_receive( func, scheme_get_node_id, sizeof( cc_data ), cc_data );
    }
    else {
        Log_Write( "get_home_id_sec_Callback\r\n" );
    }
}

//----------------------------------------------------------------------------
// Security Scheme Get CB
//
void ICACHE_FLASH_ATTR znet_cc_security_scheme_get_rec( const ZFunction func,
                                                        uint8_t node_id,
                                                        int UNUSED( cc_data_len ),
                                                        const uint8_t* UNUSED( cc_data ) ) {
    scheme_get_node_id = node_id;
    znet_fc_get_home_id( get_home_id_sec_Callback, callback_arg_sec );
}

//----------------------------------------------------------------------------
// Security Scheme Inherit
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
znet_error_t ICACHE_FLASH_ATTR znet_cc_security_scheme_inherit( uint8_t node_id,
                                                                uint8_t schemes,
                                                                ZFunction_Callback Callback,
                                                                void* callbackArg,
                                                                encap_type_t encap_type ) {
    ZW_SECURITY_SCHEME_INHERIT_FRAME data;
    data.cmdClass = COMMAND_CLASS_SECURITY;
    data.cmd = SECURITY_SCHEME_INHERIT;
    data.supportedSecuritySchemes = schemes;

    /// TODO: check channel support
    return znet_fc_send_data( node_id, 0, sizeof( data ), (uint8_t*)&data, Callback, callbackArg, encap_type );
}

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
                                                               encap_type_t encap_type ) {
    ZW_SECURITY_COMMANDS_SUPPORTED_GET_FRAME data;
    data.cmdClass = COMMAND_CLASS_SECURITY;
    data.cmd = SECURITY_COMMANDS_SUPPORTED_GET;

    return znet_fc_send_data( node_id, channel_id, sizeof( data ), (uint8_t*)&data, Callback, callbackArg, encap_type );
}

//----------------------------------------------------------------------------
// Security Supported Get Recive Command
//
void ICACHE_FLASH_ATTR znet_cc_security_supported_get_recive( const ZFunction func,
                                                              uint8_t node_id,
                                                              int UNUSED( cc_data_len ),
                                                              const uint8_t* UNUSED( cc_data ) ) {
    if( func->_security == Encapsulation_None ) {
        return;
    }

    uint8_t assigned_keys = S2_get_assigned_keys();
    uint8_t size = 3;
    uint8_t dat[3 + nSecureClasses];
    dat[0] = COMMAND_CLASS_SECURITY;
    dat[1] = SECURITY_COMMANDS_SUPPORTED_REPORT;
    dat[2] = 0;
    Log_Write( "znet_cc_security_supported_get_recive  assigned_keys = %x\r\n", assigned_keys );
    encap_type_t node_sec = get_security_by_id( node_id );

    if( ( ( assigned_keys & Encapsulation_Sec_0 ) == assigned_keys ) || ( node_sec == Encapsulation_Sec_0 ) ) {
        for( uint8_t i = 0; i < nSecureClasses; i++ )
            dat[3 + i] = SecureClasses[i];
        size += nSecureClasses;
    }

    if( size <= sizeof( dat ) ) {
        /// TODO: check channel support
        znet_fc_send_data( node_id, 0, size, (uint8_t*)&dat, NULL, NULL, func->_security );
    }
}
