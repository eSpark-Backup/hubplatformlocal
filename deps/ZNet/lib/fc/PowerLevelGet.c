#include "typedefs.h"
#include "ZErrors.h"
#include "znet.h"
#include "znet_fc_private.h"
#include "znet_lib_fc.h"
#include "logger.h"
#include "ZPlatform.h"

//----------------------------------------------------------------------------
static FuncResults_e ICACHE_FLASH_ATTR __PowerLevelGetResponse( ZFunction this ) {
    ZNet znet = this->_znet;
    Buffer rxed_frame = &znet->_frame;
    uint8_t* data = rxed_frame->_data;
    int size = rxed_frame->_len;

    if( size < 3 ) {
        Log_Write( "Power Level Get failure, size = %d \r\n", size );
        if( this->_Callback )
            ( *this->_Callback )( this, this->_callbackArg, FUNC_FAILURE_RESPONSE_SIZE, NULL, 0 );
        return FuncResults_Done;
    }

    Log_Write( "powerlevel = %d \r\n", data[2] );
    uint8_t* argData = Heap_Alloc( size + 1 );
    if( this->_Callback ) {
        ( *this->_Callback )( this, this->_callbackArg, FUNC_OK, data + 2, size - 2 );
    }
    Heap_Free( argData );
    return FuncResults_Done;
}

//----------------------------------------------------------------------------
static znet_error_t ICACHE_FLASH_ATTR __PowerLevelGetInit( ZFunction this, void* UNUSED( initArg ) ) {
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
const ZFunctionClass_t fcRFPowerLevelGet = {
    0xBA,                     // _id
    "PowerLevelGet",          //
    __PowerLevelGetInit,      // _Init
    NULL,                     // _ProcessACK
    __PowerLevelGetResponse,  // _ProcessRESPONSE
    NULL,                     // _ProcessCALLBACK
    NULL                      // _IsCallbackFrameMine
};

//----------------------------------------------------------------------------
znet_error_t ICACHE_FLASH_ATTR znet_fc_power_level_get( ZFunction_Callback Callback, void* callbackArg ) {
    ZFunction func = ZFunction_Create( &fcRFPowerLevelGet, NULL, Callback, callbackArg );
    if( func == NULL )
        return ZNET_ERR_NOMEM;
    return ZNet_AddFuncToTX( func );
}
