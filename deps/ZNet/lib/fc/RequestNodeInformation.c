#include "typedefs.h"
#include "ZErrors.h"
#include "znet.h"
#include "ZPlatform.h"
#include "znet_fc_private.h"
#include "znet_lib_fc.h"

//----------------------------------------------------------------------------
static FuncResults_e ICACHE_FLASH_ATTR __RequestNodeInformationResponse( ZFunction this ) {
    ZNet znet = this->_znet;
    Buffer rxed_frame = &znet->_frame;
    uint8_t* data = rxed_frame->_data;
    int size = rxed_frame->_len;

    if( size < 3 ) {
        if( this->_Callback )
            ( *this->_Callback )( this, this->_callbackArg, FUNC_FAILURE_RESPONSE_SIZE, NULL, 0 );
        return FuncResults_Done;
    }

    if( data[2] ) {
        if( this->_Callback ) {
            ( *this->_Callback )( this, this->_callbackArg, FUNC_OK, data + 2, size - 2 );
        }
    }
    else {
        if( this->_Callback )
            ( *this->_Callback )( this, this->_callbackArg, FUNC_FAILURE_RESPONSE_INVALID, NULL, 0 );
    }

    return FuncResults_Done;
}

//----------------------------------------------------------------------------
static znet_error_t ICACHE_FLASH_ATTR __RequestNodeInformationInit( ZFunction this, void* initArg ) {
    CyclicBuffer buffer = &this->_tx_frame;
    if( ( buffer->_data = (uint8_t*)Heap_Alloc( 7 ) ) == NULL )
        return ZNET_ERR_NOMEM;

    buffer->_size = 7;
    buffer->_begin = 0;
    buffer->_len = buffer->_end = this->_tx_frame_initial_len = 5;
    // uint8_t* data = buffer->_data;
    // data[ 0 ] = SOF;
    // data[ 1 ] = 0xFF; // len
    // data[ 2 ] = REQUEST;
    // data[ 3 ] = this->_fc->_id;
    buffer->_data[4] = initArg ? ( *(uint8_t*)initArg ) : 0;
    // data[ 5 ] = 0xFF; // callback_id/crc
    // data[ 6 ] = 0xFF; // crc
    this->_node_id = buffer->_data[4];
    return ZNET_ERR_OK;
}

//----------------------------------------------------------------------------
const ZFunctionClass_t fcRequestNodeInformation = {
    0x60,                              // _id
    "RequestNodeInformation",          //
    __RequestNodeInformationInit,      // _Init
    NULL,                              // _ProcessACK
    __RequestNodeInformationResponse,  // _ProcessRESPONSE
    NULL,                              // _ProcessCALLBACK
    NULL                               // _IsCallbackFrameMine
};

//----------------------------------------------------------------------------
// Request NIF of a device
//
// @param: znet
// ZNet object instance
//
// @param: node_id
// Node Id to be requested for a NIF
//
// @param: Callback
// @default: NULL
// Custom function to be called on function success
// NULL if callback is not needed
//
// @param: callbackArg
// Custom argument to be passed to custom function to be called on function
// success or failure
//
znet_error_t ICACHE_FLASH_ATTR znet_fc_request_node_information( uint8_t node_id,
                                                                 ZFunction_Callback Callback,
                                                                 void* callbackArg ) {
    ZFunction func = ZFunction_Create( &fcRequestNodeInformation, &node_id, Callback, callbackArg );
    if( func == NULL )
        return ZNET_ERR_NOMEM;
    return ZNet_AddFuncToTX( func );
}
