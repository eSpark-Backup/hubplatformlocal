#include "typedefs.h"

#include "ZErrors.h"
#include "znet.h"
#include "znet_main.h"
//
//

#include "ZPlatform.h"
#include "znet_fc_private.h"
//#include "znet_cc_private.h"

#include "znet_lib_fc.h"

#include "SendData.h"
#include "S2_wrap.h"
#include "Security_Scheme0.h"

#include "logger.h"

//----------------------------------------------------------------------------
FuncResults_e ICACHE_FLASH_ATTR __SendDataMultiResponse( ZFunction this ) {
    ZNetConst znet = this->_znet;
    const Buffer rxed_frame = &znet->_frame;
    uint8_t* data = rxed_frame->_data;
    int size = rxed_frame->_len;

    Log_Write( "__SendDataMultiResponse \r\n" );

    if( size < 3 ) {
        // if( this->_req_attempts-- > 0 ) return FuncResults_Restart;
        if( this->_Callback )
            ( *this->_Callback )( this, this->_callbackArg, FUNC_FAILURE_RESPONSE_SIZE, NULL, 0 );
        return FuncResults_Done;
    }

    if( data[2] == 1 ) {
        // znet_log(znet, Debug, "Delivered to Z-Wave stack\r\n");
        return FuncResults_Next;
    }
    else {
        if( this->_Callback )
            ( *this->_Callback )( this, this->_callbackArg, FUNC_FAILURE_RESPONSE_INVALID, NULL, 0 );
        return FuncResults_Done;
    }
}

//----------------------------------------------------------------------------
FuncResults_e ICACHE_FLASH_ATTR __SendDataMultiCallback( ZFunction this ) {
    ZNetConst znet = this->_znet;
    const Buffer rxed_frame = &znet->_frame;
    uint8_t* data = rxed_frame->_data;
    int size = rxed_frame->_len;

    Log_Write( "__SendDataMultiCallback size = %d, %d \r\n", size, data[3] );

    if( size < 4 ) {
        // return ( ( this->_req_attempts-- > 0 ) ? FuncResults_Restart :
        // FuncResults_Done );
        if( this->_Callback )
            ( *this->_Callback )( this, this->_callbackArg, FUNC_FAILURE_CALLBACK_SIZE, NULL, 0 );
        return FuncResults_Done;
    }

    switch( data[3] ) {
    case TRANSMIT_COMPLETE_NO_ACK:
        if( this->_Callback ) {
            ( *this->_Callback )( this, this->_callbackArg, FUNC_FAILURE_NO_ACK, data + 2, size - 2 );
        }
        break;

    case TRANSMIT_COMPLETE_FAIL:
        // can't deliver frame
    case TRANSMIT_COMPLETE_NOROUTE:
        // can't deliver frame due to no route
        if( this->_Callback ) {
            ( *this->_Callback )( this, this->_callbackArg, FUNC_FAILURE_GENERAL, data + 2, size - 2 );
        }
        break;

    case TRANSMIT_COMPLETE_OK:
        // command reception acknowledged by node
        if( this->_Callback ) {
            ( *this->_Callback )( this, this->_callbackArg, FUNC_OK, data + 2, size - 2 );
        }
        return FuncResults_Done;

    default:
        // znet_log(znet, Error, "SendData Response with callback 0x%02x
        break;
    }
    return FuncResults_Done;
}

//----------------------------------------------------------------------------
static znet_error_t ICACHE_FLASH_ATTR __SendDataMultiInit( ZFunction this, void* initArg ) {
    if( initArg ) {
        CyclicBuffer buffer = &this->_tx_frame;
        const uint8_t sendSingleCasts = ( (uint8_t*)initArg )[0];
        const uint8_t dataLen = ( (uint8_t*)initArg )[1];
        const uint8_t size = 1 /*SOF*/ + 1 /*frame_len*/ + 1 /*frame_type*/ + 1 /*func_id*/ + dataLen + 1 /*mode*/ +
                             1 /*callback_id*/ + 1 /*CRC*/;
        if( ( buffer->_data = (uint8_t*)Heap_Alloc( size ) ) == NULL )
            return ZNET_ERR_NOMEM;

        buffer->_size = size;
        buffer->_begin = 0;
        buffer->_len = buffer->_end = this->_tx_frame_initial_len = size - 2;

        memcpy( &buffer->_data[4], &( (uint8_t*)initArg )[2], dataLen );
        buffer->_data[size - 3] =
            sendSingleCasts ? TRANSMIT_OPTION_AUTO_ROUTE | TRANSMIT_OPTION_ACK : TRANSMIT_OPTION_AUTO_ROUTE;

        Heap_Free( initArg );
    }

    return ZNET_ERR_OK;
}

//----------------------------------------------------------------------------
static znet_error_t ICACHE_FLASH_ATTR __SendDataMultiBridgeInit( ZFunction this, void* initArg ) {
    if( initArg ) {
        CyclicBuffer buffer = &this->_tx_frame;
        const uint8_t sendSingleCasts = ( (uint8_t*)initArg )[0];
        const uint8_t dataLen = ( (uint8_t*)initArg )[1];
        const uint8_t size = 1 /*SOF*/ + 1 /*frame_len*/ + 1 /*frame_type*/ + 1 /*func_id*/ + 1 /*srcNodeID*/ +
                             dataLen + 1 /*mode*/ + 1 /*callback_id*/ + 1 /*CRC*/;
        if( ( buffer->_data = (uint8_t*)Heap_Alloc( size ) ) == NULL )
            return ZNET_ERR_NOMEM;

        buffer->_size = size;
        buffer->_begin = 0;
        buffer->_len = buffer->_end = this->_tx_frame_initial_len = size - 2;

        buffer->_data[4] = MY_NODE_ID;
        memcpy( &buffer->_data[5], &( (uint8_t*)initArg )[2], dataLen );
        buffer->_data[size - 3] =
            sendSingleCasts ? TRANSMIT_OPTION_AUTO_ROUTE | TRANSMIT_OPTION_ACK : TRANSMIT_OPTION_AUTO_ROUTE;

        Heap_Free( initArg );
    }

    return ZNET_ERR_OK;
}
//----------------------------------------------------------------------------
const ZFunctionClass_t fcSendDataMulti = {
    0x14,                     // _id
    "SendDataMulti",          // _name
    __SendDataMultiInit,      // _Init
    NULL,                     // _ProcessACK
    __SendDataMultiResponse,  // _ProcessRESPONSE
    __SendDataMultiCallback,  // _ProcessCALLBACK
    // __SendDataTimeout,     // _ProcessTimeout
    NULL  // _IsCallbackFrameMine
};

//----------------------------------------------------------------------------
const ZFunctionClass_t fcSendDataMultiBridge = {
    0xAB,                       // _id
    "SendDataMultiBridge",      // _name
    __SendDataMultiBridgeInit,  // _Init
    NULL,                       // _ProcessACK
    __SendDataMultiResponse,    // _ProcessRESPONSE
    __SendDataMultiCallback,    // _ProcessCALLBACK
    // __SendDataTimeout,     // _ProcessTimeout
    NULL  // _IsCallbackFrameMine
};

//----------------------------------------------------------------------------
znet_error_t ICACHE_FLASH_ATTR znet_fc_send_data_multi( uint8_t nodeQuantity,
                                                        const uint8_t* listNodeId,
                                                        uint8_t length,
                                                        const uint8_t* data,
                                                        bool sendSingleCasts,
                                                        ZFunction_Callback Callback,
                                                        void* callbackArg ) {
    const uint8_t full_length = 1 /*numberNodes*/ + nodeQuantity + 1 /*dataLength*/ + length;
    uint8_t* send_data = Heap_Alloc( 1 /*sendSingleCasts*/ + 1 /*full_length*/ + full_length );
    send_data[0] = (uint8_t)sendSingleCasts;
    send_data[1] = full_length;
    send_data[2] = nodeQuantity;
    memcpy( &send_data[3], listNodeId, nodeQuantity );
    send_data[nodeQuantity + 3] = length;
    memcpy( &send_data[nodeQuantity + 4], data, length );

    ZFunction func = NULL;
    if( Z_WAVE_LIBRARY_TYPE == ZW_LIB_CONTROLLER_BRIDGE ) {
        func = ZFunction_Create( &fcSendDataMultiBridge, send_data, Callback, callbackArg );
    }
    else {
        func = ZFunction_Create( &fcSendDataMulti, send_data, Callback, callbackArg );
    }
    func->_timeout = 0;

    if( func == NULL )
        return ZNET_ERR_NOMEM;

    for( uint8_t i = 0; i < nodeQuantity; ++i ) {
        ZNet_RemoveAllFuncToTxForNode( listNodeId[i] );
    }

    return ZNet_AddFuncToTX_timeout( func, FUNCTION_CLASS_TIMEOUT_5_S );
}
