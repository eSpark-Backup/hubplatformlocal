//////////////////////////////////////////////////////////////////////////////

#include "typedefs.h"

//#include "ZErrors.h"
#include "znet.h"

#include "ZPlatform.h"
//#include "znet_fc_private.h"
#include "znet_lib.h"
#include "znet_lib_fc.h"
#include "znet_lib_cc.h"
#include "znet_main.h"

#include <assert.h>

//////////////////////////////////////////////////////////////////////////////

//----------------------------------------------------------------------------
znet_error_t ICACHE_FLASH_ATTR znet_cc_manufacturer_specific_get( uint8_t node_id,
                                                                  uint8_t channel_id,
                                                                  ZFunction_Callback Callback,
                                                                  void* callbackArg,
                                                                  encap_type_t encap ) {
    uint8_t data[] = { COMMAND_CLASS_MANUFACTURER_SPECIFIC, MANUFACTURER_SPECIFIC_GET };
    /// TODO: check channel support
    return znet_fc_send_data( node_id, channel_id, sizeof data, data, Callback, callbackArg, encap );
}

//----------------------------------------------------------------------------
// ManufacturerSpecific Report CB
//

//////////////////////////////////////////////////////////////////////////////

//----------------------------------------------------------------------------
znet_error_t ICACHE_FLASH_ATTR znet_cc_device_specific_get( uint8_t node_id,
                                                            uint8_t channel_id,
                                                            uint8_t device_type_id,
                                                            ZFunction_Callback Callback,
                                                            void* callbackArg,
                                                            encap_type_t encap ) {
    uint8_t data[] = { COMMAND_CLASS_MANUFACTURER_SPECIFIC,
                       DEVICE_SPECIFIC_GET_V2,
                       ( device_type_id & DEVICE_SPECIFIC_GET_PROPERTIES1_DEVICE_ID_TYPE_MASK_V2 ) };
    /// TODO: check channel support
    return znet_fc_send_data( node_id, channel_id, sizeof data, data, Callback, callbackArg, encap );
}

//----------------------------------------------------------------------------
// DeviceSpecific Receive CB
//
void ICACHE_FLASH_ATTR znet_cc_manufacturer_specific_requested( const ZFunction func,
                                                                uint8_t node_id,
                                                                int UNUSED( cc_data_len ),
                                                                const uint8_t* cc_data ) {
    assert( cc_data );
    assert( cc_data[1] == MANUFACTURER_SPECIFIC_GET );

    ZW_MANUFACTURER_SPECIFIC_REPORT_FRAME data;
    data.cmdClass = COMMAND_CLASS_MANUFACTURER_SPECIFIC;
    data.cmd = MANUFACTURER_SPECIFIC_REPORT_V2;
    data.manufacturerId1 = MANUFACTURER_ID_1;
    data.manufacturerId2 = MANUFACTURER_ID_2;
    data.productTypeId1 = PRODUCT_TYPE_ID_1;
    data.productTypeId2 = PRODUCT_TYPE_ID_2;
    data.productId1 = PRODUCT_ID_1;
    data.productId2 = PRODUCT_ID_2;

    znet_cc_manufacturer_specific_fill( func, &data );  //  modify by firmware

    znet_fc_send_data( node_id, func->_endpoint, sizeof data, (const uint8_t*)&data, NULL, NULL, func->_security );
}
