/**
 * @file znet_cmd_security.c
 * @date 23 May 2018
 * @brief TBD.
 */

/// INFO: crt & system
#include <assert.h>
#include <string.h>

/// INFO: public
#include <znet/znet.h>

/// INFO: private
#include "znet_main.h"
#include "znet_log.h"
#include "znet_store.h"
#include "znet_timer.h"

/// INFO: internal
#include "heap.h"
#include <znet_lib.h>
#include <znet_lib_cc_codes.h>
#include <znet_lib_cc_transport.h>

// node cmd security suported report ///////////////////////////////////
/// INFO: Security 0 Command Class v1
#define _ZNET_NODE_SUPPORTED_BUFF_SIZE 2
static uint8_t* _znet_node_supported_data = NULL;
static znet_timer_t* _znet_node_supported_timer = NULL;

static inline void _znet_node_supported_free( void ) {
    if( _znet_node_supported_timer ) {
        znet_timer_kill( _znet_node_supported_timer );
        _znet_node_supported_timer = NULL;
    }
    Heap_Free( (void*)_znet_node_supported_data );
    _znet_node_supported_data = NULL;
}

static void _znet_node_supported_timeout( znet_timer_t* timer, void* arg, int destroy ) {
    if( !destroy ) {
        if( timer == _znet_node_supported_timer ) {
            if( _znet_node_supported_data ) {
                if( znet_cb->node_cmd_security_supported_result )
                    znet_cb->node_cmd_security_supported_result(
                        0, _znet_node_supported_data[0], _znet_node_supported_data[1], NULL, znet_cb->arg );
            }
            else {
                if( znet_cb->node_cmd_security_supported_result )
                    znet_cb->node_cmd_security_supported_result( 0, ZNET_NODE_ID_INVALID, 0, NULL, znet_cb->arg );
            }
            _znet_node_supported_free();
        }
    }
    else if( timer == _znet_node_supported_timer ) {
        _znet_node_supported_timer = NULL;
    }
}

void znet_cc_security_supported_report( const ZFunction func,
                                        uint8_t node_id,
                                        int cc_data_len,
                                        const uint8_t* cc_data ) {
    _znet_node_supported_free();

    assert( cc_data );
    assert( cc_data_len >= 3 );
    assert( cc_data[0] == ZNET_COMMAND_CLASS_SECURITY );
    assert( cc_data[1] == SECURITY_COMMANDS_SUPPORTED_REPORT );

    if( node_id < ZNET_NODE_ID_MIN || node_id > ZNET_NODE_ID_MAX )
        return;

    const size_t ccount = cc_data_len - 3;

    const size_t buff_size = sizeof( znet_security_supported_report_t ) + ccount * sizeof( znet_command_class_t );
    uint8_t buff[buff_size];
    memset( buff, 0, buff_size );

    znet_security_supported_report_t* report = (znet_security_supported_report_t*)&buff;

    report->commands_count = ccount;
    size_t s = 0;
    for( size_t i = 0; i < ccount; ++i ) {
        /// TODO: need save all, but check
        /// znet_node_info_is_support_security_command done by mark
        if( cc_data[3 + i] == ZNET_COMMAND_CLASS_MARK )
            break;
        if( cc_data[3 + i] >= 0xF1 )  // MSB
        {
            /// TODO: implement extended command class (now skiped)
            ++i;  // LSB
            --report->commands_count;
            continue;
        }
        report->commands[s++] = cc_data[3 + i];
    }

    /// TODO: check retruned result from store
    znet_store_security_supported_set( node_id, report, cc_data[2] );

    if( znet_cb->node_cmd_security_supported_result )
        znet_cb->node_cmd_security_supported_result( 0, node_id, func->_endpoint, report, znet_cb->arg );

    if( cc_data[2] == 0 ) {
        if( znet_cb->node_cmd_security_supported_result )
            znet_cb->node_cmd_security_supported_result( 0, node_id, func->_endpoint, NULL, znet_cb->arg );
    }
}

// node cmd security2 suported report ///////////////////////////////////
/// INFO: Security 2 Command Class v1

void znet_cc_security_2_supported_report( const ZFunction func,
                                          uint8_t node_id,
                                          int cc_data_len,
                                          const uint8_t* cc_data ) {
    _znet_node_supported_free();

    assert( cc_data );
    assert( cc_data_len >= 2 );
    assert( cc_data[0] == ZNET_COMMAND_CLASS_SECURITY_2 );
    assert( cc_data[1] == SECURITY_2_COMMANDS_SUPPORTED_REPORT );

    if( node_id < ZNET_NODE_ID_MIN || node_id > ZNET_NODE_ID_MAX )
        return;

    const size_t ccount = cc_data_len - 2;

    const size_t buff_size = sizeof( znet_security_supported_report_t ) + ccount * sizeof( znet_command_class_t );
    uint8_t buff[buff_size];
    memset( buff, 0, buff_size );

    znet_security_supported_report_t* report = (znet_security_supported_report_t*)&buff;

    report->commands_count = ccount;
    size_t s = 0;
    for( size_t i = 0; i < ccount; ++i ) {
        /// TODO: need save all, but check
        /// znet_node_info_is_support_security_command done by mark
        if( cc_data[2 + i] == ZNET_COMMAND_CLASS_MARK )
            break;
        if( cc_data[2 + i] >= 0xF1 )  // MSB
        {
            /// TODO: implement extended command class (now skiped)
            ++i;  // LSB
            --report->commands_count;
            continue;
        }
        report->commands[s++] = cc_data[2 + i];
    }

    /// TODO: check retruned result from store
    znet_store_security_supported_set( node_id, report, 0 );

    ZNET_LOGD( "znet_cc_security_2_supported_report sec = %d \n", func->_security );
    uint8_t security = Encapsulation_None;
    if( func->_security & Encapsulation_Sec_2 ) {
        security = ZNET_NODEINFO_SECURITY_S2_ACCESS;
    }
    else if( func->_security & Encapsulation_Sec_AUTHENTICATED ) {
        security = ZNET_NODEINFO_SECURITY_S2_AUTH;
    }
    else if( func->_security & Encapsulation_Sec_UNAUTHENTICATED ) {
        security = ZNET_NODEINFO_SECURITY_S2_UNAUTH;
    }
    else if( func->_security & Encapsulation_Sec_0 ) {
        security = ZNET_NODEINFO_SECURITY_S0;
    }
    if( znet_cb->node_cmd_security_set )
        if( znet_cb->node_cmd_security_set( node_id, security, znet_cb->arg ) )
            ZNET_LOGE( "can`t set security for node with id = %d\n", node_id );

    if( znet_cb->node_cmd_security_supported_result )
        znet_cb->node_cmd_security_supported_result( 0, node_id, func->_endpoint, report, znet_cb->arg );

    if( znet_cb->node_cmd_security_supported_result )
        znet_cb->node_cmd_security_supported_result( 0, node_id, func->_endpoint, NULL, znet_cb->arg );
}

// node cmd security suported get //////////////////////////////////////
/// INFO: Security 0 and Security 2 Command Class v1

znet_error_t znet_node_cmd_security_supported_get( znet_node_id_t node_id, znet_node_channel_id_t channel_id ) {
    if( !znet_cb ) {
        ZNET_LOGE( "Library not initialized!\n" );
        return ZNET_ERR_FAULT;
    }

    if( node_id < ZNET_NODE_ID_MIN || node_id > ZNET_NODE_ID_MAX ) {
        return ZNET_ERR_INVAL;
    }

    /// TODO: check wait report for node_id

    encap_type_t encap = znet_utils_get_security_encap_type_by_node_id( node_id );
    if( encap == Encapsulation_None ) {
        return ZNET_ERR_NOTFOUND;
    }

    if( ( _znet_node_supported_data = (uint8_t*)Heap_Alloc( _ZNET_NODE_SUPPORTED_BUFF_SIZE ) ) == NULL ) {
        return ZNET_ERR_NOMEM;
    }
    _znet_node_supported_data[0] = node_id;
    _znet_node_supported_data[1] = channel_id;

    if( encap & Encapsulation_Sec_0 ) {
        if( _znet_node_supported_timer ) {
            return ZNET_ERR_BUSY;
        }
        // S0
        if( ( _znet_node_supported_timer = znet_timer_shoot( 12000, _znet_node_supported_timeout, NULL ) ) == NULL ) {
            return ZNET_ERR_NOMEM;
        }
        return znet_cc_security_supported_get( node_id, channel_id, NULL, NULL, encap );
    }

    if( _znet_node_supported_timer ) {
        return ZNET_ERR_BUSY;
    }
    // S2
    if( ( _znet_node_supported_timer = znet_timer_shoot( 5000, _znet_node_supported_timeout, NULL ) ) == NULL ) {
        return ZNET_ERR_NOMEM;
    }
    return znet_cc_security_2_supported_get( node_id, channel_id, NULL, NULL, encap );
}
