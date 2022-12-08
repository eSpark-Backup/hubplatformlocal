
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
#include "znet_main.h"
extern void ICACHE_FLASH_ATTR zwave_plus_info_get();

#define FC_STATUS_OFFSET 2

//////////////////////////////////////////////////////////////////////////////

// Send ZWave+ Info Get
//
// @param: this
// ZNet object instance
//
// @param: node_id
// Destination Node Id
//
// @param: channel_id
// Destination channel Id
//
// @param: instance_id
// Instance Id
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
znet_error_t ICACHE_FLASH_ATTR znet_cc_zwave_plus_info_get( uint8_t node_id,
                                                            uint8_t channel_id,
                                                            ZFunction_Callback Callback,
                                                            void* callbackArg,
                                                            encap_type_t encap_type ) {
    uint8_t data[] = { COMMAND_CLASS_ZWAVEPLUS_INFO, ZWAVEPLUS_INFO_GET };
    /// TODO: check channel support
    return znet_fc_send_data( node_id, channel_id, sizeof data, data, Callback, callbackArg, encap_type );
}

//----------------------------------------------------------------------------
// Z-Wave+ Info Request Command
void ICACHE_FLASH_ATTR znet_cc_zwave_plus_info_request( const ZFunction func,
                                                        uint8_t node_id,
                                                        int UNUSED( cc_data_len ),
                                                        const uint8_t* UNUSED( cc_data ) ) {
    // Check if broadcast received
    if( ( func->_znet->_frame._data[FC_STATUS_OFFSET] & 0x0c ) == 0x04 ) {
        return;
    }

    ZW_ZWAVEPLUS_INFO_REPORT_V2_FRAME data;

    // fill by default
    data.cmdClass = COMMAND_CLASS_ZWAVEPLUS_INFO_V2;
    data.cmd = ZWAVEPLUS_INFO_REPORT_V2;
    data.zWaveVersion = ZWAVEPLUS_INFO_VERSION_V2;
    data.roleType = ROLE_TYPE_CONTROLLER_CENTRAL_STATIC;
    data.nodeType = ZWAVEPLUS_INFO_REPORT_NODE_TYPE_ZWAVEPLUS_NODE_V2;
    data.installerIconType1 = INST_ICON_TYPE_1;
    data.installerIconType2 = INST_ICON_TYPE_2;
    data.userIconType1 = USER_ICON_TYPE_1;
    data.userIconType2 = USER_ICON_TYPE_2;

    znet_cc_zwave_plus_info_fill( func, &data );  //  modify by firmware

    znet_fc_send_data( node_id, func->_endpoint, sizeof data, (uint8_t*)&data, NULL, NULL, func->_security );
    zwave_plus_info_get();
}
