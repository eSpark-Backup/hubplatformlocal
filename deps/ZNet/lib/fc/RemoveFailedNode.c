#include "typedefs.h"
#include "ZErrors.h"
#include "znet.h"
#include "znet_fc_private.h"
#include "znet_lib_fc.h"
#include "logger.h"
#include "ZPlatform.h"

#define FAILED_NODE_REMOVE_STARTED 0x00
#define FAILED_NODE_REMOVE_NOT_PRIMARY_CONTROLLER 0x02
#define FAILED_NODE_REMOVE_NO_CALLBACK_FUNCTION 0x04
#define FAILED_NODE_REMOVE_NODE_NOT_FOUND 0x08
#define FAILED_NODE_REMOVE_PROCESS_BUSY 0x10
#define FAILED_NODE_REMOVE_FAIL 0x20

#define FAILED_NODE_OK 0x00
#define FAILED_NODE_REMOVED 0x01
#define FAILED_NODE_NOT_REMOVED 0x02
#define FAILED_NODE_REPLACE 0x03
#define FAILED_NODE_REPLACE_DONE 0x04
#define FAILED_NODE_REPLACE_FAILED 0x05

//----------------------------------------------------------------------------
static FuncResults_e ICACHE_FLASH_ATTR __RemoveFailedNodeResponse( ZFunction this ) {
    ZNet znet = this->_znet;
    Buffer rxed_frame = &znet->_frame;
    uint8_t* data = rxed_frame->_data;
    int size = rxed_frame->_len;

    if( size < 3 ) {
        if( this->_Callback )
            ( *this->_Callback )( this, this->_callbackArg, FUNC_FAILURE_RESPONSE_SIZE, NULL, 0 );
        return FuncResults_Done;
    }

    switch( data[2] ) {
    case FAILED_NODE_REMOVE_STARTED:
        /// Log_Write( "Started");
        return FuncResults_Next;

    case FAILED_NODE_REMOVE_NOT_PRIMARY_CONTROLLER:
        /// Log_Write( "I'm not a primary cntrlr");
    case FAILED_NODE_REMOVE_NO_CALLBACK_FUNCTION:
        /// Log_Write( "No callback function supplied");
    case FAILED_NODE_REMOVE_NODE_NOT_FOUND:
        /// Log_Write( "Node not failed");
    case FAILED_NODE_REMOVE_PROCESS_BUSY:
        /// Log_Write( "Busy");
    case FAILED_NODE_REMOVE_FAIL:
        /// Log_Write( "Failed");
        if( this->_Callback )
            ( *this->_Callback )( this, this->_callbackArg, FUNC_FAILURE_RESPONSE_INVALID, data + 2, size - 2 );
        break;

    default:
        /// Log_Write( "Invalid RemoveFailedNode status: 0x%02x\r\n", data[2]);
        if( this->_Callback )
            ( *this->_Callback )( this, this->_callbackArg, FUNC_FAILURE_GENERAL, NULL, 0 );
        break;
    }
    return FuncResults_Done;
}

//----------------------------------------------------------------------------
static FuncResults_e ICACHE_FLASH_ATTR __RemoveFailedNodeCallback( ZFunction this ) {
    ZNet znet = this->_znet;
    Buffer rxed_frame = &znet->_frame;
    uint8_t* data = rxed_frame->_data;
    int size = rxed_frame->_len;

    if( size < 4 ) {
        if( this->_Callback )
            ( *this->_Callback )( this, this->_callbackArg, FUNC_FAILURE_CALLBACK_SIZE, NULL, 0 );
        return FuncResults_Done;
    }

    switch( data[3] ) {
    case FAILED_NODE_OK:
    case FAILED_NODE_NOT_REMOVED:
        if( this->_Callback )
            ( *this->_Callback )( this, this->_callbackArg, FUNC_FAILURE_CALLBACK_INVALID, data + 2, size - 2 );
        break;

    case FAILED_NODE_REMOVED:
        if( this->_Callback ) {
            ( *this->_Callback )( this, this->_callbackArg, FUNC_OK, data + 2, size - 2 );
        }
        break;

    default:
        /// Log_Write( "Invalid RemoveFailedNode status: 0x%02x\r\n", data[3]);
        if( this->_Callback )
            ( *this->_Callback )( this, this->_callbackArg, FUNC_FAILURE_CALLBACK_INVALID, NULL, 0 );
        break;
    }
    return FuncResults_Done;
}

//----------------------------------------------------------------------------
static znet_error_t ICACHE_FLASH_ATTR __RemoveFailedNodeInit( ZFunction this, void* initArg ) {
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
    buffer->_data[4] = initArg ? ( *(uint8_t*)initArg ) : 0;  // node_id
    // data[ 5 ] = 0xFF; // callback_id
    // data[ 6 ] = 0xFF; // crc
    this->_node_id = buffer->_data[4];
    return ZNET_ERR_OK;
}

//----------------------------------------------------------------------------
const ZFunctionClass_t fcRemoveFailedNode = {
    0x61,                        // _id
    "RemoveFailedNode",          //
    __RemoveFailedNodeInit,      // _Init
    NULL,                        // _ProcessACK
    __RemoveFailedNodeResponse,  // _ProcessRESPONSE
    __RemoveFailedNodeCallback,  // _ProcessCALLBACK
    NULL                         // _IsCallbackFrameMine
};

//----------------------------------------------------------------------------
znet_error_t ICACHE_FLASH_ATTR znet_fc_remove_failed_node( uint8_t node_id,
                                                           ZFunction_Callback Callback,
                                                           void* callbackArg ) {
    ZFunction func = ZFunction_Create( &fcRemoveFailedNode, &node_id, Callback, callbackArg );
    if( func == NULL )
        return ZNET_ERR_NOMEM;
    return ZNet_AddFuncToTX( func );
}
