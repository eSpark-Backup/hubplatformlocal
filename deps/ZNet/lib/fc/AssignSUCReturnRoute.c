#include "typedefs.h"

#include "ZErrors.h"
#include "znet.h"
//

#include "znet_fc_private.h"

//

#include "znet_lib_fc.h"

#include "logger.h"

#include "ZPlatform.h"

//----------------------------------------------------------------------------
static FuncResults_e ICACHE_FLASH_ATTR __AssignSUCReturnRouteResponse( ZFunction this ) {
    ZNet znet = this->_znet;
    Buffer rxed_frame = &znet->_frame;
    //    uint8_t* data = rxed_frame->_data;
    int size = rxed_frame->_len;

    if( size < 3 ) {
        // if( this->_req_attempts-- > 0 ) return FuncResults_Restart;
        if( this->_Callback )
            ( *this->_Callback )( this, this->_callbackArg, FUNC_FAILURE_RESPONSE_SIZE, NULL, 0 );
        return FuncResults_Done;
    }

    return FuncResults_Next;
}

//----------------------------------------------------------------------------
static FuncResults_e ICACHE_FLASH_ATTR __AssignSUCReturnRouteCallback( ZFunction this ) {
    Log_Write( "AssignSUCReturnRoute Callback\r\n" );
    ZNet znet = this->_znet;
    Buffer rxed_frame = &znet->_frame;
    uint8_t* data = rxed_frame->_data;
    int size = rxed_frame->_len;

    print_hex_buf( data, size );

    if( size < 4 ) {
        // if( this->_req_attempts-- > 0 ) return FuncResults_Restart;
        if( this->_Callback )
            ( *this->_Callback )( this, this->_callbackArg, FUNC_FAILURE_CALLBACK_SIZE, NULL, 0 );
        return FuncResults_Done;
    }

    if( data[3] == TRANSMIT_COMPLETE_OK ) {
        Log_Write( "TRANSMIT_COMPLETE_OK" );
        if( this->_Callback )
            ( *this->_Callback )( this, this->_callbackArg, FUNC_OK, NULL, 0 );
    }
    else {
        Log_Write( "AssignSUCReturnRoute , error = %d\r\n", data[3] );
        if( this->_Callback )
            ( *this->_Callback )( this, this->_callbackArg, FUNC_FAILURE_GENERAL, NULL, 0 );
    }
    return FuncResults_Done;
}

//----------------------------------------------------------------------------
static znet_error_t ICACHE_FLASH_ATTR __AssignSUCReturnRouteInit( ZFunction this, void* initArg ) {
    CyclicBuffer buffer = &this->_tx_frame;

    if( ( buffer->_data = (uint8_t*)Heap_Alloc( 8 ) ) == NULL )
        return ZNET_ERR_NOMEM;

    buffer->_size = 8;
    buffer->_begin = 0;
    buffer->_len = buffer->_end = this->_tx_frame_initial_len = 6;
    // uint8_t* data = buffer->_data;
    // data[ 0 ] = SOF;
    // data[ 1 ] = 0xFF; // len
    // data[ 2 ] = REQUEST;
    // data[ 3 ] = this->_fc->_id;
    buffer->_data[4] = initArg ? ( *(uint8_t*)initArg ) : 0;
    buffer->_data[5] = 0;
    // data[ 6 ] = 0xFF; // callback_id
    // data[ 7 ] = 0xFF; // crc
    this->_node_id = buffer->_data[4];
    return ZNET_ERR_OK;
}

//----------------------------------------------------------------------------
const ZFunctionClass_t fcAssignSUCReturnRoute = {
    0x51,  // _id
    "AssignSUCReturnRoute",
    __AssignSUCReturnRouteInit,      // _Init
    NULL,                            // _ProcessACK
    __AssignSUCReturnRouteResponse,  // _ProcessRESPONSE
    __AssignSUCReturnRouteCallback,  // _ProcessCALLBACK
    NULL                             // _IsCallbackFrameMine
};

//----------------------------------------------------------------------------
znet_error_t ICACHE_FLASH_ATTR znet_fc_assign_suc_return_route( uint8_t src_node,
                                                                ZFunction_Callback Callback,
                                                                void* callbackArg ) {
    ZFunction func = ZFunction_Create( &fcAssignSUCReturnRoute, &src_node, Callback, callbackArg );
    if( func == NULL )
        return ZNET_ERR_NOMEM;
    return ZNet_AddFuncToTX( func );
}
