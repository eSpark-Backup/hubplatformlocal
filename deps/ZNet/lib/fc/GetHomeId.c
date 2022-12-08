#include "typedefs.h"
#include "ZErrors.h"
#include "znet.h"
#include "znet_fc_private.h"
#include "znet_lib_fc.h"
#include "logger.h"
#include "ZPlatform.h"
#include "znet_lib_cc_codes.h"

extern void ICACHE_FLASH_ATTR UpdateNodeAndHomeID( uint32_t home_id, uint8_t node_id );

//----------------------------------------------------------------------------
static FuncResults_e ICACHE_FLASH_ATTR __GetHomeIdResponse( ZFunction this ) {
    ZNet znet = this->_znet;
    Buffer rxed_frame = &znet->_frame;
    uint8_t* data = rxed_frame->_data;
    int size = rxed_frame->_len;

    if( size < 7 ) {
        Log_Write( "Get home id failure \r\n" );
        if( this->_Callback )
            ( *this->_Callback )( this, this->_callbackArg, FUNC_FAILURE_RESPONSE_SIZE, NULL, 0 );
        return FuncResults_Done;
    }

    uint32_t home_id = ( uint32_t )( ( data[2] << 24 ) | ( data[3] << 16 ) | ( data[4] << 8 ) | data[5] );
    uint8_t node_id = data[6];
    UpdateNodeAndHomeID( home_id, node_id );

    if( this->_Callback ) {
        ( *this->_Callback )( this, this->_callbackArg, FUNC_OK, data + 2, size - 2 );
    }

    return FuncResults_Done;
}

//----------------------------------------------------------------------------
static void ICACHE_FLASH_ATTR InitGetHomeId( ZFunction this ) {
    CyclicBuffer const buffer = &this->_tx_frame;

    buffer->_size = 6;
    buffer->_begin = 0;
    buffer->_len = buffer->_end = this->_tx_frame_initial_len = 4;
    // uint8_t* data = buffer->_data;
    // data[ 0 ] = SOF;
    // data[ 1 ] = 0xFF; // len
    // data[ 2 ] = REQUEST;
    // data[ 3 ] = this->_fc->_id;
    buffer->_data[4] = 0;
    // data[ 5 ] = 0xFF; // callback_id
    // data[ 6 ] = 0xFF; // crc
}
//----------------------------------------------------------------------------
static znet_error_t ICACHE_FLASH_ATTR __GetHomeIdInit( ZFunction this, void* UNUSED( initArg ) ) {
    CyclicBuffer buffer = &this->_tx_frame;
    if( ( buffer->_data = (uint8_t*)Heap_Alloc( 7 ) ) == NULL )
        return ZNET_ERR_NOMEM;

    InitGetHomeId( this );
    return ZNET_ERR_OK;
}

//----------------------------------------------------------------------------
const ZFunctionClass_t fcGetHomeId = {
    0x20,                 // _id
    "GetHomeId",          //
    __GetHomeIdInit,      // _Init
    NULL,                 // _ProcessACK
    __GetHomeIdResponse,  // _ProcessRESPONSE
    NULL,                 // _ProcessCALLBACK
    NULL                  // _IsCallbackFrameMine
};

//----------------------------------------------------------------------------
znet_error_t ICACHE_FLASH_ATTR znet_fc_get_home_id( ZFunction_Callback Callback, void* callbackArg ) {
    ZFunction func = ZFunction_Create( &fcGetHomeId, NULL, Callback, callbackArg );
    if( func == NULL )
        return ZNET_ERR_NOMEM;
    return ZNet_AddFuncToTX( func );
}
