#include "typedefs.h"
#include "ZErrors.h"
#include "znet.h"
#include "znet_fc_private.h"
#include "ZPlatform.h"
#include "znet_lib_fc.h"
#include "logger.h"

/* Callback states from ZW_REQUEST_NODE_NEIGHBOR_UPDATE */
#define REQUEST_NEIGHBOR_UPDATE_STARTED 0x21
#define REQUEST_NEIGHBOR_UPDATE_DONE 0x22
#define REQUEST_NEIGHBOR_UPDATE_FAILED 0x23

//----------------------------------------------------------------------------
static FuncResults_e ICACHE_FLASH_ATTR __RequestNodeNeighborUpdateCallback( ZFunction this ) {
    ZNet znet = this->_znet;
    Buffer rxed_frame = &znet->_frame;
    uint8_t* data = rxed_frame->_data;
    int size = rxed_frame->_len;

    if( size < 4 ) {
        if( this->_Callback )
            ( *this->_Callback )( this, this->_callbackArg, FUNC_FAILURE_CALLBACK_SIZE, NULL, 0 );
        return FuncResults_Done;
    }

    Log_Write( "Started  %d %d\r\n", data[1], data[2] );

    switch( data[3] ) {
    case REQUEST_NEIGHBOR_UPDATE_STARTED:
        Log_Write( "NEIGHBOR_UPDATE_STARTED\r\n" );
        return FuncResults_Next;
    case REQUEST_NEIGHBOR_UPDATE_DONE:
        Log_Write( "NEIGHBOR_UPDATE_DONE\r\n" );
        if( this->_Callback ) {
            ( *this->_Callback )( this, this->_callbackArg, FUNC_OK, data + 2, size - 2 );
        }
        break;
    case REQUEST_NEIGHBOR_UPDATE_FAILED:
        Log_Write( "NEIGHBOR_UPDATE_FAILED\r\n" );
        if( this->_Callback ) {
            ( *this->_Callback )( this, this->_callbackArg, FUNC_FAILURE_GENERAL, data + 2, size - 2 );
        }
        break;
    }
    return FuncResults_Done;
}

//----------------------------------------------------------------------------
static znet_error_t ICACHE_FLASH_ATTR __RequestNodeNeighborUpdateInit( ZFunction this, void* initArg ) {
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
const ZFunctionClass_t fcRequestNodeNeighborUpdate = {
    0x48,                                 // _id
    "RequestNodeNeighborUpdate",          // _name
    __RequestNodeNeighborUpdateInit,      // _Init
    NULL,                                 // _ProcessACK
    NULL,                                 // _ProcessRESPONSE
    __RequestNodeNeighborUpdateCallback,  // _ProcessCALLBACK
    NULL                                  // _IsCallbackFrameMine
};

//----------------------------------------------------------------------------
znet_error_t ICACHE_FLASH_ATTR znet_fc_request_node_neighbor_update( uint8_t nodeId,
                                                                     ZFunction_Callback Callback,
                                                                     void* callbackArg ) {
#ifdef ZNET_SUPPORT_PENDING_COMMANDS_FOR_SLEEP_NODES
    if( !ZNet_IsWakingNode( nodeId ) ) {
        ZNET_LOGD( "ZW_RequestNodeNeighborUpdate queueing call, node_id=%02x\n", nodeId );
        znet_error_t add_waiting_status =
            ZNet_AddWaitingCommand( fcRequestNodeNeighborUpdate._id, nodeId, 0, Callback, callbackArg, 0, NULL, 0 );
        return add_waiting_status == ZNET_ERR_OK ? ZNET_ERR_INPROGR : add_waiting_status;
    }
#endif

    ZFunction func = ZFunction_Create( &fcRequestNodeNeighborUpdate, &nodeId, Callback, callbackArg );
    if( func == NULL )
        return ZNET_ERR_NOMEM;
    return ZNet_AddFuncToTX_timeout( func, FUNCTION_CLASS_TIMEOUT_1_MIN );
}
