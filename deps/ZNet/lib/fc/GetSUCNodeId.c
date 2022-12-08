#include "typedefs.h"

#include "ZErrors.h"
#include "znet.h"

#include "znet_fc_private.h"

#include "znet_lib_fc.h"

#include "logger.h"
#include "ZPlatform.h"

//----------------------------------------------------------------------------
static FuncResults_e ICACHE_FLASH_ATTR __GetSUCNodeIdResponse( ZFunction this ) {
    ZNet znet = this->_znet;
    Buffer rxed_frame = &znet->_frame;
    uint8_t* data = rxed_frame->_data;
    int size = rxed_frame->_len;

    Log_Write( "__GetSUCNodeIdResponse\r\n" );
    print_hex_buf( data, size );

    if( size < 3 ) {
        if( this->_Callback )
            ( *this->_Callback )( this, this->_callbackArg, FUNC_FAILURE_RESPONSE_SIZE, NULL, 0 );
        return FuncResults_Done;
    }

    if( this->_Callback ) {
        ( *this->_Callback )( this, this->_callbackArg, FUNC_OK, data + 2, size - 2 );
    }

    return FuncResults_Done;
}

//----------------------------------------------------------------------------
static znet_error_t ICACHE_FLASH_ATTR __GetSUCNodeIdInit( ZFunction this, void* initArg ) {
    CyclicBuffer buffer = &this->_tx_frame;
    if( ( buffer->_data = (uint8_t*)Heap_Alloc( 6 ) ) == NULL )
        return ZNET_ERR_NOMEM;

    buffer->_size = 6;
    buffer->_begin = 0;
    buffer->_len = buffer->_end = this->_tx_frame_initial_len = 4;
    // data[ 0 ] = SOF;
    // data[ 1 ] = 0xFF; // len
    // data[ 2 ] = REQUEST;
    // data[ 3 ] = this->_fc->_id;
    return ZNET_ERR_OK;
}

//----------------------------------------------------------------------------
const ZFunctionClass_t fcGetSUCNodeId = {
    0x56,                    // _id
    "GetSUCNodeId",          //
    __GetSUCNodeIdInit,      // _Init
    NULL,                    // _ProcessACK
    __GetSUCNodeIdResponse,  // _ProcessRESPONSE
    NULL,                    // _ProcessCALLBACK
    NULL                     // _IsCallbackFrameMine

};

//----------------------------------------------------------------------------
znet_error_t ICACHE_FLASH_ATTR znet_fc_get_suc_node_id( ZFunction_Callback Callback, void* callbackArg ) {
    ZFunction func = ZFunction_Create( &fcGetSUCNodeId, NULL, Callback, callbackArg );
    if( func == NULL )
        return ZNET_ERR_NOMEM;
    return ZNet_AddFuncToTX( func );
}
