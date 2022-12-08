
//////////////////////////////////////////////////////////////////////////////

#include "typedefs.h"

#include "byte_ops.h"

//#include "ZErrors.h"
#include "znet.h"

#include "ZPlatform.h"
//#include "znet_fc_private.h"
#include "znet_lib.h"
#include "znet_lib_fc.h"
#include "znet_lib_cc.h"
#include "logger.h"

uint8_t supervision_node_id = 0;
uint8_t supervision_timer = 0;
uint8_t supervision_status = SUPERVISION_REPORT_NO_SUPPORT;
uint8_t supervision_session_id = 0;
uint8_t supervision_security = 0;
uint8_t supervision_endpoint = 0;

extern uint8_t MY_NODE_ID;
extern uint8_t SecureClasses[];
extern uint8_t nSecureClasses;

extern encap_type_t ICACHE_FLASH_ATTR get_security_by_id( uint8_t node_id );

//////////////////////////////////////////////////////////////////////////////

//----------------------------------------------------------------------------
// Supervision Report Command Send
znet_error_t ICACHE_FLASH_ATTR znet_cc_supervision_report_send( uint8_t node_id,
                                                                uint8_t channel_id,
                                                                uint8_t status,
                                                                uint8_t session_id,
                                                                uint8_t duration,
                                                                ZFunction_Callback Callback,
                                                                void* callbackArg,
                                                                encap_type_t encap_type ) {
    ZW_SUPERVISION_REPORT_FRAME data;
    data.cmdClass = COMMAND_CLASS_SUPERVISION;
    data.cmd = SUPERVISION_REPORT;
    data.properties1 = session_id;
    data.status = status;
    data.duration = duration;
    /// TODO: check channel support
    return znet_fc_send_data( node_id, channel_id, sizeof data, (uint8_t*)&data, Callback, callbackArg, encap_type );
}

//----------------------------------------------------------------------------
// Supervision timer
void ICACHE_FLASH_ATTR supervision_timeout( void* dat ) {
    Log_Write( "supervision_timeout\r\n" );
    znet_cc_supervision_report_send( supervision_node_id,
                                     supervision_endpoint,
                                     supervision_status,
                                     supervision_session_id,
                                     0,
                                     NULL,
                                     NULL,
                                     supervision_security );

    znet_timer_stop( &supervision_timer );
}

//----------------------------------------------------------------------------
// Supervision Recieve Command

static bool znet_is_secure_cc( uint8_t cc_id ) {
    for( uint8_t i = 0; i < nSecureClasses; i++ ) {
        if( cc_id == SecureClasses[i] ) {
            return true;
        }
    }

    return false;
}

void ICACHE_FLASH_ATTR znet_cc_supervision_receive( const ZFunction func,
                                                    uint8_t node_id,
                                                    int cc_data_len,
                                                    const uint8_t* cc_data ) {
    if( cc_data[1] == SUPERVISION_GET ) {
        supervision_session_id = cc_data[2] & 0x3f;
        supervision_node_id = node_id;
        supervision_status = SUPERVISION_REPORT_NO_SUPPORT;
        supervision_security = func->_security;
        supervision_endpoint = func->_endpoint;

        const bool from_master_secured_ok = ( MY_NODE_ID != 1 ) && func->_security == get_security_by_id( node_id );

        const bool correct_secured = znet_is_secure_cc( cc_data[4] ) && func->_security != Encapsulation_None &&
                                     func->_security != Encapsulation_CRC16 &&
                                     func->_security == get_security_by_id( MY_NODE_ID );

        const bool from_slave_free_or_secured_ok =
            ( MY_NODE_ID == 1 ) && ( !znet_is_secure_cc( cc_data[4] ) || correct_secured );

        const bool should_process_command =
            is_node_added( node_id ) && ( from_master_secured_ok || from_slave_free_or_secured_ok );

        if( should_process_command ) {
            ZNet_ApplicationCommandHandler(
                (uint8_t*)&cc_data[4], cc_data[3], node_id, func->_security, func->_endpoint );
        }
        else {
            Log_Write( "=== wrong security class ===\r\n" );
            supervision_timeout( NULL );
            return;
        }

        znet_timer_init( &supervision_timer, supervision_timeout, 0 );
        if( func->_security == Encapsulation_Sec_0 )
            znet_timer_set( &supervision_timer, 600 );
        else
            znet_timer_set( &supervision_timer, 400 );
    }
}

void ICACHE_FLASH_ATTR znet_cc_supervision_report( const ZFunction func,
                                                   uint8_t node_id,
                                                   int cc_data_len,
                                                   const uint8_t* cc_data ) {}
