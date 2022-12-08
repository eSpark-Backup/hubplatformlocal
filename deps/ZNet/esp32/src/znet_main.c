/**
 * @file znet_main.c
 * @date 16 Mar 2018
 * @brief TBD.
 */

/// INFO: pair
#include "znet_main.h"

/// INFO: crt & system
#include <assert.h>
#include <string.h>

/// INFO: public
#include <znet/znet.h>
#include "znet_private.h"  //added by PM
#include "znet_lib_cc_management.h"

/// INFO: private
#include "znet_log.h"
#include "znet_store.h"
#include "znet_timer.h"

/// INFO: internal
#include "heap.h"
#include <znet_lib.h>
#include <znet_lib_fc.h>
#include <ZNetActivities.h>
#include <S2_wrap.h>
#include <s2_classcmd.h>
#include <s2_keystore.h>
#include <Security_Scheme0.h>
#include <security_layer.h>
#include <ctype.h>
#include <stdio.h>

#define STARTUP_TIMER_DURATION 3000
#define FAILED_STARTUP_TIMER_DURATION 10000

/**
 * The received INIF is received from a foreign HomeID.
 * This status is sent to ApplicationControllerUpdate in rxStatus parameter value.
 */
#define RECEIVE_STATUS_FOREIGN_HOMEID 0b10000000

const znet_callbacks_t* znet_cb = NULL;

ZNet_t znet_instance;
uint8_t znet_init_finished = 0;
static SerialPort_t serialPort;
uint8_t learn_mode = 0;
uint8_t Z_WAVE_LIBRARY_TYPE = 0;
uint8_t Z_WAVE_PROTOCOL_VERSION = 0;
uint8_t Z_WAVE_PROTOCOL_SUB_VERSION = 0;
uint8_t SIS_id = ZNET_NODE_ID_ANY;
inclusion_controller_info_t ic_info = { ZNET_NODE_ID_ANY, Encapsulation_None };
bool forced_requests_timer_run = false;
bool serialApiStarted = false;
static void _startup_timeout( znet_timer_t* timer, void* arg, int destroy );
/**
 * Timer that initiates startup mechanism.
 * Used to:
 *  - guarantee the startup even if SerialAPIStarted was not received by some reason
 *  - postpone startup if multiple SerialAPIStarted frames received
 */
static znet_timer_t* _startup_timer = NULL;
static uint8_t reset_counter = 0;

////////////////////////////////////////////////////////////////////////

void znet_print_d_i_cc_event( const ZWDeviceChangeType type,
                              uint8_t node_id,
                              uint8_t instance_id,
                              uint8_t command_id,
                              void* arg ) {
    ZNET_LOGE( "%s: stub: need implement!\n", __func__ );
}

void znet_terminated_event( const ZNet znet ) {
    ZNET_LOGE( "%s: stub: need implement!\n", __func__ );
}

void learn_ok( uint8_t node_id, bool multichannel ) {
    ZNET_LOGI( "%s: request node info after set from a primary controller has been received\n", __func__ );

    if( multichannel && znet_cb->node_cmd_multichannel_association_set_done ) {
        znet_cb->node_cmd_multichannel_association_set_done(
            ZNET_ERR_OK, node_id, ZNET_CHANNEL_ID_ROOT, 1, znet_cb->arg );
    }
    else if( !multichannel && znet_cb->node_cmd_association_set_done ) {
        znet_cb->node_cmd_association_set_done( ZNET_ERR_OK, node_id, ZNET_CHANNEL_ID_ROOT, 1, znet_cb->arg );
    }
}

void znet_cc_inclusion_receive( const ZFunction func, uint8_t node_id, int cc_data_len, const uint8_t* cc_data ) {
    assert( cc_data );
    assert( cc_data_len >= 4 );

    if( node_id >= ZNET_NODE_ID_MIN && node_id <= ZNET_NODE_ID_MAX ) {
        if( cc_data[1] == INITIATE ) {
            znet_inclution_controller_step_id_t child_node_id = cc_data[2];
            znet_inclution_controller_status_t step_id = cc_data[3];

            znet_set_ic_info( node_id, func->_security );
            if( znet_cb->node_cmd_inclusion_controller_initiate )
                znet_cb->node_cmd_inclusion_controller_initiate(
                    ZNET_ERR_OK, node_id, child_node_id, step_id, znet_cb->arg );
        }
        else if( cc_data[1] == COMPLETE ) {
            znet_inclution_controller_step_id_t step_id = cc_data[2];
            znet_inclution_controller_status_t status = cc_data[3];

            if( znet_cb->node_cmd_inclusion_controller_complete )
                znet_cb->node_cmd_inclusion_controller_complete( ZNET_ERR_OK, node_id, step_id, status, znet_cb->arg );
        }
    }
}

encap_type_t get_security_by_id( uint8_t node_id ) {
    return znet_utils_get_security_encap_type_by_node_id( node_id );
}

static bool is_cc_included_in_unsecure_nif( znet_node_id_t node_id, uint8_t cc ) {
    const size_t nif_size = znet_store_node_info_recommended_size();
    uint8_t nif_buff[nif_size];
    memset( nif_buff, 0, sizeof( nif_buff ) );

    znet_nodeinfo_t* node_info = (znet_nodeinfo_t*)&nif_buff;
    if( znet_store_node_info_get( node_id, node_info, sizeof( nif_buff ) ) ) {
        return false;
    }

    for( uint8_t i = 0; i < node_info->commands_count; ++i ) {
        if( node_info->commands[i] == cc ) {
            return true;
        }
    }
    return false;
}

encap_type_t get_security_by_id_and_cc( znet_node_id_t node_id, znet_node_channel_id_t channel, uint8_t cc ) {
    // Basic CC (0x20) must be secure for S2 nodes, unless the opposite is defined by a NIF
    if( cc == 0x20 ) {
        encap_type_t node_security = get_security_by_id( node_id );
        if( is_cc_included_in_unsecure_nif( node_id, cc ) || node_security == Encapsulation_Sec_0 ) {
            return Encapsulation_None;
        }
        if( znet_cb->check_node_supports_command_class( node_id, channel, cc, znet_cb->arg ) ) {
            return node_security;
        }
    }
    return znet_utils_get_security_encap_type_by_node_id_and_command( node_id, cc );
}

bool is_node_added( uint8_t node_id ) {
    if( znet_cb->node_cmd_is_added ) {
        return znet_cb->node_cmd_is_added( node_id, znet_cb->arg ) == ZNET_ERR_OK;
    }
    return false;
}

#if ZNET_SUPPORT_PENDING_COMMANDS_FOR_SLEEP_NODES
int node_is_wakeup( uint8_t node_id ) {
#if 1
    return znet_node_is_wakeup( node_id );
#else
    int result = 0;
    znet_nodeinfo_t* node_info = NULL;
    size_t node_info_size = 0;

    if( znet_node_info( node_id, NULL, &node_info_size ) )  // get size
        goto done;
    if( ( node_info = (znet_nodeinfo_t*)Heap_Alloc( node_info_size ) ) == NULL )  // alloc
        goto done;
    if( znet_node_info( node_id, node_info, &node_info_size ) )  // get info
        goto done;

    if( !( node_info->capability & ZNET_NODEINFO_CAPABILITY_LISTENING ||
           node_info->flirs_delay & ZNET_NODEINFO_SECURITY_FLIRS_1000 ||
           node_info->flirs_delay & ZNET_NODEINFO_SECURITY_FLIRS_250 ) ) {
        result = !0;
    }

done:
    if( node_info )
        Heap_Free( node_info );

    return result;
#endif
}
#endif

uint8_t CC_present_in_NIF_by_ID( uint8_t node_id, uint8_t CC ) {
    return ZNET_ERR_OK == znet_node_is_support_command( node_id, CC );
}

void zwave_plus_info_get() {
    if( znet_cb->node_cmd_zwave_plus_info_get )
        znet_cb->node_cmd_zwave_plus_info_get( znet_cb->arg );
}

void indicator_identify() {
    if( znet_cb->node_cmd_indicator_identify )
        znet_cb->node_cmd_indicator_identify( znet_cb->arg );
}

void znet_cc_device_reset_localy_report( const ZFunction func,
                                         uint8_t node_id,
                                         int cc_data_len,
                                         const uint8_t* cc_data ) {
    assert( cc_data );
    if( node_id >= ZNET_NODE_ID_MIN && node_id <= ZNET_NODE_ID_MAX ) {
        znet_store_node_info_clean( node_id );
        association_remove_node( node_id );

        if( znet_cb->node_cmd_device_reset_localy_result )
            znet_cb->node_cmd_device_reset_localy_result( 0, node_id, znet_cb->arg );
    }
}

////////////////////////////////////////////////////////////////////////
// ApplicationNodeUpdate
void NodeAddCallback( int cc_data_len, const uint8_t* cc_data ) {
    assert( cc_data );
    assert( cc_data_len >= 5 );
    assert( cc_data[2] == 0x40 /* UPDATE_STATE_ADD_DONE */ );
    assert( cc_data[4] >= 3 );
    assert( cc_data_len >= 5 + cc_data[4] );

    const size_t command_classes_size = cc_data[4] > 3 ? cc_data[4] - 3 /* b + g + s */ : 0;
    const size_t buff_size = sizeof( znet_nodeinfo_t ) + command_classes_size * sizeof( znet_command_class_t );

    size_t real_buff_size = znet_store_node_info_recommended_size();
    assert( real_buff_size >= buff_size );

    uint8_t buff[real_buff_size];
    memset( buff, 0, sizeof( buff ) );

    znet_nodeinfo_t* node_info = (znet_nodeinfo_t*)&buff;
#if 0
    znet_store_node_info_get( cc_data[3], node_info, sizeof( buff ) );
    if( cc_data[3] == node_info->node_id )
        return; // known !!!
    memset( buff, 0, sizeof( buff ) );
#endif

    /// TODO: investigate how to get security

    node_info->node_id = cc_data[3];
    node_info->security = ZNET_NODEINFO_SECURITY_NONE;  // unknown !!!
    node_info->basic = cc_data[5];
    node_info->generic = cc_data[6];
    node_info->specific = cc_data[7];
    node_info->commands_count = command_classes_size;
    for( size_t i = 0; i < command_classes_size; ++i ) {
        node_info->commands[i] = cc_data[8 + i];
    }

    /// TODO: check returned result from store
    znet_store_node_info_set( node_info );

    if( znet_cb->node_add_result )
        znet_cb->node_add_result( ZNET_ERR_OK, node_info, znet_cb->arg );
}

void NodeDeleteCallback( int cc_data_len, const uint8_t* cc_data ) {
    assert( cc_data );
    assert( cc_data_len >= 5 );
    assert( cc_data[2] == 0x20 /* UPDATE_STATE_DELETE_DONE */ );
    assert( cc_data_len >= 5 + cc_data[4] );

    const size_t command_classes_size = cc_data[4] > 3 ? cc_data[4] - 3 /* b + g + s */ : 0;
    const size_t buff_size = sizeof( znet_nodeinfo_t ) + command_classes_size * sizeof( znet_command_class_t );

    size_t real_buff_size = znet_store_node_info_recommended_size();
    assert( real_buff_size >= buff_size );

    uint8_t buff[real_buff_size];
    memset( buff, 0, sizeof( buff ) );

    znet_nodeinfo_t* node_info = (znet_nodeinfo_t*)&buff;
#if 0
    znet_store_node_info_get( cc_data[3], node_info, sizeof( buff ) );
    if( cc_data[3] != node_info->node_id )
        return; // unknown !!!
    memset( buff, 0, sizeof( buff ) );
#endif

    node_info->node_id = cc_data[3];
    node_info->security = ZNET_NODEINFO_SECURITY_NONE;  // unknown !!!
    if( cc_data[4] >= 3 ) {
        node_info->basic = cc_data[5];
        node_info->generic = cc_data[6];
        node_info->specific = cc_data[7];
    }
    node_info->commands_count = command_classes_size;
    for( size_t i = 0; i < command_classes_size; ++i ) {
        node_info->commands[i] = cc_data[8 + i];
    }

    /// TODO: check returned result from store
    znet_store_node_info_clean( node_info->node_id );

    if( znet_cb->node_rem_result )
        znet_cb->node_rem_result( 0, node_info, znet_cb->arg );
}

void NodeUpdateSISCallback( int cc_data_len, const uint8_t* cc_data ) {
    assert( cc_data );
    assert( cc_data_len >= 5 );
    assert( cc_data[2] == 0x10 /* UPDATE_STATE_SUC_ID */ );

    /// TODO: investigate ->
    /// S2_wrap.c:sec2_event_handler
    /// s2_inclusion.c:s2_inclusion_challenge_response
    ///
    ///         if( learn_mode && ( MY_NODE_ID != SIS_id ) &&
    ///             ( CC_present_in_NIF_by_ID( SIS_id,
    ///             COMMAND_CLASS_INCLUSION_CONTROLLER ) ||
    ///             ( SIS_id == 0 ) ) )
    ///
    /// call CC_present_in_NIF_by_ID( SIS_id, COMMAND_CLASS_INCLUSION_CONTROLLER )
    /// valid only after callback NodeUpdateCallback for SIS_id

    SIS_id = cc_data[3];
    ZNET_LOGD( "%s SIS_id = %d\n", __func__, SIS_id );

    if( znet_cb->sis_updated )
        znet_cb->sis_updated( 0, SIS_id, znet_cb->arg );
}

////////////////////////////////////////////////////////////////////////
// NodeRequestFailedCallback
void NodeRequestFailedCallback( int cc_data_len, const uint8_t* cc_data ) {
    assert( cc_data );
    assert( cc_data_len >= 4 );

    const size_t buff_size = sizeof( znet_nodeinfo_t );

    size_t real_buff_size = znet_store_node_info_recommended_size();
    assert( real_buff_size >= buff_size );

    uint8_t buff[real_buff_size];
    memset( buff, 0, sizeof( buff ) );

    znet_nodeinfo_t* node_info = (znet_nodeinfo_t*)&buff;
    node_info->node_id = cc_data[3];

    if( znet_cb->node_update_info_result )
        znet_cb->node_update_info_result( ZNET_ERR_FAULT, cc_data[3], node_info, znet_cb->arg );
}

////////////////////////////////////////////////////////////////////////
void ICACHE_FLASH_ATTR UpdateNodeAndHomeID( uint32_t home_id, uint8_t node_id ) {
    ZNET_LOGD( "%s MY_NODE_ID %d, node_id %d\n", __func__, MY_NODE_ID, node_id );
    DEVICE_HOME_ID = home_id;
    if( MY_NODE_ID != node_id ) {
        MY_NODE_ID = node_id;
    }

    if( MY_NODE_ID != 1 ) {
        learn_mode = 1;
    }
    else {
        learn_mode = 0;
    }
}

static void _znet_node_replace_if_failed_S2_cb( uint8_t status, uint8_t dnode, uint8_t security_flag ) {
    ZNET_LOGD( "%s status=%d, dnode=%d, arg=0x%X\n", __func__, status, dnode, security_flag );

    if( dnode < ZNET_NODE_ID_MIN || dnode > ZNET_NODE_ID_MAX )
        return;

    size_t buff_size = znet_store_node_info_recommended_size();

    uint8_t buff[buff_size];
    memset( buff, 0, sizeof( buff ) );

    znet_nodeinfo_t* node_info = (znet_nodeinfo_t*)&buff;
    if( znet_store_node_info_get( dnode, node_info, sizeof( buff ) ) || node_info->node_id != dnode ) {
        return;
    }
    if( security_flag & NODE_FLAG_SECURITY2_ACCESS ) {
        node_info->security = ZNET_NODEINFO_SECURITY_S2_ACCESS;
    }
    else if( security_flag & NODE_FLAG_SECURITY2_AUTHENTICATED ) {
        node_info->security = ZNET_NODEINFO_SECURITY_S2_AUTH;
    }
    else if( security_flag & NODE_FLAG_SECURITY2_UNAUTHENTICATED ) {
        node_info->security = ZNET_NODEINFO_SECURITY_S2_UNAUTH;
    }
    else if( security_flag & NODE_FLAG_SECURITY0 ) {
        node_info->security = ZNET_NODEINFO_SECURITY_S0;
    }
    /// TODO: check returned result from store
    znet_store_node_info_set( node_info );

    znet_node_request_info( node_info->node_id, node_info->security );  // update NIF, before security update

    if( znet_cb->node_replace_if_failed_result )
        znet_cb->node_replace_if_failed_result( ZNET_ERR_OK, node_info, znet_cb->arg );
}

static void _znet_node_replace_if_failed_S0_cb( uint8_t status, uint8_t dnode, uint8_t security_flag ) {
    if( dnode < ZNET_NODE_ID_MIN || dnode > ZNET_NODE_ID_MAX )
        return;
    ZNET_LOGD( "%s status=%d, dnode=%d, arg=0x%X\n", __func__, status, dnode, security_flag );

    size_t buff_size = znet_store_node_info_recommended_size();

    uint8_t buff[buff_size];
    memset( buff, 0, sizeof( buff ) );

    znet_nodeinfo_t* node_info = (znet_nodeinfo_t*)&buff;
    if( znet_store_node_info_get( dnode, node_info, sizeof( buff ) ) || node_info->node_id != dnode ) {
        return;
    }
    if( security_flag ) {
        node_info->security = ZNET_NODEINFO_SECURITY_S0;
    }
    /// TODO: check returned result from store
    znet_store_node_info_set( node_info );

    znet_node_request_info( node_info->node_id, node_info->security );  // update NIF, before security update

    if( znet_cb->node_replace_if_failed_result )
        znet_cb->node_replace_if_failed_result( ZNET_ERR_OK, node_info, znet_cb->arg );
}

static void _znet_node_replace_if_failed_get_protocol_info_cb( ZFunction func,
                                                               void* arg,
                                                               ZFuncFailures_e reason,
                                                               const void* dataPtr,
                                                               size_t dataSize ) {
    assert( dataPtr );
    assert( dataSize >= 6 );

    const znet_node_id_t node_id = func->_node_id;
    if( node_id < ZNET_NODE_ID_MIN || node_id > ZNET_NODE_ID_MAX )
        return;

    const uint8_t* data = (const uint8_t*)dataPtr;
    if( !data || dataSize < 6 )
        return;

    const size_t buff_size = znet_store_node_info_recommended_size();
    uint8_t buff[buff_size];
    memset( buff, 0, sizeof( buff ) );

    znet_nodeinfo_t* node_info = (znet_nodeinfo_t*)&buff;
    if( znet_store_node_info_get( node_id, node_info, sizeof( buff ) ) )
        return;

    if( node_info->flags & ZMET_NODEINFO_FLAGS_GETPROTOINFO ) {
        if( data[0] & 0x80 )
            node_info->capability |= ZNET_NODEINFO_CAPABILITY_LISTENING;
        else
            node_info->capability &= ~ZNET_NODEINFO_CAPABILITY_LISTENING;

        if( data[1] & 0x80 )
            node_info->security |= ZNET_NODEINFO_SECURITY_OPT_FUNC;
        else
            node_info->security &= ~ZNET_NODEINFO_SECURITY_OPT_FUNC;

        if( data[1] & 0x40 )
            node_info->capability |= ZNET_NODEINFO_SECURITY_FLIRS_1000;
        else
            node_info->capability &= ~ZNET_NODEINFO_SECURITY_FLIRS_1000;

        if( data[1] & 0x20 )
            node_info->capability |= ZNET_NODEINFO_SECURITY_FLIRS_250;
        else
            node_info->capability &= ~ZNET_NODEINFO_SECURITY_FLIRS_250;

        node_info->flags &= ~ZMET_NODEINFO_FLAGS_GETPROTOINFO;
        /// TODO: check returned result from store
        znet_store_node_info_set( node_info );
    }
}

void NodeUpdateCallback( int cc_data_len, const uint8_t* cc_data ) {
    assert( cc_data );
    assert( cc_data_len >= 5 );
    assert( cc_data[2] == 0x84 /* UPDATE_STATE_NODE_INFO_RECEIVED */ );
    assert( cc_data_len >= 5 + cc_data[4] );

    const size_t command_classes_size = cc_data[4] > 3 ? cc_data[4] - 3 /* b + g + s */ : 0;
    const size_t buff_size = sizeof( znet_nodeinfo_t ) + command_classes_size * sizeof( znet_command_class_t );
    size_t real_buff_size = znet_store_node_info_recommended_size();
    assert( real_buff_size >= buff_size );

    uint8_t buff[real_buff_size];
    memset( buff, 0, sizeof( buff ) );

    znet_nodeinfo_t* node_info = (znet_nodeinfo_t*)&buff;

    node_info->node_id = cc_data[3];
    node_info->basic = cc_data[5];
    node_info->generic = cc_data[6];
    node_info->specific = cc_data[7];

    node_info->commands_count = command_classes_size;
    for( size_t i = 0; i < command_classes_size; ++i ) {
        node_info->commands[i] = cc_data[8 + i];
    }

    uint8_t my_node_security = znet_utils_get_highest_security( S2_get_assigned_keys() );
    ZNET_LOGD( "%s MY_NODE_ID %d, MY_NODE_SECURITY %d\n", __func__, MY_NODE_ID, my_node_security );
    const bool is_security_supported =
        ZNET_ERR_OK == znet_node_info_is_support_command( node_info, ZNET_COMMAND_CLASS_SECURITY );
    const bool is_security_2_supported =
        ZNET_ERR_OK == znet_node_info_is_support_command( node_info, ZNET_COMMAND_CLASS_SECURITY_2 );
    if( is_security_supported || is_security_2_supported ) {
        if( MY_NODE_ID != 1 ) {
            if( is_security_2_supported ) {
                node_info->security = my_node_security;
            }
            else if( is_security_supported ) {
                if( my_node_security == ZNET_NODEINFO_SECURITY_NONE )
                    node_info->security = my_node_security;
                else
                    node_info->security = ZNET_NODEINFO_SECURITY_S0;
            }
        }
        else {
            if( znet_cb->node_cmd_is_added &&
                znet_cb->node_cmd_is_added( node_info->node_id, znet_cb->arg ) == ZNET_ERR_OK ) {
                node_info->security = znet_cb->node_cmd_security_get( node_info->node_id, znet_cb->arg );
            }
            else {
                if( is_security_2_supported ) {
                    node_info->security = ZNET_NODEINFO_SECURITY_S2_ACCESS;  // maximum
                }
                else if( is_security_supported ) {
                    node_info->security = ZNET_NODEINFO_SECURITY_S0;
                }
                else {
                    node_info->security = ZNET_NODEINFO_SECURITY_NONE;
                }
                if( znet_cb->node_cmd_security_set ) {
                    znet_cb->node_cmd_security_set( node_info->node_id, node_info->security, znet_cb->arg );
                }
            }
        }
    }
    ZNET_LOGD( "%s node security %d\n", __func__, node_info->security );

    if( node_info->flags & ZNET_NODEINFO_FLAGS_REPLACED ) {
        node_info->flags &= ~ZNET_NODEINFO_FLAGS_REPLACED;

        node_info->flags |= ZMET_NODEINFO_FLAGS_GETPROTOINFO;
        /// TODO: check returned result from store
        znet_store_node_info_set( node_info );

        znet_fc_get_node_protocol_info( node_info->node_id, _znet_node_replace_if_failed_get_protocol_info_cb, NULL );

        if( is_security_2_supported ) {
            node_info->security = ZNET_NODEINFO_SECURITY_S2_ACCESS;  // maximum
        }
        else if( is_security_supported ) {
            node_info->security = ZNET_NODEINFO_SECURITY_S0;
        }

        if( znet_should_delegate_inclusion_to_sis( node_info->node_id ) ) {
            if( znet_cb->node_add_result )
                znet_cb->node_add_result( ZNET_ERR_OK, node_info, znet_cb->arg );
            inclusion_controller_info_t sis_info = {
                znet_get_sis_node_id(), znet_utils_get_security_encap_type_by_node_id( znet_get_sis_node_id() )
            };
            znet_node_inclusion_initiate(
                sis_info, node_info->node_id, ZNET_INCLUSION_CONTROLLER_PROXY_INCLUSION_REPLACE );
        }
        else if( is_security_2_supported ) {
            if( znet_cb->node_add_result )
                znet_cb->node_add_result( ZNET_ERR_OK, node_info, znet_cb->arg );
            sec2_start_add_node( node_info->node_id, _znet_node_replace_if_failed_S2_cb );
        }
        else if( is_security_supported ) {
            if( znet_cb->node_add_result )
                znet_cb->node_add_result( ZNET_ERR_OK, node_info, znet_cb->arg );
            security_add_begin( node_info->node_id,
                                ( node_info->basic == ZNET_BASIC_CLASS_STATIC_CONTROLLER ),
                                _znet_node_replace_if_failed_S0_cb );
        }
        else {
            if( znet_cb->node_add_result )
                znet_cb->node_add_result( ZNET_ERR_OK, node_info, znet_cb->arg );
        }

        if( znet_cb->node_replace_if_failed_result ) {
            znet_cb->node_replace_if_failed_result( ZNET_ERR_OK, node_info, znet_cb->arg );
        }
    }

    znet_store_node_info_set( node_info );

    ZNet_WakingNode( node_info->node_id, !0 );

    if( znet_cb->node_update_info_result )
        znet_cb->node_update_info_result( 0, node_info->node_id, node_info, znet_cb->arg );

    if( ic_info.id == ZNET_NODE_ID_ANY && node_info->security != ZNET_NODEINFO_SECURITY_NONE ) {
        // TODO: implement logic of respond waiting in c++ part via request manager and callbacks
        znet_node_cmd_security_supported_get( node_info->node_id, ZNET_CHANNEL_ID_ROOT );
    }
}

////////////////////////////////////////////////////////////////////////
// flash -> store -> flash

void save_fl( int dis, char* str, uint32_t len ) {
    if( dis == 0x3000 )  //  S2 keystore
    {
        znet_store_S2_keystore_set( str, len );
        return;
    }

    ZNET_LOGE( "%s: stub: need implement!\n", __func__ );
}

uint8_t read_fl( int dis, char* str, int len ) {
    if( dis == 0x3000 )  //  S2 keystore
    {
        return znet_store_S2_keystore_get( str, len );
    }

    ZNET_LOGE( "%s: stub: need implement!\n", __func__ );
    return 1;
}

////////////////////////////////////////////////////////////////////////

bool get_known_S0_key( uint8_t* buf ) {
    assert( buf );

    if( znet_cb->get_known_S0_key ) {
        return znet_cb->get_known_S0_key( buf, znet_cb->arg );
    }

    return false;
}

////////////////////////////////////////////////////////////////////////

#define _ZNET_INIT_GET_HOME_ID_SIZE 8
#define _ZNET_INIT_GET_SUC_NODE_ID_SIZE 8

static void _znet_init_get_suc_node_id_cb( ZFunction func,
                                           void* arg,
                                           ZFuncFailures_e reason,
                                           const void* dataPtr,
                                           size_t dataSize ) {
    if( FUNC_OK == reason && dataSize ) {
        assert( dataPtr );
        if( dataPtr == NULL )
            return;
        const uint8_t* data = (const uint8_t*)dataPtr;
        uint8_t buff[5] = {};
        buff[2] = 0x10 /*UPDATE_STATE_SUC_ID*/;
        buff[3] = data[0];  // SIS ID
        if( buff[3] == ZNET_NODE_ID_ANY && MY_NODE_ID == 1 ) {
            buff[3] = MY_NODE_ID;
            znet_fc_set_suc_node_id( buff[3], NULL, NULL );
        }

        NodeUpdateSISCallback( sizeof( buff ), buff );
    }
    znet_init_finished = 1;

    if( znet_cb->init_finish )
        znet_cb->init_finish( ZNET_ERR_OK, znet_cb->arg );
}
static uint8_t chanel_id = 0;
static void _znet_fc_set_threshold_cb( ZFunction func,
                                       void* arg,
                                       ZFuncFailures_e reason,
                                       const void* dataPtr,
                                       size_t dataSize ) {
    if( chanel_id > 1 ) {
        if( ZNET_ERR_OK != znet_fc_get_suc_node_id( _znet_init_get_suc_node_id_cb, NULL ) ) {
            ZNET_LOGE( "get suc node id failed!\n" );
        }
    }
    else {
        ++chanel_id;
        if( ZNET_ERR_OK != znet_fc_set_threshold( chanel_id, 78, _znet_fc_set_threshold_cb, NULL ) ) {
            ZNET_LOGE( "set threshold failed!\n" );
        }
    }
}

static void _znet_fc_library_version_cb( ZFunction func,
                                         void* arg,
                                         ZFuncFailures_e reason,
                                         const void* dataPtr,
                                         size_t dataSize ) {
    if( reason == FUNC_OK && dataPtr ) {
        const char* library_version = (const char*)dataPtr;
        ZNET_LOGD( "Library version:  %s\n", library_version );
        Z_WAVE_LIBRARY_TYPE = ( (const uint8_t*)dataPtr )[dataSize - 1];
        ZNET_LOGD( "Library type:  %d\n", Z_WAVE_LIBRARY_TYPE );

        while( *library_version != '\0' && !isdigit( *library_version ) ) {
            ++library_version;
        }

        if( strchr( library_version, '.' ) == NULL ) {
            ZNET_LOGW( "Unexpected library version format\n" );
        }

        sscanf( library_version, "%hhu.%hhu", &Z_WAVE_PROTOCOL_VERSION, &Z_WAVE_PROTOCOL_SUB_VERSION );

        if( znet_cb->library_info ) {
            const size_t buff_size = sizeof( znet_library_info_t ) + dataSize - 1;
            uint8_t buff[buff_size];
            memset( buff, 0, buff_size );

            znet_library_info_t* info = (znet_library_info_t*)&buff;
            info->size = dataSize - 2;
            memcpy( info->name, dataPtr, info->size );

            znet_cb->library_info( ZNET_ERR_OK, info, Z_WAVE_LIBRARY_TYPE, znet_cb->arg );
        }
    }
    chanel_id = 0;
    if( ZNET_ERR_OK != znet_fc_set_threshold( chanel_id, 78, _znet_fc_set_threshold_cb, NULL ) ) {
        ZNET_LOGE( "set threshold failed!\n" );
    }
}

void _znet_init_application_node_info_cb( ZFunction func,
                                          void* arg,
                                          ZFuncFailures_e reason,
                                          const void* dataPtr,
                                          size_t dataSize ) {
    if( FUNC_OK == reason ) {
        if( ZNET_ERR_OK != znet_fc_library_version( _znet_fc_library_version_cb, NULL ) ) {
            ZNET_LOGE( "get library version failed!\n" );
        }
    }
    else {
        ZNET_LOGE( "SerialAPIApplicationNodeInformation failed: 0x%x\n", reason );
        if( reset_counter < 5 ) {
            ++reset_counter;
            serialApiStarted = false;
            if( znet_cb->reset_zwave_chip ) {
                znet_cb->reset_zwave_chip( znet_cb->arg );
            }
        }
        else {
            if( znet_cb->init_finish ) {
                znet_cb->init_finish( ZNET_ERR_FAULT, znet_cb->arg );
            }
            _startup_timer = znet_timer_shoot( FAILED_STARTUP_TIMER_DURATION, _startup_timeout, NULL );
        }
    }
}

static void _znet_init_get_home_id_cb( ZFunction func,
                                       void* arg,
                                       ZFuncFailures_e reason,
                                       const void* dataPtr,
                                       size_t dataSize ) {
    if( FUNC_OK == reason ) {
        assert( dataPtr );
        if( dataPtr == NULL )
            return;

        SIS_id = ZNET_NODE_ID_ANY;

        if( MY_NODE_ID != 1 ) {
            learn_mode = 1;
            znet_fc_serial_api_application_node_info( TRUE, _znet_init_application_node_info_cb, NULL );
        }
        else {
            learn_mode = 0;
            znet_fc_serial_api_application_node_info( FALSE, _znet_init_application_node_info_cb, NULL );
        }

        ZNET_LOGI( "Home ID: 0x%X Node ID: %hhu\n", DEVICE_HOME_ID, MY_NODE_ID );
        security_init();  /// TODO: investigate for refactoring global variables
    }
    else {
        ZNET_LOGE( "%s: failed, reset_counter = %d\n", __func__, reset_counter );
        if( znet_cb->reset_zwave_chip )
            znet_cb->reset_zwave_chip( znet_cb->arg );
        serialApiStarted = false;
        if( reset_counter < 5 ) {
            ++reset_counter;
        }
        else {
            if( reset_counter == 5 ) {
                if( znet_cb->init_finish )
                    znet_cb->init_finish( ZNET_ERR_FAULT, znet_cb->arg );
                ++reset_counter;
            }
            _startup_timer = znet_timer_shoot( FAILED_STARTUP_TIMER_DURATION, _startup_timeout, NULL );
        }
    }
}

static void znet_restore_state( void ) {
    associations_load();
    multichannel_associations_load();

    // Todo: expand as needed
}

znet_error_t znet_init( const znet_callbacks_t* callbacks ) {
    /// TODO: check all callbacks
    if( !callbacks ) {
        goto failed;
    }

    znet_cb = callbacks;

    if( !Heap_Init() ) {
        ZNET_LOGE( "Heap_Init failed!\n" );
        goto failed;
    }

    if( !Lists_Init() ) {
        ZNET_LOGE( "Lists_Init failed!\n" );
        goto failed;
    }

    if( !SystemTime_Init() ) {
        ZNET_LOGE( "SystemTime_Init failed!\n" );
        goto failed;
    }

    if( !SerialPort_Init( &serialPort, 2, Handshakings_None, 115200, DataBits_8, FALSE, Parities_None, StopBits_1 ) ) {
        ZNET_LOGE( "SerialPort_Init failed!\n" );
        goto failed_serial_port_init;
    }

    if( ZNet_Init( &serialPort ) != NoError ) {
        ZNET_LOGE( "ZNet_Init failed!\n" );
        goto failed_znet_init;
    }

    /// TODO: dead code?
    ZNet_AddCallback( DeviceAdded | DeviceRemoved | InstanceAdded | InstanceRemoved | CommandAdded | CommandRemoved,
                      znet_print_d_i_cc_event,
                      NULL );

    if( !SerialPort_Open( &serialPort ) ) {
        ZNET_LOGE( "SerialPort_Open failed!\n" );
        goto failed_serial_port_open;
    }

    if( ZNet_Start( znet_terminated_event ) != NoError ) {
        ZNET_LOGE( "ZNet_Start failed!\n" );
        goto failed_znet_start;
    }

    if( znet_store_init() ) {
        ZNET_LOGE( "znet_store_init failed!\n" );
        goto failed_znet_store_init;
    }

    znet_restore_state();

    ZNetRXActivity_init();

    serialApiStarted = false;
    // Will time out in case if SerialAPIStarted is not received by some reason
    _startup_timer = znet_timer_shoot( STARTUP_TIMER_DURATION, _startup_timeout, NULL );

    return ZNET_ERR_OK;

/// TODO: free resource if failed (heap, list, system_time)
failed_znet_store_init:
    ZNet_Stop( &znet_instance );
failed_znet_start:
    SerialPort_Close( &serialPort );
failed_serial_port_open:
    ZNet_DeInit( &znet_instance );
failed_znet_init:
    SerialPort_DeInit( &serialPort );
failed_serial_port_init:
failed:
    znet_cb = NULL;
    return ZNET_ERR_FAULT;
}

void znet_deinit( void ) {
    ZNet_DeInit( &znet_instance );
    znet_cb = NULL;
}

void znet_proc( void ) {
    if( !znet_cb ) {
        ZNET_LOGE( "Library not initialized!\n" );
        return;
    }

    ZNetRXActivityFunc();
}

// set default /////////////////////////////////////////////////////////
/// INFO: ZW_SetDefault

#define _ZNET_SET_DEFAULT_GET_HOME_ID_SIZE 8

static void _znet_set_default_get_home_id_cb( ZFunction func,
                                              void* arg,
                                              ZFuncFailures_e reason,
                                              const void* dataPtr,
                                              size_t dataSize ) {
    if( reason != FUNC_OK ) {
        if( znet_cb->set_default ) {
            znet_cb->set_default( znet_get_error( reason ), znet_cb->arg );
        }
        return;
    }
    assert( dataPtr );
    if( dataPtr == NULL )
        return;
    const uint8_t* data = (const uint8_t*)dataPtr;
    /// INFO: set global variables in S2_wrap.h
    DEVICE_HOME_ID = (int32_t)data[3];
    DEVICE_HOME_ID |= (uint32_t)data[2] << 8;
    DEVICE_HOME_ID |= (uint32_t)data[1] << 16;
    DEVICE_HOME_ID |= (uint32_t)data[0] << 24;

    MY_NODE_ID = data[4];
    assert( MY_NODE_ID == 1 );

    ZNET_LOGI( "Home ID: 0x%X Node ID: %hhu\n", DEVICE_HOME_ID, MY_NODE_ID );

    znet_fc_serial_api_application_node_info( FALSE, NULL, NULL );

    SIS_id = MY_NODE_ID;
    znet_fc_set_suc_node_id( SIS_id, NULL, NULL );

    security_init();  /// TODO: investigate for refactoring global variables

    if( znet_cb->set_default ) {
        znet_cb->set_default( ZNET_ERR_OK, znet_cb->arg );
    }
}

static void _znet_set_default_cb( ZFunction func,
                                  void* arg,
                                  ZFuncFailures_e reason,
                                  const void* dataPtr,
                                  size_t dataSize ) {
    if( FUNC_OK != reason ) {
        if( znet_cb->set_default )
            znet_cb->set_default( znet_get_error( reason ), znet_cb->arg );

        return;
    }

    if( znet_cb->set_default_started )
        znet_cb->set_default_started( znet_cb->arg );

    znet_error_t err_code = znet_store_reset();

    if( err_code == ZNET_ERR_OK ) {
        uint8_t buff[znet_store_node_info_recommended_size()];
        memset( buff, 0, sizeof( buff ) );
        znet_nodeinfo_t* node_info = (znet_nodeinfo_t*)&buff;
        node_info->node_id = MY_NODE_ID;
        node_info->security = ZNET_NODEINFO_SECURITY_S2_ACCESS;  // maximum
        node_info->basic = BASIC_TYPE_STATIC_CONTROLLER;
        node_info->generic = GENERIC_TYPE_STATIC_CONTROLLER;
        node_info->specific = SPECIFIC_TYPE_GATEWAY;
        node_info->commands_count = 0;
        err_code = znet_store_node_info_set( node_info );
    }

    if( ZNET_ERR_OK != znet_fc_get_home_id( &_znet_set_default_get_home_id_cb, NULL ) ) {
        znet_cb->set_default( FUNC_FAILURE_GENERAL, znet_cb->arg );
        ZNET_LOGE( "get home id failed!\n" );
        return;
    }
}

static void _znet_device_reset_locally_notification_cb( ZFunction func,
                                                        void* arg,
                                                        ZFuncFailures_e reason,
                                                        const void* dataPtr,
                                                        size_t dataSize ) {
    if( FUNC_OK != reason ) {
        ZNET_LOGE( "Failed to send DEVICE_RESET_LOCALLY_NOTIFICATION: 0x%04X\n", reason );
    }

    if( arg ) {
        ( ( void ( * )( void ) )( arg ) )();
    }
}

static void _znet_send_device_reset_locally_notification( uint8_t node_id, void ( *callback )( void ) ) {
    encap_type_t encap = znet_utils_get_security_encap_type_by_node_id( node_id );

    znet_cc_device_reset_localy_notification(
        node_id, ZNET_CHANNEL_ID_ROOT, _znet_device_reset_locally_notification_cb, callback, encap );
}

static void _znet_device_reset_locally_notification_sequential_cb( ZFunction func,
                                                                   void* arg,
                                                                   ZFuncFailures_e reason,
                                                                   const void* dataPtr,
                                                                   size_t dataSize ) {
    uint8_t nodeId, endpoint;
    ZNet_CheckCallback( func, reason, &nodeId, &endpoint );

    if( nodeId < ZNET_NODE_ID_MIN || nodeId > ZNET_NODE_ID_MAX ) {
        return;
    }

    if( FUNC_OK != reason ) {
        ZNET_LOGE( "Failed to send DEVICE_RESET_LOCALLY_NOTIFICATION: 0x%04X\n", reason );
    }

    uint8_t* node_ids;
    const size_t nodes_count = association_get_nodes_in_lifeline_group( &node_ids );

    for( uint8_t node_id_index = 0; node_id_index < nodes_count - 1; ++node_id_index ) {
        if( node_ids[node_id_index] == nodeId ) {
            _znet_send_device_reset_locally_notification_sequential( node_ids[node_id_index + 1], arg );
            return;
        }
    }

    if( arg ) {
        ( ( void ( * )( void ) )( arg ) )();
    }
}

void _znet_send_device_reset_locally_notification_sequential( uint8_t node_id, void ( *callback )( void ) ) {
    encap_type_t encap = znet_utils_get_security_encap_type_by_node_id( node_id );

    znet_cc_device_reset_localy_notification(
        node_id, ZNET_CHANNEL_ID_ROOT, _znet_device_reset_locally_notification_sequential_cb, callback, encap );
}

void znet_send_device_reset_locally_notifications( void ( *on_finish )( void ) ) {
    uint8_t* node_ids;
    const size_t nodes_count = association_get_nodes_in_lifeline_group( &node_ids );

    // No associations - invoke the callback immediately
    if( nodes_count == 0 ) {
        if( on_finish )
            on_finish();

        return;
    }

    _znet_send_device_reset_locally_notification_sequential( node_ids[0], on_finish );
}

znet_error_t znet_set_default( void ) {
    ZNET_LOGI( "%s\n", __func__ );

    if( !znet_cb ) {
        ZNET_LOGE( "Library not initialized!\n" );
        return ZNET_ERR_FAULT;
    }
    znet_set_ic_info( ZNET_NODE_ID_ANY, Encapsulation_None );
    association_reset();  // move it to callback?

    return znet_fc_set_default( _znet_set_default_cb, NULL );
}

static void _znet_set_default_raw_cb( ZFunction func,
                                      void* arg,
                                      ZFuncFailures_e reason,
                                      const void* dataPtr,
                                      size_t dataSize ) {
    ZNET_LOGI( "%s\n", __func__ );
    if( znet_cb->set_default_raw_result ) {
        znet_cb->set_default_raw_result( reason == FUNC_OK ? ZNET_ERR_OK : znet_get_error( reason ), znet_cb->arg );
    }
}

znet_error_t znet_set_default_raw() {
    ZNET_LOGI( "%s\n", __func__ );
    znet_set_ic_info( ZNET_NODE_ID_ANY, Encapsulation_None );
    return znet_fc_set_default( _znet_set_default_raw_cb, NULL );
}

static void _znet_serial_api_soft_reset_cb( ZFunction func,
                                            void* arg,
                                            ZFuncFailures_e reason,
                                            const void* dataPtr,
                                            size_t dataSize ) {
    ZNET_LOGI( "%s\n", __func__ );
}

znet_error_t znet_serial_api_soft_reset( void ) {
    if( !znet_cb ) {
        ZNET_LOGE( "Library not initialized!\n" );
        return ZNET_ERR_FAULT;
    }

    return znet_fc_serial_api_soft_reset( _znet_serial_api_soft_reset_cb, NULL );
}

// send node information ///////////////////////////////////////////////
/// INFO: ZW_SendNodeInformation

znet_error_t znet_send_node_info( void ) {
    if( !znet_cb ) {
        ZNET_LOGE( "Library not initialized!\n" );
        return ZNET_ERR_FAULT;
    }

    return znet_fc_send_node_information( FALSE, NULL, NULL );
}

// assign suc return route /////////////////////////////////////////////
/// INFO: ZW_AssignSUCReturnRoute

znet_error_t znet_assign_suc_return_route( znet_node_id_t node_id ) {
    if( !znet_cb ) {
        ZNET_LOGE( "Library not initialized!\n" );
        return ZNET_ERR_FAULT;
    }

    if( node_id < ZNET_NODE_ID_MIN || node_id > ZNET_NODE_ID_MAX ) {
        return ZNET_ERR_INVAL;
    }

    return znet_fc_assign_suc_return_route( node_id, NULL, NULL );
}

// network update //////////////////////////////////////////////////////
/// INFO: ZW_RequestNetWorkUpdate

static void _znet_network_update_cb( ZFunction func,
                                     void* arg,
                                     ZFuncFailures_e reason,
                                     const void* dataPtr,
                                     size_t dataSize ) {
    if( FUNC_OK != reason ) {
        /// TODO: implement ZW_SUC_UPDATE_OVERFLOW for start make a replication

        if( znet_cb->network_update )
            znet_cb->network_update( znet_get_error( reason ), znet_cb->arg );

        return;
    }

    if( znet_cb->network_update )
        znet_cb->network_update( 0, znet_cb->arg );
}

znet_error_t znet_network_update( void ) {
    if( !znet_cb ) {
        ZNET_LOGE( "Library not initialized!\n" );
        return ZNET_ERR_FAULT;
    }
    /// TODO: check (main controller || sis in network: SIS_id != 0)
    return znet_fc_request_network_update( _znet_network_update_cb, NULL );
}

static void _znet_request_neighbor_cb( ZFunction func,
                                       void* arg,
                                       ZFuncFailures_e reason,
                                       const void* dataPtr,
                                       size_t dataSize ) {
    if( znet_cb->request_neighbor_result )
        znet_cb->request_neighbor_result(
            reason == FUNC_OK ? ZNET_ERR_OK : znet_get_error( reason ), func->_node_id, znet_cb->arg );
}

znet_error_t znet_request_node_neighbor( znet_node_id_t node_id ) {
    if( !znet_cb ) {
        ZNET_LOGE( "Library not initialized!\n" );
        return ZNET_ERR_FAULT;
    }
    return znet_fc_request_node_neighbor_update( node_id, _znet_request_neighbor_cb, NULL );
}

znet_error_t znet_get_background_RSSI() {
    if( !znet_cb ) {
        ZNET_LOGE( "Library not initialized!\n" );
        return ZNET_ERR_FAULT;
    }
    return znet_fc_get_background_RSSI( NULL, NULL );
}

// S2 //////////////////////////////////////////////////////////////////

static void _znet_get_private_s2_dsk_cb( ZFunction func,
                                         void* arg,
                                         ZFuncFailures_e reason,
                                         const void* dataPtr,
                                         size_t dataSize ) {
    if( znet_cb->private_s2_dsk ) {
        znet_cb->private_s2_dsk( reason == FUNC_OK ? ZNET_ERR_OK : znet_get_error( reason ), dataPtr, dataSize );
    }
}

znet_error_t znet_get_private_s2_dsk() {
    if( !znet_cb ) {
        ZNET_LOGE( "Library not initialized!\n" );
        return ZNET_ERR_FAULT;
    }
    const size_t S2_DSK_KEY_SIZE = 32;
    return znet_fc_nvr_get_value( 0x23, S2_DSK_KEY_SIZE, _znet_get_private_s2_dsk_cb, NULL );
}

/// INFO: answer for STATE_KEYS_REQ
void znet_node_s2_select_keys( znet_S2_keys_t selected_keys ) {
    S2_response_incl( STATE_KEYS_RES, selected_keys, NULL );
}

/// INFO: answer for STATE_PUB_KEY_REQ
void znet_node_s2_response_pub_key( const znet_S2_pub_key_t* pub_key ) {
    if( !pub_key /*|| pub_key->size != 2*/ )
        return;

    S2_response_incl( STATE_PUB_KEY_RES, 0, pub_key->key );
}

/// INFO: query (answer by S2_response_incl)
void S2_request_incl( uint8_t state, uint8_t keys, uint8_t node_id, uint8_t* pub_key ) {
    ZNET_LOGD( "%s state(0x%X) keys(0x%X)\n", __func__, state, keys );
    switch( state ) {
    case STATE_KEYS_REQ:
        if( znet_cb->node_s2_select_keys ) {
            znet_cb->node_s2_select_keys( keys, node_id, znet_cb->arg );
        }
        else {
            S2_response_incl(
                STATE_KEYS_RES, ( keys & ( KEY_CLASS_S0 | KEY_CLASS_S2_UNAUTHENTICATED | KEY_CLASS_S2_CSA ) ), NULL );
        }
        break;

    case STATE_PUB_KEY_REQ: {
        const size_t key_size = ( keys & KEY_CLASS_S2_CSA ) ? 16 : 14;
        const size_t key_shift = ( keys & KEY_CLASS_S2_CSA ) ? 0 : 2;
        const size_t buff_size = sizeof( znet_S2_pub_key_t ) + key_size;

        uint8_t buff[buff_size];
        memset( buff, 0, buff_size );

        znet_S2_pub_key_t* key = (znet_S2_pub_key_t*)buff;

        key->size = key_size;
        for( size_t i = 0; i < key_size; ++i ) {
            key->key[i] = pub_key[i + key_shift];
        }

        if( keys & ( KEY_CLASS_S2_AUTHENTICATED | KEY_CLASS_S2_ACCESS ) ) {
            if( znet_cb->node_s2_request_pub_key ) {
                znet_cb->node_s2_request_pub_key( node_id, keys, key, znet_cb->arg );
            }
        }
        else {
            S2_response_incl( STATE_PUB_KEY_RES, keys, pub_key );
        }
    } break;

    case STATE_SHOW_PUB_KEY:
        if( keys & ( SECURITY_2_SECURITY_2_CLASS_2 | SECURITY_2_SECURITY_2_CLASS_1 ) ) {
            const size_t key_size = 16;
            const size_t buff_size = sizeof( znet_S2_pub_key_t ) + key_size;

            uint8_t buff[buff_size];
            memset( buff, 0, buff_size );

            znet_S2_pub_key_t* key = (znet_S2_pub_key_t*)buff;

            key->size = key_size;
            for( size_t i = 0; i < key_size; ++i ) {
                key->key[i] = pub_key[i];
            }

            if( znet_cb->node_s2_show_pub_key ) {
                znet_cb->node_s2_show_pub_key( keys, key, znet_cb->arg );
            }
        }
        break;
    }
}

// node add ////////////////////////////////////////////////////////////
/// INFO: ZW_AddNodeToNetwork

static void _znet_node_add_S2_cb( uint8_t status, uint8_t dnode, uint8_t security_flag ) {
    const size_t buff_size = znet_store_node_info_recommended_size();
    uint8_t buff[buff_size];
    memset( buff, 0, sizeof( buff ) );
    znet_nodeinfo_t* node_info = (znet_nodeinfo_t*)&buff;
    ZNET_LOGD( "%s status=%d, dnode=%d, arg=0x%X\n", __func__, status, dnode, security_flag );

    if( znet_store_node_info_get( dnode, node_info, sizeof( buff ) ) ) {
        return;
    }

    if( status != ZNET_ERR_OK ) {
        if( znet_cb->node_add_security_failed ) {
            znet_cb->node_add_security_failed( node_info->node_id, znet_cb->arg );
        }
        return;
    }

    node_info->security = ZNET_NODEINFO_SECURITY_NONE;
    if( security_flag & NODE_FLAG_SECURITY2_ACCESS ) {
        node_info->security = ZNET_NODEINFO_SECURITY_S2_ACCESS;
    }
    else if( security_flag & NODE_FLAG_SECURITY2_AUTHENTICATED ) {
        node_info->security = ZNET_NODEINFO_SECURITY_S2_AUTH;
    }
    else if( security_flag & NODE_FLAG_SECURITY2_UNAUTHENTICATED ) {
        node_info->security = ZNET_NODEINFO_SECURITY_S2_UNAUTH;
    }
    else if( security_flag & NODE_FLAG_SECURITY0 ) {
        node_info->security = ZNET_NODEINFO_SECURITY_S0;
    }

    /// TODO: check returned result from store
    znet_store_node_info_set( node_info );

    if( znet_cb->node_cmd_security_set ) {
        znet_cb->node_cmd_security_set( node_info->node_id, node_info->security, znet_cb->arg );
    }

    znet_node_request_info( node_info->node_id, node_info->security );  // update NIF, before security update
}

static void _znet_node_add_S0_cb( uint8_t status, uint8_t dnode, uint8_t security_flag ) {
    const size_t buff_size = znet_store_node_info_recommended_size();
    uint8_t buff[buff_size];
    memset( buff, 0, sizeof( buff ) );
    znet_nodeinfo_t* node_info = (znet_nodeinfo_t*)&buff;
    ZNET_LOGD( "%s status=%d, dnode=%d, arg=0x%X\n", __func__, status, dnode, security_flag );
    if( znet_store_node_info_get( dnode, node_info, sizeof( buff ) ) ) {
        return;
    }

    if( status != ZNET_ERR_OK ) {
        if( znet_cb->node_add_security_failed ) {
            znet_cb->node_add_security_failed( node_info->node_id, znet_cb->arg );
        }
        return;
    }

    node_info->security = ZNET_NODEINFO_SECURITY_NONE;
    if( security_flag ) {
        node_info->security = ZNET_NODEINFO_SECURITY_S0;
    }

    /// TODO: check returned result from store
    znet_store_node_info_set( node_info );

    if( znet_cb->node_cmd_security_set ) {
        znet_cb->node_cmd_security_set( node_info->node_id, node_info->security, znet_cb->arg );
    }

    if( znet_should_delegate_inclusion_to_sis( node_info->node_id ) ) {
        if( znet_node_is_have_command(
                znet_get_sis_node_id(), ZNET_CHANNEL_ID_ROOT, ZNET_COMMAND_CLASS_INCLUSION_CONTROLLER ) ) {
            inclusion_controller_info_t ic_info = {
                znet_get_sis_node_id(), znet_utils_get_security_encap_type_by_node_id( znet_get_self_node_id() )
            };
            znet_node_inclusion_complete(
                ic_info, ZNET_INCLUSION_CONTROLLER_S0_INCLUSION, ZNET_INCLUSION_CONTROLLER_STEP_OK );
        }
        else {
            znet_node_request_info( node_info->node_id, node_info->security );  // update NIF, before security update
            if( znet_cb->node_add_result )
                znet_cb->node_add_result( ZNET_ERR_OK, node_info, znet_cb->arg );
        }
    }
    else {
        znet_node_request_info( node_info->node_id, node_info->security );  // update NIF, before security update
        if( znet_cb->node_add_result )
            znet_cb->node_add_result( ZNET_ERR_OK, node_info, znet_cb->arg );
    }
}

static void _znet_node_add_get_protocol_info_cb( ZFunction func,
                                                 void* arg,
                                                 ZFuncFailures_e reason,
                                                 const void* dataPtr,
                                                 size_t dataSize ) {
    if( dataPtr == NULL || func == NULL || dataSize < 6 ) {
        ZNET_LOGE( "%s fail\n", __func__ );
        return;
    }

    const znet_node_id_t node_id = func->_node_id;
    if( node_id < ZNET_NODE_ID_MIN || node_id > ZNET_NODE_ID_MAX )
        return;

    const uint8_t* data = (const uint8_t*)dataPtr;
    if( !data || dataSize < 6 )
        return;

    const size_t buff_size = znet_store_node_info_recommended_size();
    uint8_t buff[buff_size];
    memset( buff, 0, sizeof( buff ) );

    znet_nodeinfo_t* node_info = (znet_nodeinfo_t*)&buff;
    if( znet_store_node_info_get( node_id, node_info, sizeof( buff ) ) )
        return;

    if( node_info->flags & ZMET_NODEINFO_FLAGS_GETPROTOINFO ) {
        if( data[0] & 0x80 )
            node_info->capability |= ZNET_NODEINFO_CAPABILITY_LISTENING;
        else
            node_info->capability &= ~ZNET_NODEINFO_CAPABILITY_LISTENING;

        if( data[1] & 0x80 )
            node_info->security |= ZNET_NODEINFO_SECURITY_OPT_FUNC;
        else
            node_info->security &= ~ZNET_NODEINFO_SECURITY_OPT_FUNC;

        if( data[1] & 0x40 )
            node_info->capability |= ZNET_NODEINFO_SECURITY_FLIRS_1000;
        else
            node_info->capability &= ~ZNET_NODEINFO_SECURITY_FLIRS_1000;

        if( data[1] & 0x20 )
            node_info->capability |= ZNET_NODEINFO_SECURITY_FLIRS_250;
        else
            node_info->capability &= ~ZNET_NODEINFO_SECURITY_FLIRS_250;

        node_info->flags &= ~ZMET_NODEINFO_FLAGS_GETPROTOINFO;
        /// TODO: check returned result from store
        znet_store_node_info_set( node_info );
    }

    uint8_t security = ZNET_NODEINFO_SECURITY_NONE;

    if( !znet_node_info_is_support_command( node_info, ZNET_COMMAND_CLASS_SECURITY_2 ) ) {
        // S2
        security = node_info->security = ZNET_NODEINFO_SECURITY_S2_ACCESS;  // maximum
    }
    else if( !znet_node_info_is_support_command( node_info, ZNET_COMMAND_CLASS_SECURITY ) ) {
        // S0
        uint8_t my_node_security = S2_get_assigned_keys();
        if( my_node_security & Encapsulation_Sec_0 ) {
            security = node_info->security = ZNET_NODEINFO_SECURITY_S0;
        }
    }

    if( znet_should_delegate_inclusion_to_sis( node_info->node_id ) ) {
        if( znet_cb->node_add_result )
            znet_cb->node_add_result( ZNET_ERR_OK, node_info, znet_cb->arg );
    }
    // RT:00.11.002D.1: force S2 bootstrapping during Smart Start inclusion
    else if( security == ZNET_NODEINFO_SECURITY_S2_ACCESS || node_info->smart_start_data.data != NULL ) {
        if( znet_cb->node_add_result )
            znet_cb->node_add_result( ZNET_ERR_OK, node_info, znet_cb->arg );
        sec2_start_add_node( node_info->node_id, _znet_node_add_S2_cb );
    }
    else if( security == ZNET_NODEINFO_SECURITY_S0 ) {
        if( znet_cb->node_add_result )
            znet_cb->node_add_result( ZNET_ERR_OK, node_info, znet_cb->arg );
        security_add_begin(
            node_info->node_id, ( node_info->basic == ZNET_BASIC_CLASS_STATIC_CONTROLLER ), _znet_node_add_S0_cb );
    }
    else {
        if( znet_cb->node_add_result )
            znet_cb->node_add_result( ZNET_ERR_OK, node_info, znet_cb->arg );
    }
}

static void _znet_node_add_cb_one( ZFunction func,
                                   void* arg,
                                   ZFuncFailures_e reason,
                                   const void* dataPtr,
                                   size_t dataSize ) {
    // stop inclusion
    znet_fc_add_node_to_network( ZNET_ADD_NODE_STOP, NULL, NULL );

    if( FUNC_OK != reason ) {
        if( znet_cb->node_add_result )
            znet_cb->node_add_result( znet_get_error( reason ), NULL, znet_cb->arg );
        return;
    }

    assert( dataPtr );
    assert( dataSize >= 6 );

    const uint8_t* _znet_node_add_data = (const uint8_t*)dataPtr;
    if( !_znet_node_add_data || dataSize < 6 ) {
        if( znet_cb->node_add_result )
            znet_cb->node_add_result( ZNET_ERR_INVAL, NULL, znet_cb->arg );
        return;
    }

    /// TODO: parse data (listen, secure, etc.)
    if( dataSize && _znet_node_add_data[5] )  // length && generic
    {
        const size_t command_classes_size = _znet_node_add_data[3] > 3 ? _znet_node_add_data[3] - 3 /* b + g + s */ : 0;
        const size_t buff_size = sizeof( znet_nodeinfo_t ) + command_classes_size * sizeof( znet_command_class_t );
        uint8_t buff[buff_size];
        memset( buff, 0, buff_size );

        znet_nodeinfo_t* node_info = (znet_nodeinfo_t*)&buff;
        node_info->node_id = _znet_node_add_data[2];
        node_info->security = ZNET_NODEINFO_SECURITY_NONE;
        node_info->basic = _znet_node_add_data[4];
        node_info->generic = _znet_node_add_data[5];
        node_info->specific = _znet_node_add_data[6];
        node_info->commands_count = command_classes_size;
        node_info->smart_start_data.data = arg;

        for( size_t i = 0; i < command_classes_size; ++i ) {
            node_info->commands[i] = _znet_node_add_data[7 + i];
        }

        node_info->flags |= ZMET_NODEINFO_FLAGS_GETPROTOINFO;

        /// TODO: check returned result from store
        znet_store_node_info_set( node_info );

        ZNet_WakingNode( node_info->node_id, !0 );

        znet_fc_get_node_protocol_info( node_info->node_id, _znet_node_add_get_protocol_info_cb, NULL );
    }
    else {
        if( znet_cb->node_add_result )
            znet_cb->node_add_result( ZNET_ERR_INVAL, NULL, znet_cb->arg );
    }
}

znet_error_t znet_node_add( void ) {
    if( !znet_cb ) {
        ZNET_LOGE( "Library not initialized!\n" );
        return ZNET_ERR_FAULT;
    }
    znet_set_ic_info( ZNET_NODE_ID_ANY, Encapsulation_None );
    return znet_fc_add_node_to_network( ZNET_ADD_NODE_START, _znet_node_add_cb_one, NULL );
}

znet_error_t znet_node_add_dsk_to_network( const uint8_t* dsk_derived_home_id ) {
    if( !znet_cb ) {
        ZNET_LOGE( "Library not initialized!\n" );
        return ZNET_ERR_FAULT;
    }
    znet_smart_start_data_t smart_start_data;
    memcpy( smart_start_data.nhid, dsk_derived_home_id, sizeof( smart_start_data.nhid ) );
    return znet_fc_add_node_dsk_to_network( dsk_derived_home_id, _znet_node_add_cb_one, smart_start_data.data );
}

znet_error_t znet_smart_start_listen_mode_enable() {
    if( !znet_cb ) {
        ZNET_LOGE( "Library not initialized!\n" );
        return ZNET_ERR_FAULT;
    }
    ZNET_LOGI( "%s\n", __func__ );
    return znet_fc_add_node_to_network( ZNET_ADD_NODE_SMART_START, NULL, NULL );
}

znet_error_t znet_smart_start_listen_mode_disable() {
    if( !znet_cb ) {
        ZNET_LOGE( "Library not initialized!\n" );
        return ZNET_ERR_FAULT;
    }
    ZNET_LOGI( "%s\n", __func__ );
    return znet_fc_add_node_to_network( ZNET_ADD_NODE_STOP, NULL, NULL );
}

static void _znet_node_add_break_cb( ZFunction func,
                                     void* arg,
                                     ZFuncFailures_e reason,
                                     const void* dataPtr,
                                     size_t dataSize ) {
    if( znet_cb->node_add_break_result )
        znet_cb->node_add_break_result(
            FUNC_OK == reason ? ZNET_ERR_OK : znet_get_error( reason ), NULL, znet_cb->arg );
}

znet_error_t znet_node_add_break( void ) {
    if( !znet_cb ) {
        ZNET_LOGE( "Library not initialized!\n" );
        return ZNET_ERR_FAULT;
    }

    // stop inclusion
    znet_error_t err = znet_fc_add_node_to_network( ZNET_ADD_NODE_STOP, _znet_node_add_break_cb, NULL );

    return err;
}

void znet_node_sec2_abort_join() {
    if( !znet_cb ) {
        ZNET_LOGE( "Library not initialized!\n" );
        return;
    }

    sec2_abort_join();
}

// node rem ////////////////////////////////////////////////////////////
/// INFO: ZW_RemoveNodeFromNetwork

#define _ZNET_NODE_REM_BUFF_SIZE 64

static void _znet_node_rem_break_cb( ZFunction func,
                                     void* arg,
                                     ZFuncFailures_e reason,
                                     const void* dataPtr,
                                     size_t dataSize ) {
    // Silently consume any error
    if( znet_cb->node_rem_break_result )
        znet_cb->node_rem_break_result( ZNET_ERR_OK, NULL, znet_cb->arg );
}

znet_error_t znet_node_rem_break( void ) {
    if( !znet_cb ) {
        ZNET_LOGE( "Library not initialized!\n" );
        return ZNET_ERR_FAULT;
    }

    return znet_fc_remove_node_from_network( FALSE, _znet_node_rem_break_cb, NULL );
}

static void _znet_node_rem_cb( ZFunction func,
                               void* arg,
                               ZFuncFailures_e reason,
                               const void* dataPtr,
                               size_t dataSize ) {
    znet_fc_remove_node_from_network( FALSE, NULL, NULL );
    if( FUNC_OK != reason ) {
        if( znet_cb->node_rem_result )
            znet_cb->node_rem_result( znet_get_error( reason ), NULL, znet_cb->arg );
        return;
    }

    assert( dataPtr );
    if( dataPtr == NULL )
        return;
    const uint8_t* data = (const uint8_t*)dataPtr;

    /// TODO: parse data
    if( dataSize && data[5] )  // length && generic
    {
        const size_t command_classes_size = data[3] > 3 ? data[3] - 3 /* b + g + s */ : 0;
        const size_t buff_size = sizeof( znet_nodeinfo_t ) + command_classes_size * sizeof( znet_command_class_t );
        uint8_t buff[buff_size];
        memset( buff, 0, buff_size );

        znet_nodeinfo_t* node_info = (znet_nodeinfo_t*)&buff;
        node_info->node_id = data[2];
        node_info->basic = data[4];
        node_info->generic = data[5];
        node_info->specific = data[6];
        node_info->commands_count = command_classes_size;

        for( size_t i = 0; i < command_classes_size; ++i ) {
            node_info->commands[i] = data[7 + i];
        }

        /// TODO: check returned result from store
        if( node_info->node_id >= ZNET_NODE_ID_MIN && node_info->node_id <= ZNET_NODE_ID_MAX ) {
            znet_store_node_info_clean( node_info->node_id );
            znet_store_security_supported_clean( node_info->node_id );
            association_remove_node( node_info->node_id );
        }

        if( znet_cb->node_rem_result )
            znet_cb->node_rem_result( 0, node_info, znet_cb->arg );
    }
    else {
        if( znet_cb->node_rem_result )
            znet_cb->node_rem_result( ZNET_ERR_INVAL, NULL, znet_cb->arg );
    }
}

static void _znet_node_rem_failed_cb( ZFunction func,
                                      void* arg,
                                      ZFuncFailures_e reason,
                                      const void* dataPtr,
                                      size_t dataSize ) {
    if( FUNC_OK != reason ) {
        if( znet_cb->node_rem_result )
            znet_cb->node_rem_result( znet_get_error( reason ), NULL, znet_cb->arg );
        return;
    }

    assert( dataPtr );
    if( dataPtr == NULL )
        return;
    const uint8_t* data = (const uint8_t*)dataPtr;

    /// TODO: parse data
    if( dataSize && data[5] )  // length && generic
    {
        const size_t command_classes_size = data[3] > 3 ? data[3] - 3 /* b + g + s */ : 0;
        const size_t buff_size = sizeof( znet_nodeinfo_t ) + command_classes_size * sizeof( znet_command_class_t );
        uint8_t buff[buff_size];
        memset( buff, 0, buff_size );

        znet_nodeinfo_t* node_info = (znet_nodeinfo_t*)&buff;
        node_info->node_id = data[2];

        if( func != NULL ) {
            if( func->_tx_frame._size > 4 ) {
                node_info->node_id = func->_node_id;
            }
        }

        /// TODO: check returned result from store
        if( node_info->node_id >= ZNET_NODE_ID_MIN && node_info->node_id <= ZNET_NODE_ID_MAX ) {
            znet_store_node_info_clean( node_info->node_id );
            znet_store_security_supported_clean( node_info->node_id );
        }

        if( znet_cb->node_rem_result ) {
            znet_cb->node_rem_result( 0, node_info, znet_cb->arg );
        }
    }
    else {
        if( znet_cb->node_rem_result )
            znet_cb->node_rem_result( ZNET_ERR_INVAL, NULL, znet_cb->arg );
    }
}

znet_error_t znet_node_rem( void ) {
    if( !znet_cb ) {
        ZNET_LOGE( "Library not initialized!\n" );
        return ZNET_ERR_FAULT;
    }

    znet_error_t err = znet_fc_remove_node_from_network( TRUE, _znet_node_rem_cb, NULL );

    return err;
}

znet_error_t znet_node_rem_failed_node( uint8_t node_id ) {
    if( !znet_cb ) {
        ZNET_LOGE( "Library not initialized!\n" );
        return ZNET_ERR_FAULT;
    }

    znet_error_t err = znet_fc_remove_failed_node( node_id, _znet_node_rem_failed_cb, NULL );

    return err;
}

// node replace if failed //////////////////////////////////////////////
/// INFO: ZW_ReplaceFailedNode

#define _ZNET_NODE_REPLACE_IF_FAILED_BUFF_SIZE 64

static void _znet_node_replace_if_failed_cb( ZFunction func,
                                             void* arg,
                                             ZFuncFailures_e reason,
                                             const void* dataPtr,
                                             size_t dataSize ) {
    if( FUNC_OK == reason ) {
        const znet_node_id_t node_id = func->_node_id;

        size_t buff_size = znet_store_node_info_recommended_size();

        uint8_t buff[buff_size];
        memset( buff, 0, sizeof( buff ) );

        znet_nodeinfo_t* node_info = (znet_nodeinfo_t*)&buff;
        if( znet_store_node_info_get( node_id, node_info, sizeof( buff ) ) ) {
            memset( buff, 0, sizeof( buff ) );
        }
        node_info->node_id = node_id;
        node_info->security = ZNET_NODEINFO_SECURITY_NONE;  /// INFO: drop security
        node_info->flags |= ZNET_NODEINFO_FLAGS_REPLACED;
        znet_store_node_info_set( node_info );

        /// INFO: request NIF && connect S0 or S2 if supported && call callback
        znet_fc_request_node_information( node_id, NULL, NULL );
        return;
    }

    if( znet_cb->node_replace_if_failed_result )
        znet_cb->node_replace_if_failed_result( ZNET_ERR_FAULT, NULL, znet_cb->arg );
}

znet_error_t znet_node_replace_if_failed( znet_node_id_t node_id ) {
    if( !znet_cb ) {
        ZNET_LOGE( "Library not initialized!\n" );
        return ZNET_ERR_FAULT;
    }

    if( node_id < ZNET_NODE_ID_MIN || node_id > ZNET_NODE_ID_MAX ) {
        return ZNET_ERR_INVAL;
    }

    return znet_fc_replace_failed_node( node_id, _znet_node_replace_if_failed_cb, NULL );
}

// node learn //////////////////////////////////////////////////////////
/// INFO: ZW_SetLearnMode

#define _ZNET_NODE_LEARN_BUFF_SIZE 64
static uint8_t _znet_node_learn_wait_Sx = 0;  /// Is not used !!!!!!!

#define _ZNET_NODE_LEARN_GET_HOME_ID_SIZE 8
#define _ZNET_NODE_LEARN_GET_SUC_NODE_ID_SIZE 8

static void _znet_node_learn_S0_cb( uint8_t status, uint8_t dnode, uint8_t unused ) {
    ZNET_LOGD( "%s status=%d, dnode=%d, arg=0x%X\n", __func__, status, dnode, 0 );

    if( ZNET_ERR_OK == status || _znet_node_learn_wait_Sx == 1 || dnode == 0 ) {
        ZNET_LOGD( "%s flag\n", __func__ );

        if( dnode ) {
            if( znet_cb->node_cmd_security_set )
                if( znet_cb->node_cmd_security_set( dnode, ZNET_NODEINFO_SECURITY_S0, znet_cb->arg ) )
                    ZNET_LOGE( "can`t set security for node with id = %d\n", dnode );

            if( znet_cb->node_learn_result )
                znet_cb->node_learn_result( ZNET_ERR_OK, dnode, znet_cb->arg );
        }
        _znet_node_learn_wait_Sx = 0;
    }
    else if( _znet_node_learn_wait_Sx == 2 ) {
        --_znet_node_learn_wait_Sx;
    }
}

static void _znet_node_learn_S2_cb( uint8_t status, uint8_t dnode, uint8_t security_flag ) {
    ZNET_LOGD( "%s status=%d, dnode=%d, arg=0x%X\n", __func__, status, dnode, security_flag );

    if( ZNET_ERR_OK == status || _znet_node_learn_wait_Sx == 1 ) {
        uint8_t security = ZNET_NODEINFO_SECURITY_NONE;
        if( security_flag & NODE_FLAG_SECURITY2_ACCESS ) {
            security = ZNET_NODEINFO_SECURITY_S2_ACCESS;
        }
        else if( security_flag & NODE_FLAG_SECURITY2_AUTHENTICATED ) {
            security = ZNET_NODEINFO_SECURITY_S2_AUTH;
        }
        else if( security_flag & NODE_FLAG_SECURITY2_UNAUTHENTICATED ) {
            security = ZNET_NODEINFO_SECURITY_S2_UNAUTH;
        }
        else if( security_flag & NODE_FLAG_SECURITY0 ) {
            security = ZNET_NODEINFO_SECURITY_S0;
        }

        if( znet_cb->node_cmd_security_set )
            if( znet_cb->node_cmd_security_set( dnode, security, znet_cb->arg ) )
                ZNET_LOGE( "can`t set security for node with id = %d\n", dnode );

        if( znet_cb->node_learn_result )
            znet_cb->node_learn_result( ZNET_ERR_OK, dnode, znet_cb->arg );

        _znet_node_learn_wait_Sx = 0;
    }
    else if( _znet_node_learn_wait_Sx == 2 ) {
        --_znet_node_learn_wait_Sx;
    }
}

static void _znet_node_learn_cb( ZFunction func,
                                 void* arg,
                                 ZFuncFailures_e reason,
                                 const void* dataPtr,
                                 size_t dataSize ) {
    znet_fc_set_learn_mode( ZW_SET_LEARN_MODE_DISABLE, NULL, NULL );

    const uint8_t* data = (const uint8_t*)dataPtr;
    if( FUNC_OK == reason && dataPtr != NULL && dataSize ) {
        if( NODE_STATUS_STARTED == data[1] ) {
            return;
        }
        // Note: only Node ID == 0 should be considered as exclusion event,
        // but for some reason on newer Z-Wave chips Node ID == 1
        else if( NODE_STATUS_DONE == data[1] && dataSize > 2 && ( data[2] == 0 || data[2] == 1 ) ) {
            if( znet_cb->node_learn_result )
                znet_cb->node_learn_result( ZNET_ERR_OK, 0, znet_cb->arg );
            return;
        }
        if( data[2] == MY_NODE_ID ) {
            znet_cb->node_learn_result( ZNET_ERR_OK, data[2], znet_cb->arg );
            return;
        }
    }

    if( FUNC_OK != reason ) {
        if( znet_cb->node_learn_result )
            znet_cb->node_learn_result( znet_get_error( reason ), 0, znet_cb->arg );

        learn_mode = 0;

        return;
    }

    // Reset all security keys on learn start
    // to avoid wrong security level
    keystore_network_key_clear( 0xFF );

    security_learn_begin( _znet_node_learn_S0_cb );
    S2_start_incl_timer( _znet_node_learn_S2_cb );

    if( data[0] )  // length
    {
        znet_store_reset();
        znet_cb->node_learn_result( ZNET_ERR_OK, data[2], znet_cb->arg );
        return;
    }
    else {
        if( znet_cb->node_learn_result )
            znet_cb->node_learn_result( ZNET_ERR_INVAL, 0, znet_cb->arg );
    }
}

static void _znet_node_learn_break_cb( ZFunction func,
                                       void* arg,
                                       ZFuncFailures_e reason,
                                       const void* dataPtr,
                                       size_t dataSize ) {
    if( znet_cb->node_learn_break_result )
        znet_cb->node_learn_break_result( znet_get_error( reason ), 0, znet_cb->arg );
}

znet_error_t znet_node_learn_break( void ) {
    if( !znet_cb ) {
        ZNET_LOGE( "Library not initialized!\n" );
        return ZNET_ERR_FAULT;
    }

    learn_mode = 0;
    return znet_fc_set_learn_mode( ZW_SET_LEARN_MODE_DISABLE, NULL, NULL );
}

znet_error_t znet_node_learn( void ) {
    if( !znet_cb ) {
        ZNET_LOGE( "Library not initialized!\n" );
        return ZNET_ERR_FAULT;
    }

    return znet_fc_set_learn_mode( ZW_SET_LEARN_MODE_CLASSIC, _znet_node_learn_cb, NULL );
}

union node_request_info_param {
    void* data;
    uint8_t security;
};

static void znet_node_request_info_cb( ZFunction func,
                                       void* arg,
                                       ZFuncFailures_e reason,
                                       const void* dataPtr,
                                       size_t dataSize ) {
    const znet_node_id_t node_id = func->_node_id;

    if( node_id < ZNET_NODE_ID_MIN || node_id > ZNET_NODE_ID_MAX )
        return;

    union node_request_info_param param;
    param.data = arg;

    if( znet_cb->node_update_info_result )
        znet_cb->node_update_info_result( znet_get_error( reason ), node_id, NULL, znet_cb->arg );

    if( param.security && znet_get_error( reason ) != ZNET_ERR_REQUEST_SENT_OK ) {
        // if znet_get_error( reason ) == ZNET_ERR_REQUEST_SENT_OK znet_node_cmd_security_supported_get will be sent on
        // node info report
        // TODO: implement logic of respond waiting in c++ part via request manager and callbacks
        znet_node_cmd_security_supported_get( node_id, ZNET_CHANNEL_ID_ROOT );
    }
}

znet_error_t znet_node_request_info( znet_node_id_t node_id, uint8_t security ) {
    if( !znet_cb ) {
        ZNET_LOGE( "Library not initialized!\n" );
        return ZNET_ERR_FAULT;
    }

    if( node_id < ZNET_NODE_ID_MIN || node_id > ZNET_NODE_ID_MAX ) {
        return ZNET_ERR_INVAL;
    }
    ZNET_LOGD( "%s node_id %d\n", __func__, node_id );

    union node_request_info_param param;
    param.data = NULL;
    param.security = security;

    return znet_fc_request_node_information( node_id, znet_node_request_info_cb, param.data );
}

static void znet_node_learn_nodes_cb( ZFunction func,
                                      void* arg,
                                      ZFuncFailures_e reason,
                                      const void* dataPtr,
                                      size_t dataSize ) {
    assert( dataPtr );
    assert( dataSize >= 5 );

    learn_mode = 0;
    const uint8_t* data = (const uint8_t*)dataPtr;
    if( data[2] != 29 ) {
        return;
    }

    if( znet_cb->node_list_result )
        znet_cb->node_list_result( 0, &data[3], znet_cb->arg );
}

znet_error_t znet_node_learn_nodes_request( void ) {
    if( !znet_cb ) {
        ZNET_LOGE( "Library not initialized!\n" );
        return ZNET_ERR_FAULT;
    }

    znet_fc_get_home_id( NULL, NULL );

    return znet_fc_serial_api_get_init_data( znet_node_learn_nodes_cb, NULL );
}

////////////////////////////////////////////////////////////////////////

#define ZNET_NODE_INFO_RECOMMENDED_SIZE 64

znet_error_t znet_node_info( znet_node_id_t node_id, znet_nodeinfo_t* node_info, size_t* node_info_size ) {
    if( !znet_cb ) {
        ZNET_LOGE( "Library not initialized!\n" );
        return ZNET_ERR_FAULT;
    }

    if( !node_info_size ) {
        return ZNET_ERR_INVAL;
    }

    if( !node_info ) {
        *node_info_size = ZNET_NODE_INFO_RECOMMENDED_SIZE;
        return ZNET_ERR_OK;
    }

    if( node_id < ZNET_NODE_ID_MIN || node_id > ZNET_NODE_ID_MAX ) {
        return ZNET_ERR_INVAL;
    }

    if( *node_info_size < ZNET_NODE_INFO_RECOMMENDED_SIZE ) {
        return ZNET_ERR_INVAL;
    }

    if( znet_store_node_info_get( node_id, node_info, *node_info_size ) ) {
        return ZNET_ERR_COMM;
    }

    if( node_info->generic && ( znet_node_info_size( node_info ) <= *node_info_size ) ) {
        *node_info_size = znet_node_info_size( node_info );
        return ZNET_ERR_OK;
    }

    return ZNET_ERR_FAULT;
}

#define ZNET_SECURITY_SUPPORTED_RECOMMENDED_SIZE 64

znet_error_t znet_security_supported( znet_node_id_t node_id,
                                      znet_security_supported_report_t* security_supported,
                                      size_t* security_supported_size ) {
    if( !znet_cb ) {
        ZNET_LOGE( "Library not initialized!\n" );
        return ZNET_ERR_FAULT;
    }

    if( !security_supported_size ) {
        return ZNET_ERR_INVAL;
    }

    if( !security_supported ) {
        *security_supported_size = ZNET_SECURITY_SUPPORTED_RECOMMENDED_SIZE;
        return ZNET_ERR_OK;
    }

    if( node_id < ZNET_NODE_ID_MIN || node_id > ZNET_NODE_ID_MAX ) {
        return ZNET_ERR_INVAL;
    }

    if( *security_supported_size < ZNET_SECURITY_SUPPORTED_RECOMMENDED_SIZE ) {
        return ZNET_ERR_INVAL;
    }

    if( znet_store_security_supported_get( node_id, security_supported, *security_supported_size ) ) {
        return ZNET_ERR_COMM;
    }

    if( znet_security_supported_size( security_supported ) <= *security_supported_size ) {
        *security_supported_size = znet_security_supported_size( security_supported );
        return ZNET_ERR_OK;
    }

    return ZNET_ERR_FAULT;
}

bool znet_should_delegate_inclusion_to_sis( znet_node_id_t node_id ) {
    const znet_node_id_t sis_id = znet_get_sis_node_id();
    return sis_id != ZNET_NODE_ID_ANY && sis_id != znet_get_self_node_id() && sis_id != node_id;
}

////////////////////////////////////////////////////////////////////////
static znet_timer_t* _znet_forced_request_timer = NULL;

static void _znet_forced_request_timeout( znet_timer_t* timer, void* arg, int destroy ) {
    if( !destroy ) {
        if( timer == _znet_forced_request_timer ) {
            if( znet_cb->forced_request_result ) {
                forced_requests_timer_run = false;
                znet_cb->forced_request_result( ZNET_ERR_OK, znet_cb->arg );
            }
        }
    }
    else if( timer == _znet_forced_request_timer ) {
        _znet_forced_request_timer = NULL;
    }
}

znet_error_t znet_forced_request_timer_create( int destroy ) {
    if( destroy == 0 ) {
        if( !znet_cb ) {
            ZNET_LOGE( "Library not initialized!\n" );
            return ZNET_ERR_FAULT;
        }

        if( !forced_requests_timer_run ) {
            if( ( _znet_forced_request_timer = znet_timer_shoot( 21600000, _znet_forced_request_timeout, NULL ) ) ==
                NULL ) {
                return ZNET_ERR_NOMEM;
            }
            else {
                ZNET_LOGD( "%s with 6 hours timeout!\n", __func__ );
            }
        }
        forced_requests_timer_run = true;
    }
    else if( _znet_forced_request_timer ) {
        _znet_forced_request_timer = NULL;
    }
    return ZNET_ERR_OK;
}

////////////////////////////////////////////////////////////////////////

znet_error_t znet_node_info_is_support_command( const znet_nodeinfo_t* node_info, znet_command_class_t command ) {
    if( !node_info ) {
        return ZNET_ERR_INVAL;
    }

    for( size_t i = 0; i < node_info->commands_count; ++i ) {
        if( command == node_info->commands[i] ) {
            return ZNET_ERR_OK;
        }
    }

    return ZNET_ERR_FAULT;
}

znet_error_t znet_node_is_support_command( znet_node_id_t node_id, znet_command_class_t command ) {
    int result = ZNET_ERR_FAULT;
    if( znet_cb->node_cmd_is_supported_cc )
        result = znet_cb->node_cmd_is_supported_cc( node_id, command, znet_cb->arg );
    return result;
}

bool znet_node_is_have_command( znet_node_id_t node_id,
                                znet_node_channel_id_t endpoint,
                                znet_command_class_t command ) {
    bool result = false;
    if( znet_cb->check_node_supports_command_class )
        result = znet_cb->check_node_supports_command_class( node_id, endpoint, command, znet_cb->arg );
    return result;
}

int znet_node_is_wakeup( znet_node_id_t node_id ) {
    int result = 0;
    if( znet_cb->node_cmd_is_wakeup )
        result = znet_cb->node_cmd_is_wakeup( node_id, znet_cb->arg );
    return result;
}

znet_error_t znet_node_info_is_support_security_command( const znet_security_supported_report_t* security_supported,
                                                         znet_command_class_t command ) {
    if( !security_supported ) {
        return ZNET_ERR_INVAL;
    }

    for( size_t i = 0; i < security_supported->commands_count; ++i ) {
        if( command == security_supported->commands[i] ) {
            return ZNET_ERR_OK;
        }
    }

    return ZNET_ERR_FAULT;
}

////////////////////////////////////////////////////////////////////////

uint8_t znet_utils_get_highest_security( uint8_t flags ) {
    uint8_t highest_security = ZNET_NODEINFO_SECURITY_NONE;
    if( flags & Encapsulation_Sec_2 ) {
        highest_security = ZNET_NODEINFO_SECURITY_S2_ACCESS;
    }
    else if( flags & Encapsulation_Sec_AUTHENTICATED ) {
        highest_security = ZNET_NODEINFO_SECURITY_S2_AUTH;
    }
    else if( flags & Encapsulation_Sec_UNAUTHENTICATED ) {
        highest_security = ZNET_NODEINFO_SECURITY_S2_UNAUTH;
    }
    else if( flags & Encapsulation_Sec_0 ) {
        highest_security = ZNET_NODEINFO_SECURITY_S0;
    }
    return highest_security;
}

uint8_t znet_utils_get_security_encap_type_by_node_id( znet_node_id_t node_id ) {
    encap_type_t result = Encapsulation_None;

    uint8_t security = 0;

    if( node_id == znet_get_self_node_id() ) {
        security = znet_utils_get_highest_security( S2_get_assigned_keys() );
    }
    else if( znet_cb->node_cmd_security_get ) {
        security = znet_cb->node_cmd_security_get( node_id, znet_cb->arg );
    }

    if( ( security & ZNET_NODEINFO_SECURITY_MASK ) == ZNET_NODEINFO_SECURITY_S2_ACCESS ) {
        result |= Encapsulation_Sec_2;
    }
    else if( ( security & ZNET_NODEINFO_SECURITY_MASK ) == ZNET_NODEINFO_SECURITY_S2_AUTH ) {
        result |= Encapsulation_Sec_AUTHENTICATED;
    }
    else if( ( security & ZNET_NODEINFO_SECURITY_MASK ) == ZNET_NODEINFO_SECURITY_S2_UNAUTH ) {
        result |= Encapsulation_Sec_UNAUTHENTICATED;
    }
    else if( ( security & ZNET_NODEINFO_SECURITY_MASK ) == ZNET_NODEINFO_SECURITY_S0 ) {
        result |= Encapsulation_Sec_0;
    }

    ZNET_LOGD( "get security (0x%X) by node_id (%d)\n", result, node_id );
    return result;
}

encap_type_t znet_utils_get_security_encap_type_by_node_id_and_command( znet_node_id_t node_id,
                                                                        znet_command_class_t command ) {
    /*
       TODO   A controlling node attempting to communicate with a supporting node SHOULD try using its highest
       Security Class. If the communication is not successful and the controlling node has been granted several
       Security Classes, the controlling node MAY try using any lower Security Classes.
    */
    encap_type_t result = Encapsulation_None;
    const encap_type_t encap = znet_utils_get_security_encap_type_by_node_id( node_id );
    if( encap & Encapsulation_Sec_2 || encap & Encapsulation_Sec_AUTHENTICATED ||
        encap & Encapsulation_Sec_UNAUTHENTICATED || encap & Encapsulation_Sec_0 ) {
        znet_security_supported_report_t* security_supported = NULL;
        size_t security_supported_size = 0;
        if( znet_security_supported( node_id, NULL, &security_supported_size ) )
            goto done;
        if( ( security_supported = (znet_security_supported_report_t*)Heap_Alloc( security_supported_size ) ) == NULL )
            goto done;
        if( znet_security_supported( node_id, security_supported, &security_supported_size ) )
            goto done;
        if( ZNET_ERR_OK == znet_node_info_is_support_security_command( security_supported, command ) ) {
            result |= encap;
        }
    done:
        if( security_supported )
            Heap_Free( security_supported );
    }

    return result;
}

znet_error_t znet_get_error( ZFuncFailures_e reason ) {
    switch( reason ) {
    case FUNC_OK:
        return ZNET_ERR_REQUEST_SENT_OK;

    case FUNC_FAILURE_ADD_TO_TX:
    case FUNC_FAILURE_ADD_TO_ACK:
    case FUNC_FAILURE_ADD_TO_RESPONSE:
    case FUNC_FAILURE_ADD_TO_CALLBACK:
        return ZNET_ERR_COMM;

    case FUNC_FAILURE_RXED_BUFFER_TOO_SHORT:
    case FUNC_FAILURE_RESPONSE_SIZE:
    case FUNC_FAILURE_RESPONSE_INVALID:
        return ZNET_ERR_COMM;

    case FUNC_FAILURE_CALLBACK_SIZE:
        return ZNET_ERR_CALLBACK;

    case FUNC_FAILURE_TIMEOUT:
        return ZNET_ERR_TIME;

    case FUNC_FAILURE_GENERAL:
        return ZNET_ERR_FAULT;

    case FUNC_FAILURE_NOMEM:
        return ZNET_ERR_NOMEM;

    case FUNC_FAILURE_NO_ACK:
        return ZNET_ERR_UNREACH;

    case FUNC_FAILURE_NETWORK_BUSY:
        return ZNET_ERR_NETWORK_BUSY;

    default:
        return ZNET_ERR_FAULT;
    }
}

znet_home_id_t znet_get_home_id() {
    return DEVICE_HOME_ID;
}

znet_node_id_t znet_get_self_node_id() {
    return MY_NODE_ID;
}

bool znet_if_support_security_level( uint8_t security_level ) {
    uint8_t keys = S2_get_assigned_keys();
    uint8_t level = Encapsulation_None;
    switch( security_level ) {
    case ZNET_NODEINFO_SECURITY_S2_UNAUTH:
        level |= Encapsulation_Sec_UNAUTHENTICATED;
        break;
    case ZNET_NODEINFO_SECURITY_S2_AUTH:
        level |= Encapsulation_Sec_AUTHENTICATED;
        break;
    case ZNET_NODEINFO_SECURITY_S2_ACCESS:
        level |= Encapsulation_Sec_2;
        break;
    case ZNET_NODEINFO_SECURITY_S0:
        level |= Encapsulation_Sec_0;
        break;
    default:
        break;
    }

    ZNET_LOGD( "%s  keys = 0x%X, level = 0x%X\n", __func__, keys, level );
    return keys & level;
}

znet_node_id_t znet_get_sis_node_id() {
    return SIS_id;
}

bool is_self_node_sis_or_primary() {
    return znet_get_self_node_id() == znet_get_sis_node_id() || znet_get_self_node_id() == 1;
}

uint8_t znet_get_my_highest_security() {
    return znet_utils_get_highest_security( S2_get_assigned_keys() );
}

void znet_set_ic_info( znet_node_id_t id, uint8_t sec ) {
    ZNET_LOGD( "%s id %d, sec %d\n", __func__, id, sec );
    ic_info.id = id;
    ic_info.security = sec;
}

inclusion_controller_info_t znet_get_ic_info() {
    return ic_info;
}

void znet_node_start_S0( uint8_t node_id, bool static_contoller ) {
    security_add_begin( node_id, static_contoller, _znet_node_add_S0_cb );
}

void znet_node_start_S2( uint8_t node_id ) {
    sec2_start_add_node( node_id, _znet_node_add_S2_cb );
}

void ICACHE_FLASH_ATTR StartSerialAPI( int len, uint8_t* buf ) {
    ZNET_LOGD( "SerialAPIStarted is received: postpone startup\n" );
    if( _startup_timer ) {
        znet_timer_kill( _startup_timer );
    }
    _startup_timer = znet_timer_shoot( STARTUP_TIMER_DURATION, _startup_timeout, NULL );
}

static void _startup_timeout( znet_timer_t* timer, void* arg, int destroy ) {
    if( !destroy ) {
        serialApiStarted = true;
        znet_fc_get_home_id( &_znet_init_get_home_id_cb, NULL );
    }
    else if( timer == _startup_timer ) {
        _startup_timer = NULL;
    }
}

void AddNodeInvokedCallback() {
    znet_cb->node_add_invoked( znet_cb->arg );
}

void RemNodeInvokedCallback() {
    znet_cb->node_rem_invoked( znet_cb->arg );
}

void LearnModeInvokedCallback() {
    znet_cb->node_learn_invoked( znet_cb->arg );
}

/**
 * If Controller has been put into Smart Start mode by calling ZW_AddNodeToNetwork with ADD_NODE_SMART_START, the
 * controller will when a Smart Start inclusion request (NHID) are received use ApplicationControllerUpdate (0x49) with
 * UPDATE_STATE_NODE_INFO_SMARTSTART_HOMEID_RECEIVED (0x85) to inform the Application that a Smart Start node with the
 * specified characteristics requests for Inclusion.
 *
 * Serial API:
 *      ZW->HOST: REQ | 0x49 | 0x85 | bNodeID | dskID[4] | bLen | basic | generic | specific | cmdclasses[ ]
 *
 * Note: By some reason ZW puts some extra byte after bNodeID, that is in fact not a part of a NHID.
 *       Thus, the NHID part starts from data[5], not from data[4].
 *       Unfortunately this point is undocumented.
 */
void NWIHomeIDCallback( int len, uint8_t* buf ) {
    if( znet_cb && znet_cb->node_nhid_received ) {
        znet_cb->node_nhid_received( &buf[5], znet_cb->arg );
    }
}

/**
 * If Controller has been put into Smart Start mode by calling ZW_AddNodeToNetwork with ADD_NODE_SMART_START, the
 * controller will when a Smart Start Included Node Information Frame (INIF) are received use
 * ApplicationControllerUpdate (0x49) with UPDATE_STATE_INCLUDED_NODE_INFO_RECEIVED (0x86) to inform the Application
 * that a Smart Start node with the specified characteristics has been powered up.
 *
 * Serial API:
 *      ZW->HOST: REQ | 0x49 | 0x86 | bNodeID | bLen | rxStatus | dskID[4]
 */
void IncludedNIFCallback( int len, uint8_t* buf ) {
    if( znet_cb && znet_cb->node_inif_received ) {
        znet_cb->node_inif_received( buf[3], &buf[6], buf[5] & RECEIVE_STATUS_FOREIGN_HOMEID, znet_cb->arg );
    }
}
