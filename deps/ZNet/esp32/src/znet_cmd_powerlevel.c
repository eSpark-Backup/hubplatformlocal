/**
 * @file znet_cmd_powerlevel.c
 * @date 15 May 2018
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

/// INFO: internal
#include "heap.h"
#include <znet_lib.h>
#include <znet_lib_cc_codes.h>
#include <znet_lib_cc_network.h>

// node cmd powerlevel report //////////////////////////////////////////
/// INFO: Powerlevel Command Class v1

void znet_cc_powerlevel_report( const ZFunction func, uint8_t node_id, int cc_data_len, const uint8_t* cc_data ) {
    assert( cc_data );
    assert( cc_data_len >= 3 );
    assert( cc_data[0] == ZNET_COMMAND_CLASS_POWERLEVEL );
    assert( cc_data[1] == POWERLEVEL_REPORT );

    if( node_id < ZNET_NODE_ID_MIN || node_id > ZNET_NODE_ID_MAX )
        return;

    if( cc_data[2] > ZNET_POWERLEVEL_MINUS_9dBm )
        return;

    if( cc_data[2] != ZNET_POWERLEVEL_NORMAL ) {
        assert( cc_data_len >= 4 );
        if( cc_data[3] < ZNET_POWERLEVEL_TIMEOUT_MIN /* ||
            cc_data[3] > ZNET_POWERLEVEL_TIMEOUT_MAX */ )
            return;
    }

    /// TODO: check in storage node_id

    znet_powerlevel_report_t report = {};
    report.power_level = cc_data[2];
    if( cc_data[2] != ZNET_POWERLEVEL_NORMAL ) {
        report.timeout = cc_data[3];
    }

    if( znet_cb->node_cmd_powerlevel_result )
        znet_cb->node_cmd_powerlevel_result( 0, node_id, &report, znet_cb->arg );
}

// node cmd powerlevel get /////////////////////////////////////////////
/// INFO: Powerlevel Command Class v1

static void _znet_node_cmd_powerlevel_get_cb( ZFunction func,
                                              void* arg,
                                              ZFuncFailures_e reason,
                                              const void* dataPtr,
                                              size_t dataSize ) {
    uint8_t nodeId;
    ZNet_CheckCallback( func, reason, &nodeId, NULL );

    if( znet_cb->node_cmd_powerlevel_result )
        znet_cb->node_cmd_powerlevel_result( znet_get_error( reason ), nodeId, NULL, znet_cb->arg );
}

znet_error_t znet_node_cmd_powerlevel_get( znet_node_id_t node_id ) {
    if( !znet_cb ) {
        ZNET_LOGE( "Library not initialized!\n" );
        return ZNET_ERR_FAULT;
    }

    if( node_id < ZNET_NODE_ID_MIN || node_id > ZNET_NODE_ID_MAX ) {
        return ZNET_ERR_INVAL;
    }

    /// TODO: check wait report for node_id

    encap_type_t encap = znet_utils_get_security_encap_type_by_node_id( node_id );

    return znet_cc_powerlevel_get( node_id, _znet_node_cmd_powerlevel_get_cb, NULL, encap );
}

// node cmd powerlevel set /////////////////////////////////////////////
/// INFO: Powerlevel Command Class v1

znet_error_t znet_node_cmd_powerlevel_set( znet_node_id_t node_id,
                                           znet_powerlevel_value_t power_level,
                                           znet_powerlevel_timeout_t timeout ) {
    if( !znet_cb ) {
        ZNET_LOGE( "Library not initialized!\n" );
        return ZNET_ERR_FAULT;
    }

    if( node_id < ZNET_NODE_ID_MIN || node_id > ZNET_NODE_ID_MAX ) {
        return ZNET_ERR_INVAL;
    }

    if( power_level > ZNET_POWERLEVEL_MINUS_9dBm ) {
        return ZNET_ERR_INVAL;
    }
    if( timeout < ZNET_POWERLEVEL_TIMEOUT_MIN ) {
        return ZNET_ERR_INVAL;
    }

    /// TODO: check in storage node_id

    encap_type_t encap = znet_utils_get_security_encap_type_by_node_id( node_id );

    return znet_cc_powerlevel_set( node_id, power_level, timeout, NULL, NULL, encap );
}

// node cmd powerlevel test node report ////////////////////////////////
/// INFO: Powerlevel Command Class v1

void znet_cc_powerlevel_test_node_report( const ZFunction func,
                                          uint8_t node_id,
                                          int cc_data_len,
                                          const uint8_t* cc_data ) {
    assert( cc_data );
    assert( cc_data_len >= 3 );
    assert( cc_data[0] == ZNET_COMMAND_CLASS_POWERLEVEL );
    assert( cc_data[1] == POWERLEVEL_TEST_NODE_REPORT );

    if( node_id < ZNET_NODE_ID_MIN || node_id > ZNET_NODE_ID_MAX )
        return;

    if( cc_data[2] != 0 ) {
        assert( cc_data_len >= 6 );
    }

    /// TODO: check in storage node_id

    znet_powerlevel_test_node_report_t report = {};
    report.test_id = cc_data[2];
    if( cc_data[2] != 0 ) {
        report.status = cc_data[3];
        report.test_count_ack = cc_data[5];
        report.test_count_ack |= (uint16_t)cc_data[4] << 8;
    }

    if( znet_cb->node_cmd_powerlevel_test_node_result )
        znet_cb->node_cmd_powerlevel_test_node_result( 0, node_id, &report, znet_cb->arg );
}

// node cmd powerlevel test node get ///////////////////////////////////
/// INFO: Powerlevel Command Class v1

static void _znet_node_cmd_powerlevel_test_node_get_cb( ZFunction func,
                                                        void* arg,
                                                        ZFuncFailures_e reason,
                                                        const void* dataPtr,
                                                        size_t dataSize ) {
    uint8_t nodeId;
    ZNet_CheckCallback( func, reason, &nodeId, NULL );

    if( znet_cb->node_cmd_powerlevel_test_node_result )
        znet_cb->node_cmd_powerlevel_test_node_result( znet_get_error( reason ), nodeId, NULL, znet_cb->arg );
}

znet_error_t znet_node_cmd_powerlevel_test_node_get( znet_node_id_t node_id ) {
    if( !znet_cb ) {
        ZNET_LOGE( "Library not initialized!\n" );
        return ZNET_ERR_FAULT;
    }

    if( node_id < ZNET_NODE_ID_MIN || node_id > ZNET_NODE_ID_MAX ) {
        return ZNET_ERR_INVAL;
    }

    /// TODO: check wait report for node_id

    encap_type_t encap = znet_utils_get_security_encap_type_by_node_id( node_id );

    return znet_cc_powerlevel_test_node_get( node_id, _znet_node_cmd_powerlevel_test_node_get_cb, NULL, encap );
}

// node cmd powerlevel test node set ///////////////////////////////////
/// INFO: Powerlevel Command Class v1

znet_error_t znet_node_cmd_powerlevel_test_node_set( znet_node_id_t node_id,
                                                     znet_node_id_t test_id,
                                                     znet_powerlevel_value_t power_level,
                                                     uint16_t test_count ) {
    if( !znet_cb ) {
        ZNET_LOGE( "Library not initialized!\n" );
        return ZNET_ERR_FAULT;
    }

    if( node_id < ZNET_NODE_ID_MIN || node_id > ZNET_NODE_ID_MAX ) {
        return ZNET_ERR_INVAL;
    }

    if( test_id < ZNET_NODE_ID_MIN || test_id > ZNET_NODE_ID_MAX ) {
        return ZNET_ERR_INVAL;
    }

    if( power_level > ZNET_POWERLEVEL_MINUS_9dBm ) {
        return ZNET_ERR_INVAL;
    }

    if( test_count == 0 ) {
        return ZNET_ERR_INVAL;
    }

    /// TODO: check in storage node_id

    encap_type_t encap = znet_utils_get_security_encap_type_by_node_id( node_id );

    return znet_cc_powerlevel_test_node_set( node_id, test_id, power_level, test_count, NULL, NULL, encap );
}
