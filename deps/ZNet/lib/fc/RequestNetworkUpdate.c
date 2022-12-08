#include "typedefs.h"
#include "ZErrors.h"
#include "znet.h"
#include "znet_fc_private.h"
#include "ZPlatform.h"
#include "znet_lib_fc.h"
#include "logger.h"

#define SUC_UPDATE_DONE 0x00
#define SUC_UPDATE_ABORT 0x01
#define SUC_UPDATE_WAIT 0x02
#define SUC_UPDATE_DISABLED 0x03
#define SUC_UPDATE_OVERFLOW 0x04
#define SUC_SET_SUCCEEDED 0x05
#define SUC_SET_FAILED 0x06

//----------------------------------------------------------------------------
static FuncResults_e ICACHE_FLASH_ATTR __RequestNetworkUpdateResponse( ZFunction this ) {
    ZNet znet = this->_znet;
    Buffer rxed_frame = &znet->_frame;
    uint8_t* data = rxed_frame->_data;
    int size = rxed_frame->_len;

    if( size < 3 ) {
        // if( this->_req_attempts-- > 0 ) return FuncResults_Restart;
        if( this->_Callback )
            ( *this->_Callback )( this, this->_callbackArg, FUNC_FAILURE_RESPONSE_SIZE, NULL, 0 );
        return FuncResults_Done;
    }

    if( data[2] ) {
        /// Log_Write( "RequestNetworkUpdate: started");
        return FuncResults_Next;
    }
    else {
        /// Log_Write( "RequestNetworkUpdate: failed");
        if( this->_Callback )
            ( *this->_Callback )( this, this->_callbackArg, FUNC_FAILURE_RESPONSE_INVALID, NULL, 0 );
    }

    return FuncResults_Done;
}

//----------------------------------------------------------------------------
static FuncResults_e ICACHE_FLASH_ATTR __RequestNetworkUpdateCallback( ZFunction this ) {
    ZNet znet = this->_znet;
    Buffer rxed_frame = &znet->_frame;
    uint8_t* data = rxed_frame->_data;
    int size = rxed_frame->_len;

    if( size < 4 ) {
        // if( this->_req_attempts-- > 0 ) return FuncResults_Restart;
        if( this->_Callback )
            ( *this->_Callback )( this, this->_callbackArg, FUNC_FAILURE_CALLBACK_SIZE, NULL, 0 );
        return FuncResults_Done;
    }

    switch( data[3] ) {
    case SUC_UPDATE_DONE:
        /// Log_Write( "Network update done");
        if( this->_Callback ) {
            ( *this->_Callback )( this, this->_callbackArg, FUNC_OK, data + 2, size - 2 );
        }
        break;

    case SUC_UPDATE_ABORT:
        /// Log_Write( "Network update failed due to some error");
    /// break;
    case SUC_UPDATE_WAIT:
        /// Log_Write( "Network update failed, because SUC is busy");
    /// break;
    case SUC_UPDATE_DISABLED:
        /// Log_Write( "Network update failed, because SUC is disabled");
    /// break;
    case SUC_UPDATE_OVERFLOW:
        /// Log_Write( "Network update failed due to lots of changes since last
        if( this->_Callback )
            ( *this->_Callback )( this, this->_callbackArg, data[3], NULL, 0 );
        break;

    default:
        /// Log_Write( "Invalid RequestNetworkUpdate status: 0x%02x\r\n",
        /// data[3]);
        if( this->_Callback )
            ( *this->_Callback )( this, this->_callbackArg, FUNC_FAILURE_CALLBACK_INVALID, NULL, 0 );
        break;
    }

    return FuncResults_Done;
}

//----------------------------------------------------------------------------
static znet_error_t ICACHE_FLASH_ATTR __RequestNetworkUpdateInit( ZFunction this, void* initArg ) {
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
    // buffer->_data[4] = initArg ? ( *(uint8_t*)initArg ) : 0;  // node_id
    // data[ 5 ] = 0xFF; // callback_id
    // data[ 6 ] = 0xFF; // crc
    return ZNET_ERR_OK;
}
//----------------------------------------------------------------------------
const ZFunctionClass_t fcRequestNetworkUpdate = {
    0x53,  // _id
    "RequestNetworkUpdate",
    __RequestNetworkUpdateInit,      // _Init
    NULL,                            // _ProcessACK
    __RequestNetworkUpdateResponse,  // _ProcessRESPONSE
    __RequestNetworkUpdateCallback,  // _ProcessCALLBACK
    // NULL,                           // _ProcessTimeout
    NULL  // _IsCallbackFrameMine
};

//----------------------------------------------------------------------------
znet_error_t ICACHE_FLASH_ATTR znet_fc_request_network_update( ZFunction_Callback Callback, void* callbackArg ) {
    ZFunction func = ZFunction_Create( &fcRequestNetworkUpdate, NULL, Callback, callbackArg );
    if( func == NULL )
        return ZNET_ERR_NOMEM;
    return ZNet_AddFuncToTX( func );
}
