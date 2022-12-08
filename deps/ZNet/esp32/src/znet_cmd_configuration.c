/**
 * @file znet_cmd_binary_switch.c
 * @date 2 May 2018
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
#include <znet_lib_cc_application.h>

union ConfigurationInfo {
    void* data;
    uint8_t parameter_number;
};

uint32_t concatenateParamProp( const uint8_t* data_start, uint8_t length ) {
    uint32_t result = 0;
    for( int i = 0; i < length; ++i ) {
        result = result | data_start[i] << ( 8 * ( length - i - 1 ) );
    }

    return result;
}

// node cmd configuration report ///////////////////////////////////////
/// INFO:  Configuration_Report Command Class v1
void znet_cc_configuration_report( const ZFunction func, uint8_t node_id, int cc_data_len, const uint8_t* cc_data ) {
    assert( cc_data );
    assert( cc_data_len >= ZNET_CMD_CONFIGURATION_REPORT_CHECK_LEN );
    assert( cc_data[0] == ZNET_COMMAND_CLASS_CONFIGURATION );
    assert( cc_data[1] == CONFIGURATION_REPORT );
    //    assert( cc_data[2] );

    if( !cc_data[2] ) {
        if( znet_cb->node_cmd_configuration_result ) {
            ZNET_LOGE( "Invalid parameter number! \n" );
            znet_cb->node_cmd_configuration_result( ZNET_ERR_INVAL, node_id, func->_endpoint, 0, NULL, znet_cb->arg );
        }
    }

    if( node_id < ZNET_NODE_ID_MIN || node_id > ZNET_NODE_ID_MAX )
        return;

    uint8_t param_size = cc_data[3] & CONFIGURATION_SET_LEVEL_SIZE_MASK;
    if( param_size > ZNET_CMD_CONFIGURATION_PARAM_NUM_MAX || param_size == 0 ||
        param_size == ZNET_CMD_CONFIGURATION_PARAM_NUM_INVALID ) {
        ZNET_LOGE( "Invalid size ID!\n" );
        return;
    }

    znet_configuration_report_t report = {};
    report.param_number = cc_data[2];
    report.data_count = param_size;
    report.value = cc_data[4];
    if( report.data_count > 1 ) {
        report.value = ( report.value << 8 ) | ( (uint32_t)cc_data[5] );
    }
    if( report.data_count > 2 ) {
        report.value = ( report.value << 8 ) | ( (uint32_t)cc_data[6] );
        report.value = ( report.value << 8 ) | ( (uint32_t)cc_data[7] );
    }

    if( znet_cb->node_cmd_configuration_result )
        znet_cb->node_cmd_configuration_result(
            0, node_id, func->_endpoint, report.param_number, &report, znet_cb->arg );
}

/// INFO: Configuration_Get Command Class v1

static void _znet_node_cmd_configuration_get_cb( ZFunction func,
                                                 void* arg,
                                                 ZFuncFailures_e reason,
                                                 const void* dataPtr,
                                                 size_t dataSize ) {
    uint8_t nodeId, endpoint;
    ZNet_CheckCallback( func, reason, &nodeId, &endpoint );

    union ConfigurationInfo info;
    info.data = arg;

    if( znet_cb->node_cmd_configuration_result )
        znet_cb->node_cmd_configuration_result(
            znet_get_error( reason ), nodeId, endpoint, info.parameter_number, NULL, znet_cb->arg );
}

znet_error_t znet_node_cmd_configuration_get( znet_node_id_t node_id,
                                              znet_node_channel_id_t channel_id,
                                              uint8_t config_param_num ) {
    if( !znet_cb ) {
        ZNET_LOGE( "Library not initialized!\n" );
        return ZNET_ERR_FAULT;
    }

    if( node_id < ZNET_NODE_ID_MIN || node_id > ZNET_NODE_ID_MAX ) {
        return ZNET_ERR_INVAL;
    }

    /// TODO: check in storage node_id
    /// TODO: check wait report for node_id

    union ConfigurationInfo info;
    info.parameter_number = config_param_num;

    encap_type_t encap = znet_utils_get_security_encap_type_by_node_id( node_id );

    return znet_cc_configuration_get(
        node_id, channel_id, config_param_num, _znet_node_cmd_configuration_get_cb, info.data, encap );
}

/// INFO: Configuration_Set Command Class v1

static void _znet_node_cmd_configuration_set_cb( ZFunction func,
                                                 void* arg,
                                                 ZFuncFailures_e reason,
                                                 const void* dataPtr,
                                                 size_t dataSize ) {
    uint8_t nodeId, endpoint;
    ZNet_CheckCallback( func, reason, &nodeId, &endpoint );

    union ConfigurationInfo info;
    info.data = arg;

    if( znet_cb->node_cmd_configuration_request_status ) {
        znet_cb->node_cmd_configuration_request_status(
            znet_get_error( reason ), nodeId, endpoint, info.parameter_number, znet_cb->arg );
    }
}

znet_error_t znet_node_cmd_configuration_set( znet_node_id_t node_id,
                                              znet_node_channel_id_t channel_id,
                                              uint8_t config_param_num,
                                              uint8_t config_size,
                                              int set_to_default,
                                              znet_cmd_configuration_value_t config_value ) {
    if( !znet_cb ) {
        ZNET_LOGE( "Library not initialized!\n" );
        return ZNET_ERR_FAULT;
    }

    if( node_id < ZNET_NODE_ID_MIN || node_id > ZNET_NODE_ID_MAX ) {
        return ZNET_ERR_INVAL;
    }

    uint8_t temp_val = config_size & CONFIGURATION_SET_LEVEL_SIZE_MASK;
    if( temp_val > ZNET_CMD_CONFIGURATION_PARAM_NUM_MAX || temp_val == 0 ||
        temp_val == ZNET_CMD_CONFIGURATION_PARAM_NUM_INVALID ) {
        return ZNET_ERR_INVAL;
    }

    union ConfigurationInfo info;
    info.parameter_number = config_param_num;

    encap_type_t encap = znet_utils_get_security_encap_type_by_node_id( node_id );

    return znet_cc_configuration_set( node_id,
                                      channel_id,
                                      config_param_num,
                                      ( set_to_default ? TRUE : FALSE ),
                                      config_value,
                                      config_size,
                                      _znet_node_cmd_configuration_set_cb,
                                      info.data,
                                      encap );
}

/// INFO: Configuration Command Class v2
void znet_cc_configuration_bulk_report( const ZFunction func,
                                        uint8_t node_id,
                                        int cc_data_len,
                                        const uint8_t* cc_data ) {
    assert( cc_data );
    assert( cc_data_len >= ZNET_CMD_CONFIGURATION_BULK_REPORT_CHECK_LEN );
    assert( cc_data[0] == ZNET_COMMAND_CLASS_CONFIGURATION );
    assert( cc_data[1] == CONFIGURATION_BULK_REPORT_V4 );
    assert( cc_data[4] );

    // TBD: consider usage of cc_data[5] -> Reports to follow
    uint16_t param_offset = ( (uint16_t)cc_data[2] << 8 ) | cc_data[3];
    if( param_offset == 0 ) {
        ZNET_LOGE( "Invalid offset parameter!\n" );
        return;
    }

    uint8_t param_size = cc_data[6] & CONFIGURATION_BULK_REPORT_PROPERTIES1_SIZE_MASK_V2;
    if( param_size > ZNET_CMD_CONFIGURATION_PARAM_NUM_MAX || param_size == 0 ||
        param_size == ZNET_CMD_CONFIGURATION_PARAM_NUM_INVALID ) {
        ZNET_LOGE( "Invalid size ID!\n" );
        return;
    }

    if( node_id < ZNET_NODE_ID_MIN || node_id > ZNET_NODE_ID_MAX )
        return;

    /* Configuration Value is (M*N bytes) where N = param_size
       and number of parameters M = cc_data[4] */
    size_t data_count = param_size * cc_data[4];
    assert( cc_data_len >= ZNET_CMD_CONFIGURATION_BULK_REPORT_CHECK_LEN + ( param_size * cc_data[4] ) );

    const size_t buff_size = sizeof( znet_configuration_bulk_report_t ) + ( cc_data[4] * sizeof( uint32_t ) );
    uint8_t buff[buff_size];
    memset( buff, 0, buff_size );

    znet_configuration_bulk_report_t* bulk_report = (znet_configuration_bulk_report_t*)&buff;
    bulk_report->param_offset = param_offset;
    bulk_report->params_count = cc_data[4];
    bulk_report->param_size = param_size;
    bulk_report->rep_to_follows = cc_data[5];

    int counter = ZNET_CMD_CONFIGURATION_BULK_REPORT_CHECK_LEN;
    for( int i = 0; i < bulk_report->params_count; ++i ) {
        assert( counter < cc_data_len );
        bulk_report->values[i] = concatenateParamProp( cc_data + counter, param_size );
        counter += param_size;
    }

    if( znet_cb->node_cmd_configuration_bulk_result )
        znet_cb->node_cmd_configuration_bulk_result( 0,
                                                     node_id,
                                                     func->_endpoint,
                                                     bulk_report->param_offset,
                                                     bulk_report->params_count,
                                                     bulk_report,
                                                     znet_cb->arg );
}

znet_error_t znet_node_cmd_configuration_bulk_set( znet_node_id_t node_id,
                                                   znet_node_channel_id_t channel_id,
                                                   znet_cmd_configuration_id_t config_id,
                                                   uint8_t config_count,
                                                   uint8_t config_size,
                                                   int need_report,
                                                   int set_to_default,
                                                   const uint8_t* config_value ) {
    if( !znet_cb ) {
        ZNET_LOGE( "Library not initialized!\n" );
        return ZNET_ERR_FAULT;
    }

    if( node_id < ZNET_NODE_ID_MIN || node_id > ZNET_NODE_ID_MAX ) {
        return ZNET_ERR_INVAL;
    }

    uint8_t temp_val = config_size & CONFIGURATION_SET_LEVEL_SIZE_MASK;
    if( temp_val > ZNET_CMD_CONFIGURATION_PARAM_NUM_MAX || temp_val == 0 ||
        temp_val == ZNET_CMD_CONFIGURATION_PARAM_NUM_INVALID ) {
        return ZNET_ERR_INVAL;
    }

    encap_type_t encap = znet_utils_get_security_encap_type_by_node_id( node_id );

    return znet_cc_configuration_bulk_set( node_id,
                                           channel_id,
                                           config_id,
                                           config_count,
                                           ( set_to_default ? TRUE : FALSE ),
                                           ( need_report ? TRUE : FALSE ),
                                           temp_val,
                                           config_value,
                                           NULL,
                                           NULL,
                                           encap );
}

znet_error_t znet_node_cmd_configuration_bulk_get( znet_node_id_t node_id,
                                                   znet_node_channel_id_t channel_id,
                                                   znet_cmd_configuration_id_t config_id,
                                                   uint8_t config_count ) {
    if( !znet_cb ) {
        ZNET_LOGE( "Library not initialized!\n" );
        return ZNET_ERR_FAULT;
    }

    if( node_id < ZNET_NODE_ID_MIN || node_id > ZNET_NODE_ID_MAX ) {
        return ZNET_ERR_INVAL;
    }

    encap_type_t encap = znet_utils_get_security_encap_type_by_node_id( node_id );

    return znet_cc_configuration_bulk_get( node_id, channel_id, config_id, config_count, NULL, NULL, encap );
}

/// INFO: Configuration Command Class v3
void znet_cc_configuration_name_report( const ZFunction func,
                                        uint8_t node_id,
                                        int cc_data_len,
                                        const uint8_t* cc_data ) {
    assert( cc_data );
    assert( cc_data_len >= ZNET_CMD_CONFIGURATION_NIP_REPORT_CHECK_LEN );
    assert( cc_data[0] == ZNET_COMMAND_CLASS_CONFIGURATION );
    assert( cc_data[1] == CONFIGURATION_NAME_REPORT_V4 );

    if( node_id < ZNET_NODE_ID_MIN || node_id > ZNET_NODE_ID_MAX )
        return;

    int name_length = cc_data_len - ZNET_CMD_CONFIGURATION_NIP_REPORT_CHECK_LEN;

    const size_t buff_size = sizeof( znet_configuration_name_report_t ) + name_length;
    uint8_t buff[buff_size];
    memset( buff, 0, buff_size );

    znet_configuration_name_report_t* name_report = (znet_configuration_name_report_t*)&buff;
    name_report->param_number = ( (uint16_t)cc_data[2] << 8 ) | cc_data[3];
    name_report->name_length = name_length;
    name_report->rep_to_follows = cc_data[4];
    for( int i = 0; i < name_length; i++ )
        name_report->data[i] = cc_data[ZNET_CMD_CONFIGURATION_NIP_REPORT_CHECK_LEN + i];

    if( znet_cb->node_cmd_configuration_name_result )
        znet_cb->node_cmd_configuration_name_result(
            0, node_id, func->_endpoint, name_report->param_number, name_report, znet_cb->arg );
}

static void _znet_node_cmd_configuration_name_get_cb( ZFunction func,
                                                      void* arg,
                                                      ZFuncFailures_e reason,
                                                      const void* dataPtr,
                                                      size_t dataSize ) {
    uint8_t nodeId, endpoint;
    ZNet_CheckCallback( func, reason, &nodeId, &endpoint );

    union ConfigurationInfo info;
    info.data = arg;

    if( znet_cb->node_cmd_configuration_name_result )
        znet_cb->node_cmd_configuration_name_result(
            znet_get_error( reason ), nodeId, endpoint, info.parameter_number, NULL, znet_cb->arg );
}

znet_error_t znet_node_cmd_configuration_name_get( znet_node_id_t node_id,
                                                   znet_node_channel_id_t channel_id,
                                                   znet_cmd_configuration_id_t param_number,
                                                   bool do_request ) {
    if( !do_request ) {
        ZNET_LOGI( "Next packages have to be sent automatically\n" );
        return ZNET_ERR_OK;
    }

    if( !znet_cb ) {
        ZNET_LOGE( "Library not initialized!\n" );
        return ZNET_ERR_FAULT;
    }

    if( node_id < ZNET_NODE_ID_MIN || node_id > ZNET_NODE_ID_MAX ) {
        return ZNET_ERR_INVAL;
    }

    union ConfigurationInfo info;
    info.parameter_number = param_number;

    encap_type_t encap = znet_utils_get_security_encap_type_by_node_id( node_id );

    return znet_cc_configuration_name_get(
        node_id, channel_id, param_number, _znet_node_cmd_configuration_name_get_cb, info.data, encap );
}

void znet_cc_configuration_info_report( const ZFunction func,
                                        uint8_t node_id,
                                        int cc_data_len,
                                        const uint8_t* cc_data ) {
    assert( cc_data );
    assert( cc_data_len >= ZNET_CMD_CONFIGURATION_NIP_REPORT_CHECK_LEN );
    assert( cc_data[0] == ZNET_COMMAND_CLASS_CONFIGURATION );
    assert( cc_data[1] == CONFIGURATION_INFO_REPORT_V4 );

    if( node_id < ZNET_NODE_ID_MIN || node_id > ZNET_NODE_ID_MAX )
        return;

    int info_length = cc_data_len - ZNET_CMD_CONFIGURATION_NIP_REPORT_CHECK_LEN;

    const size_t buff_size = sizeof( znet_configuration_info_report_t ) + info_length;
    uint8_t buff[buff_size];
    memset( buff, 0, buff_size );

    znet_configuration_info_report_t* info_report = (znet_configuration_info_report_t*)&buff;
    info_report->param_number = ( (uint16_t)cc_data[2] << 8 ) | cc_data[3];
    info_report->rep_to_follows = cc_data[4];
    info_report->info_length = info_length;
    for( int i = 0; i < info_length; i++ )
        info_report->data[i] = cc_data[ZNET_CMD_CONFIGURATION_NIP_REPORT_CHECK_LEN + i];

    if( znet_cb->node_cmd_configuration_info_result )
        znet_cb->node_cmd_configuration_info_result(
            0, node_id, func->_endpoint, info_report->param_number, info_report, znet_cb->arg );
}

static void _znet_node_cmd_configuration_info_get_cb( ZFunction func,
                                                      void* arg,
                                                      ZFuncFailures_e reason,
                                                      const void* dataPtr,
                                                      size_t dataSize ) {
    uint8_t nodeId, endpoint;
    ZNet_CheckCallback( func, reason, &nodeId, &endpoint );

    union ConfigurationInfo info;
    info.data = arg;

    if( znet_cb->node_cmd_configuration_info_result )
        znet_cb->node_cmd_configuration_info_result(
            znet_get_error( reason ), nodeId, endpoint, info.parameter_number, NULL, znet_cb->arg );
}

znet_error_t znet_node_cmd_configuration_info_get( znet_node_id_t node_id,
                                                   znet_node_channel_id_t channel_id,
                                                   znet_cmd_configuration_id_t param_number,
                                                   bool do_request ) {
    if( !do_request ) {
        ZNET_LOGI( "Next packages have to be sent automatically\n" );
        return ZNET_ERR_OK;
    }

    if( !znet_cb ) {
        ZNET_LOGE( "Library not initialized!\n" );
        return ZNET_ERR_FAULT;
    }

    if( node_id < ZNET_NODE_ID_MIN || node_id > ZNET_NODE_ID_MAX ) {
        return ZNET_ERR_INVAL;
    }

    union ConfigurationInfo info;
    info.parameter_number = param_number;

    encap_type_t encap = znet_utils_get_security_encap_type_by_node_id( node_id );

    return znet_cc_configuration_info_get(
        node_id, channel_id, param_number, _znet_node_cmd_configuration_info_get_cb, info.data, encap );
}

void znet_cc_configuration_properties_report( const ZFunction func,
                                              uint8_t node_id,
                                              int cc_data_len,
                                              const uint8_t* cc_data ) {
    assert( cc_data );
    assert( cc_data_len >= ZNET_CMD_CONFIGURATION_PROP_REPORT_CHECK_LEN );
    assert( cc_data[0] == ZNET_COMMAND_CLASS_CONFIGURATION );
    assert( cc_data[1] == CONFIGURATION_PROPERTIES_REPORT_V4 );
    assert( ( cc_data[4] & 0x07 ) != 0x03 );

    if( node_id < ZNET_NODE_ID_MIN || node_id > ZNET_NODE_ID_MAX )
        return;

    znet_configuration_properties_report_t report;
    memset( &report, 0, sizeof( report ) );

    report.param_number = ( (uint16_t)cc_data[2] << 8 ) | cc_data[3];
    report.param_format = ( cc_data[4] & CONFIGURATION_PROPERTIES_REPORT_PROPERTIES1_FORMAT_MASK_V4 ) >>
                          CONFIGURATION_PROPERTIES_REPORT_PROPERTIES1_FORMAT_SHIFT_V4;
    report.param_size = cc_data[4] & CONFIGURATION_PROPERTIES_REPORT_PROPERTIES1_SIZE_MASK_V4;
    report.read_only = cc_data[4] & CONFIGURATION_PROPERTIES_REPORT_READ_ONLY_MASK;

    assert( cc_data_len >= ZNET_CMD_CONFIGURATION_PROP_REPORT_CHECK_LEN + report.param_size * 3 );

    int counter = ZNET_CMD_CONFIGURATION_PROP_REPORT_COMMON_PART + 1;

    report.param_min_value = concatenateParamProp( cc_data + counter, report.param_size );
    counter += report.param_size;

    report.param_max_value = concatenateParamProp( cc_data + counter, report.param_size );
    counter += report.param_size;

    report.param_default_value = concatenateParamProp( cc_data + counter, report.param_size );
    counter += report.param_size;

    report.next_param_number = ( (uint16_t)cc_data[counter] << 8 ) | cc_data[counter + 1];

    if( znet_cb->node_cmd_configuration_properties_result )
        znet_cb->node_cmd_configuration_properties_result(
            0, node_id, func->_endpoint, report.param_number, &report, znet_cb->arg );
}

static void _znet_node_cmd_configuration_properties_get_cb( ZFunction func,
                                                            void* arg,
                                                            ZFuncFailures_e reason,
                                                            const void* dataPtr,
                                                            size_t dataSize ) {
    uint8_t nodeId, endpoint;
    ZNet_CheckCallback( func, reason, &nodeId, &endpoint );

    union ConfigurationInfo info;
    info.data = arg;

    if( znet_cb->node_cmd_configuration_properties_result )
        znet_cb->node_cmd_configuration_properties_result(
            znet_get_error( reason ), nodeId, endpoint, info.parameter_number, NULL, znet_cb->arg );
}

znet_error_t znet_node_cmd_configuration_properties_get( znet_node_id_t node_id,
                                                         znet_node_channel_id_t channel_id,
                                                         znet_cmd_configuration_id_t param_number ) {
    if( !znet_cb ) {
        ZNET_LOGE( "Library not initialized!\n" );
        return ZNET_ERR_FAULT;
    }

    if( node_id < ZNET_NODE_ID_MIN || node_id > ZNET_NODE_ID_MAX ) {
        return ZNET_ERR_INVAL;
    }

    union ConfigurationInfo info;
    info.parameter_number = param_number;

    encap_type_t encap = znet_utils_get_security_encap_type_by_node_id( node_id );

    return znet_cc_configuration_properties_get(
        node_id, channel_id, param_number, _znet_node_cmd_configuration_properties_get_cb, info.data, encap );
}

znet_error_t znet_node_cmd_configuration_default_reset( znet_node_id_t node_id, znet_node_channel_id_t channel_id ) {
    if( !znet_cb ) {
        ZNET_LOGE( "Library not initialized!\n" );
        return ZNET_ERR_FAULT;
    }

    if( node_id < ZNET_NODE_ID_MIN || node_id > ZNET_NODE_ID_MAX ) {
        return ZNET_ERR_INVAL;
    }

    encap_type_t encap = znet_utils_get_security_encap_type_by_node_id( node_id );

    return znet_cc_configuration_default_reset( node_id, channel_id, NULL, NULL, encap );
}
