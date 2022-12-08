#include "typedefs.h"
#include "ZErrors.h"
#include "znet.h"
#include "znet_fc_private.h"
#include "znet_lib_fc.h"
#include "znet_lib.h"
#include "logger.h"
#include "ZPlatform.h"

//----------------------------------------------------------------------------
static FuncResults_e ICACHE_FLASH_ATTR __SerialAPIGetInitDataResponse( ZFunction this ) {
    ZNet znet = this->_znet;
    Buffer rxed_frame = &znet->_frame;
    uint8_t* data = rxed_frame->_data;
    int size = rxed_frame->_len;

    if( size < 7 ) {
        if( this->_Callback )
            ( *this->_Callback )( this, this->_callbackArg, FUNC_FAILURE_RESPONSE_SIZE, NULL, 0 );
        return FuncResults_Done;
    }
    else {
        if( this->_Callback ) {
            ( *this->_Callback )( this, this->_callbackArg, FUNC_OK, data + 2, size - 2 );
        }
    }
    return FuncResults_Done;
}

//----------------------------------------------------------------------------
static znet_error_t ICACHE_FLASH_ATTR __SerialAPIGetInitDataInit( ZFunction this, void* UNUSED( initArg ) ) {
    CyclicBuffer buffer = &this->_tx_frame;
    if( ( buffer->_data = (uint8_t*)Heap_Alloc( 5 ) ) == NULL )
        return ZNET_ERR_NOMEM;

    buffer->_size = 5;
    buffer->_begin = 0;
    buffer->_len = buffer->_end = this->_tx_frame_initial_len = 4;
    // uint8_t* data = buffer->_data;
    // data[ 0 ] = SOF;
    // data[ 1 ] = 0xFF; // len
    // data[ 2 ] = REQUEST;
    // data[ 3 ] = this->_fc->_id;
    // buffer->_data[ 4 ] = (MODE_NODE_ANY |
    // MODE_NODE_ADD_NODE_OPTION_NETWORK_WIDE | MODE_NODE_OPTION_HIGH_POWER); //
    // 0xc1; // mode  data[ 5 ] = 0xFF; // callback_id  data[ 6 ] = 0xFF; // crc

    return ZNET_ERR_OK;
}

//----------------------------------------------------------------------------
const ZFunctionClass_t fcSerialAPIGetInitData = {
    0x02,                            // _id
    "SerialAPIGetInitData",          //
    __SerialAPIGetInitDataInit,      // _Init
    NULL,                            // _ProcessACK
    __SerialAPIGetInitDataResponse,  // _ProcessRESPONSE
    NULL,                            // _ProcessCALLBACK
    NULL                             // _IsCallbackFrameMine
};

//----------------------------------------------------------------------------
znet_error_t ICACHE_FLASH_ATTR znet_fc_serial_api_get_init_data( ZFunction_Callback Callback, void* callbackArg ) {
    ZFunction func = ZFunction_Create( &fcSerialAPIGetInitData, NULL, Callback, callbackArg );
    if( func == NULL )
        return ZNET_ERR_NOMEM;
    return ZNet_AddFuncToTX( func );
}
