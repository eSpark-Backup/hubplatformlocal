#include "typedefs.h"

#include "byte_ops.h"

#include "ZErrors.h"
#include "znet.h"
//

#include "znet_fc_private.h"

//

#include "znet_lib_fc.h"

#include "ZPlatform.h"

//----------------------------------------------------------------------------
static FuncResults_e ICACHE_FLASH_ATTR __GetNodeProtocolInfoResponse( ZFunction this ) {
    ZNet znet = this->_znet;
    Buffer rxed_frame = &znet->_frame;
    const uint8_t* data = rxed_frame->_data;
    size_t size = rxed_frame->_len;

    if( size < 8 ) {
        if( this->_Callback ) {
            this->_Callback( this, this->_callbackArg, FUNC_FAILURE_RESPONSE_SIZE, NULL, 0 );
        }

        return FuncResults_Done;
    }

    if( this->_Callback ) {
        this->_Callback( this, this->_callbackArg, FUNC_OK, data + 2, size - 2 );
    }

    return FuncResults_Done;
}

//----------------------------------------------------------------------------
static znet_error_t ICACHE_FLASH_ATTR __GetNodeProtocolInfoInit( ZFunction this, void* initArg ) {
    CyclicBuffer buffer = &this->_tx_frame;

    if( ( buffer->_data = (uint8_t*)Heap_Alloc( 6 ) ) == NULL ) {
        return ZNET_ERR_NOMEM;
    }

    buffer->_size = 6;
    buffer->_begin = 0;
    buffer->_len = buffer->_end = this->_tx_frame_initial_len = 5;

    buffer->_data[4] = initArg ? ( *(uint8_t*)initArg ) : 0;
    this->_node_id = buffer->_data[4];
    return ZNET_ERR_OK;
}

//----------------------------------------------------------------------------
const ZFunctionClass_t fcGetNodeProtocolInfo = {
    0x41,  // _id
    "GetNodeProtocolInformation",
    __GetNodeProtocolInfoInit,      // _Init
    NULL,                           // _ProcessACK
    __GetNodeProtocolInfoResponse,  // _ProcessRESPONSE
    NULL,                           // _ProcessCALLBACK
    // NULL,                         // _ProcessTimeout
    NULL  // _IsCallbackFrameMine
    /*
        "GetNodeProtocolInformation",
        "Get node protocol information",
        __GetNodeProtocolInfoResponse,      // response handler
        NULL,                               // callback handler
        NULL,                               // ack handler
        NULL,                               // timeout handler
        //FALSE,                              // ugent
        //FALSE,                              // block
        //FALSE,                              // allow duplicate
        //FALSE,                              // sleeping check
        //FALSE,                              // security
        //TRUE,                               // has node
        //FALSE,                              // send to node
        //0.0f,
        //0.0f,
        //0.0f
    */
};

znet_error_t ICACHE_FLASH_ATTR znet_fc_get_node_protocol_info( uint8_t node_id,
                                                               ZFunction_Callback Callback,
                                                               void* callbackArg ) {
    ZFunction func = ZFunction_Create( &fcGetNodeProtocolInfo, &node_id, Callback, callbackArg );
    if( func == NULL ) {
        return ZNET_ERR_NOMEM;
    }
    return ZNet_AddFuncToTX( func );
}
