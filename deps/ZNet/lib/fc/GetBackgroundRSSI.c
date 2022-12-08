#include "typedefs.h"

#include "ZErrors.h"
#include "znet.h"
#include "znet_fc_private.h"
#include "znet_lib_fc.h"
#include "logger.h"
#include "ZPlatform.h"
#include <stdio.h>

//----------------------------------------------------------------------------
static FuncResults_e ICACHE_FLASH_ATTR __GetBackGroundRSSIResponse( ZFunction this ) {
    ZNet znet = this->_znet;
    Buffer rxed_frame = &znet->_frame;
    uint8_t* data = rxed_frame->_data;
    int size = rxed_frame->_len;

    printf( "RSSI level\r\n" );
    for( uint8_t i = 0; i < size - 2; ++i ) {
        printf( "chanel %d, RSSI -%d dBm\r\n", i, ( 224 - data[2 + i] + 32 ) );
    }

    if( this->_Callback )
        ( *this->_Callback )( this, this->_callbackArg, FUNC_FAILURE_GENERAL, NULL, 0 );

    return FuncResults_Done;
}

//----------------------------------------------------------------------------
static znet_error_t ICACHE_FLASH_ATTR __GetBackGroundRSSIInit( ZFunction this, void* initArg ) {
    CyclicBuffer buffer = &this->_tx_frame;
    if( ( buffer->_data = (uint8_t*)Heap_Alloc( 6 ) ) == NULL )
        return ZNET_ERR_NOMEM;

    buffer->_size = 6;
    buffer->_begin = 0;
    buffer->_len = buffer->_end = this->_tx_frame_initial_len = 4;
    // uint8_t* data = buffer->_data;
    // data[ 0 ] = SOF;
    // data[ 1 ] = 0xFF; // len
    // data[ 2 ] = REQUEST;
    // data[ 3 ] = this->_fc->_id;
    // data[ 4 ] = 0xFF; // callback_id
    // data[ 5 ] = 0xFF; // crc
    return ZNET_ERR_OK;
}

//----------------------------------------------------------------------------
const ZFunctionClass_t fcGetBackGroundRSSI = {
    0x3B,                         // _id
    "GetBackGroundRSSI",          // _name
    __GetBackGroundRSSIInit,      // _Init
    NULL,                         // _ProcessACK
    __GetBackGroundRSSIResponse,  // _ProcessRESPONSE
    NULL,                         // _ProcessCALLBACK
    NULL                          // _IsCallbackFrameMine
};

//----------------------------------------------------------------------------
znet_error_t ICACHE_FLASH_ATTR znet_fc_get_background_RSSI( ZFunction_Callback Callback, void* callbackArg ) {
    ZFunction func = ZFunction_Create( &fcGetBackGroundRSSI, NULL, Callback, callbackArg );
    return ZNet_AddFuncToTX( func );
}
