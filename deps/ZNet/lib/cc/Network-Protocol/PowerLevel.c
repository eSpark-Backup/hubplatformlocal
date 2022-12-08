
//////////////////////////////////////////////////////////////////////////////

#include "typedefs.h"

#include "byte_ops.h"

#include "znet.h"

#include "ZPlatform.h"
#include "znet_lib.h"
#include "znet_lib_fc.h"
#include "znet_lib_cc.h"
#include "znet_main.h"

#include <math.h>

//////////////////////////////////////////////////////////////////////////////

//----------------------------------------------------------------------------
// Send PowerLevel Get
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
znet_error_t ICACHE_FLASH_ATTR znet_cc_powerlevel_get( uint8_t node_id,
                                                       ZFunction_Callback Callback,
                                                       void* callbackArg,
                                                       encap_type_t encap_type ) {
    uint8_t data[] = { COMMAND_CLASS_POWERLEVEL, POWERLEVEL_GET };
    /// TODO: check channel support
    return znet_fc_send_data( node_id, 0, sizeof data, data, Callback, callbackArg, encap_type );
}

//----------------------------------------------------------------------------
// Send PowerLevel Set
//
// @param: this
// ZNet object instance
//
// @param: node_id
// Destination Node Id
//
// @param: level
// Power level to set (from 0 to 9)
// The 'Power level' can return the following values:
// Value  Description
// -----  -----------
// 0x00   NormalPower
// 0x01   minus1dBm
// 0x02   minus2dBm
// 0x03   minus3dBm
// 0x04   minus4dBm
// 0x05   minus5dBm
// 0x06   minus6dBm
// 0x07   minus7dBm
// 0x08   minus8dBm
// 0x09   minus9dBm
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
                                                       encap_type_t encap_type ) {
    uint8_t data[] = { COMMAND_CLASS_POWERLEVEL, POWERLEVEL_SET, level, timeout };
    /// TODO: check channel support
    return znet_fc_send_data( node_id, 0, sizeof data, data, Callback, callbackArg, encap_type );
}

//----------------------------------------------------------------------------
// Send PowerLevel Test Node Get
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
znet_error_t ICACHE_FLASH_ATTR znet_cc_powerlevel_test_node_get( uint8_t node_id,
                                                                 ZFunction_Callback Callback,
                                                                 void* callbackArg,
                                                                 encap_type_t encap_type ) {
    uint8_t data[] = { COMMAND_CLASS_POWERLEVEL, POWERLEVEL_TEST_NODE_GET };
    /// TODO: check channel support
    return znet_fc_send_data( node_id, 0, sizeof data, data, Callback, callbackArg, encap_type );
}

//----------------------------------------------------------------------------
// Send PowerLevel Test Node Set
//
// @param: this
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
                                                                 encap_type_t encap_type ) {
    uint8_t data[] = { COMMAND_CLASS_POWERLEVEL, POWERLEVEL_TEST_NODE_SET, testNodeId, level, 0xFF, 0xFF };
    int16_to_bytes( frameCount, &data[4] );
    /// TODO: check channel support
    return znet_fc_send_data( node_id, 0, sizeof data, data, Callback, callbackArg, encap_type );
}

//----------------------------------------------------------------------------
#include "logger.h"
#define TIMER_STATUS_EMPTY 0
#define TIMER_STATUS_TEST 1
#define TIMER_STATUS_SET 2

#define TEST_IN_PROGRESS

uint8_t power_level_timer = 0;
uint16_t test_frame_count = 0;
uint16_t test_frame_count_ok = 0;
uint16_t test_frame_count_nok = 0;
uint8_t test_node_id = 0;
uint8_t test_power_lewel = 0;
uint8_t controller_node_id = 0;
uint8_t timer_status = TIMER_STATUS_EMPTY;
uint8_t test_status;
uint8_t power_level_sec = 0;
uint8_t power_level_enpoint = 0;
uint8_t test_security = 0;
uint8_t power_level = POWERLEVEL_TEST_NODE_SET_NORMALPOWER;

//----------------------------------------------------------------------------
void send_power_level_report() {
    ZW_POWERLEVEL_REPORT_FRAME data;
    data.cmdClass = COMMAND_CLASS_POWERLEVEL;
    data.cmd = POWERLEVEL_REPORT;
    data.powerLevel = power_level;
    data.timeout = round( znet_timer_get_remaining( &power_level_timer ) / 1000.0 );
    znet_fc_send_data(
        controller_node_id, power_level_enpoint, sizeof data, (uint8_t*)&data, NULL, NULL, power_level_sec );
}

//----------------------------------------------------------------------------
znet_error_t send_test_frame();

//----------------------------------------------------------------------------
void ICACHE_FLASH_ATTR send_test_frame_Callback( ZFunction func,
                                                 void* UNUSED( arg ),
                                                 ZFuncFailures_e reason,
                                                 const void* dataPtr,
                                                 size_t dataSize ) {
    if( FUNC_OK == reason ) {
        test_frame_count_ok++;
    }
    else {
        test_frame_count_nok++;
    }
    send_test_frame();
}

//----------------------------------------------------------------------------
znet_error_t ICACHE_FLASH_ATTR send_test_frame() {
    Log_Write( "send_test_frame test_frame_count %d\r\n", test_frame_count );
    znet_error_t result = ZNET_ERR_OK;
    if( test_frame_count != 0 ) {
        test_frame_count--;
        result = znet_fc_send_test_frame( test_node_id, test_power_lewel, send_test_frame_Callback, NULL );
    }
    else {
        ZW_POWERLEVEL_TEST_NODE_REPORT_FRAME data;
        data.cmdClass = COMMAND_CLASS_POWERLEVEL;
        data.cmd = POWERLEVEL_TEST_NODE_REPORT;
        data.testNodeid = test_node_id;
        data.statusOfOperation = test_frame_count_ok ? POWERLEVEL_TEST_NODE_REPORT_ZW_TEST_SUCCES :
                                                       POWERLEVEL_TEST_NODE_REPORT_ZW_TEST_FAILED;

        test_status = test_frame_count_ok ? POWERLEVEL_TEST_NODE_REPORT_ZW_TEST_SUCCES :
                                            POWERLEVEL_TEST_NODE_REPORT_ZW_TEST_FAILED;
        data.testFrameCount1 = test_frame_count_ok >> 8;
        data.testFrameCount2 = test_frame_count_ok;
        Log_Write( "znet_fc_send_data test_security %d\r\n", test_security );
        /// TODO: check channel support
        result = znet_fc_send_data( controller_node_id, 0, sizeof data, (uint8_t*)&data, NULL, NULL, test_security );
    }
    return result;
}

//----------------------------------------------------------------------------
static void ICACHE_FLASH_ATTR power_level_timer_Callback( void* UNUSED( arg ) ) {
    if( timer_status == TIMER_STATUS_SET ) {
        power_level = POWERLEVEL_TEST_NODE_SET_NORMALPOWER;
        if( Z_WAVE_LIBRARY_TYPE != ZW_LIB_CONTROLLER_BRIDGE ) {
            znet_fc_power_level_set( POWERLEVEL_TEST_NODE_SET_NORMALPOWER, NULL, NULL );
        }
        timer_status = TIMER_STATUS_EMPTY;
        znet_timer_deinit( &power_level_timer );
    }
}

//----------------------------------------------------------------------------
void ICACHE_FLASH_ATTR
PowerLevelGetOnSuccess( ZFunction func, void* arg, ZFuncFailures_e reason, const void* dataPtr, size_t dataSize ) {
    if( FUNC_OK == reason ) {
        power_level = ( (uint8_t*)dataPtr )[0];
        send_power_level_report();
    }
}

//----------------------------------------------------------------------------
// Powerlevel Receive
//
void ICACHE_FLASH_ATTR znet_cc_powerlevel_receive( const ZFunction func,
                                                   uint8_t node_id,
                                                   int UNUSED( cc_data_len ),
                                                   const uint8_t* cc_data ) {
    controller_node_id = node_id;
    power_level_sec = func->_security;
    power_level_enpoint = func->_endpoint;

    if( cc_data[1] == POWERLEVEL_TEST_NODE_SET ) {
        test_frame_count = cc_data[5] + cc_data[4] * 0x100;
        if( ( cc_data[3] > POWERLEVEL_TEST_NODE_SET_NORMALPOWER ) &&
            ( cc_data[3] <= POWERLEVEL_TEST_NODE_SET_MINUS9DBM ) && ( test_frame_count != 0 ) ) {
            supervision_status = SUPERVISION_REPORT_SUCCESS;
            test_node_id = cc_data[2];
            test_power_lewel = cc_data[3];
            test_frame_count_ok = 0;
            test_frame_count_nok = 0;
            test_security = func->_security;
            if( test_frame_count > 0 )
                --test_frame_count;
            test_status = POWERLEVEL_TEST_NODE_REPORT_ZW_TEST_INPROGRESS;
            znet_fc_send_test_frame( test_node_id, test_power_lewel, send_test_frame_Callback, NULL );
        }
        else {
            supervision_status = SUPERVISION_REPORT_FAIL;
        }
    }

    if( cc_data[1] == POWERLEVEL_TEST_NODE_GET ) {
        ZW_POWERLEVEL_TEST_NODE_REPORT_FRAME data;
        data.cmdClass = COMMAND_CLASS_POWERLEVEL;
        data.cmd = POWERLEVEL_TEST_NODE_REPORT;
        data.testNodeid = test_node_id;
        data.statusOfOperation = test_status;
        data.testFrameCount1 = test_frame_count_ok >> 8;
        data.testFrameCount2 = test_frame_count_ok & 0xFF;
        znet_fc_send_data( node_id, func->_endpoint, sizeof data, (uint8_t*)&data, NULL, NULL, func->_security );
    }

    if( cc_data[1] == POWERLEVEL_GET ) {
        if( Z_WAVE_LIBRARY_TYPE == ZW_LIB_CONTROLLER_BRIDGE ) {
            send_power_level_report();
        }
        else {
            znet_fc_power_level_get( PowerLevelGetOnSuccess, NULL );
        }
    }

    if( cc_data[1] == POWERLEVEL_SET ) {
        if( ( cc_data[2] > POWERLEVEL_TEST_NODE_SET_NORMALPOWER ) &&
            ( cc_data[2] <= POWERLEVEL_TEST_NODE_SET_MINUS9DBM ) ) {
            supervision_status = SUPERVISION_REPORT_SUCCESS;
            timer_status = TIMER_STATUS_SET;
            power_level = cc_data[2];
            if( Z_WAVE_LIBRARY_TYPE != ZW_LIB_CONTROLLER_BRIDGE ) {
                znet_fc_power_level_set( cc_data[2], NULL, NULL );
            }
            znet_timer_init( &power_level_timer, power_level_timer_Callback, func->_znet );
            znet_timer_set( &power_level_timer, cc_data[3] * 1000 );
        }
        else {
            supervision_status = SUPERVISION_REPORT_FAIL;
        }
    }
}
