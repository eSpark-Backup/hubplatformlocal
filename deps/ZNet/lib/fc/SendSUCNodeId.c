#include "typedefs.h"

#include "ZErrors.h"
#include "znet.h"
//

#include "znet_fc_private.h"

#include "znet_lib_fc.h"

#include "logger.h"
#include "ZPlatform.h"

//----------------------------------------------------------------------------
static FuncResults_e ICACHE_FLASH_ATTR __SendSUCNodeIdResponse( ZFunction this ) {
    ZNet znet = this->_znet;
    Buffer rxed_frame = &znet->_frame;
    uint8_t* data = rxed_frame->_data;
    int size = rxed_frame->_len;

    Log_Write( "__SendSUCNodeIdResponse\r\n" );
    print_hex_buf( data, size );

    if( size < 3 ) {
        if( this->_Callback )
            ( *this->_Callback )( this, this->_callbackArg, FUNC_FAILURE_RESPONSE_SIZE, NULL, 0 );
        return FuncResults_Done;
    }

    if( data[2] == 1 ) {
        return FuncResults_Next;
    }
    else {
        if( this->_Callback )
            ( *this->_Callback )( this, this->_callbackArg, FUNC_FAILURE_RESPONSE_INVALID, NULL, 0 );
    }

    return FuncResults_Done;
}

//----------------------------------------------------------------------------
static FuncResults_e ICACHE_FLASH_ATTR __SendSUCNodeIdCallback( ZFunction this ) {
    ZNet znet = this->_znet;
    Buffer rxed_frame = &znet->_frame;
    uint8_t* data = rxed_frame->_data;
    int size = rxed_frame->_len;

    Log_Write( "__SendSUCNodeIdCallback\r\n" );
    print_hex_buf( data, size );

    if( size < 4 ) {
        if( this->_Callback )
            ( *this->_Callback )( this, this->_callbackArg, FUNC_FAILURE_CALLBACK_SIZE, NULL, 0 );
        return FuncResults_Done;
    }

    switch( data[3] ) {
    case TRANSMIT_COMPLETE_NO_ACK:
    case TRANSMIT_COMPLETE_FAIL:
        if( this->_Callback )
            ( *this->_Callback )( this, this->_callbackArg, FUNC_FAILURE_GENERAL, NULL, 0 );
        return FuncResults_Done;
        break;

    case TRANSMIT_COMPLETE_NOROUTE:
        break;

    case TRANSMIT_COMPLETE_OK:
        if( this->_Callback ) {
            ( *this->_Callback )( this, this->_callbackArg, FUNC_OK, NULL, 0 );
        }
        break;

    default:

        break;
    }

    return FuncResults_Done;
}

//----------------------------------------------------------------------------
static znet_error_t ICACHE_FLASH_ATTR __SendSucNodeIdInit( ZFunction this, void* initArg ) {
    CyclicBuffer buffer = &this->_tx_frame;
    if( ( buffer->_data = (uint8_t*)Heap_Alloc( 10 ) ) == NULL )
        return ZNET_ERR_NOMEM;

    buffer->_size = 8;
    buffer->_begin = 0;
    buffer->_len = buffer->_end = this->_tx_frame_initial_len = 6;
    // data[ 0 ] = SOF;
    // data[ 1 ] = 0xFF; // len
    // data[ 2 ] = REQUEST;
    // data[ 3 ] = this->_fc->_id;
    buffer->_data[4] = initArg ? ( *(uint8_t*)initArg ) : 0;              // nodeID
    buffer->_data[5] = TRANSMIT_OPTION_ACK | TRANSMIT_OPTION_AUTO_ROUTE;  // TxOption
    // data[ 6 ] = 0xFF; // callback_id
    // data[ 7 ] = 0xFF; // crc
    this->_node_id = buffer->_data[4];
    return ZNET_ERR_OK;
}

//----------------------------------------------------------------------------
const ZFunctionClass_t fcSendSUCNodeId = {
    0x57  // _id
    ,
    "SendSUCNodeId",
    __SendSucNodeIdInit  // _Init
    ,
    NULL  // _ProcessACK
    ,
    __SendSUCNodeIdResponse  // _ProcessRESPONSE
    ,
    __SendSUCNodeIdCallback  // _ProcessCALLBACK
    //, NULL                    // _ProcessTimeout
    ,
    NULL  // _IsCallbackFrameMine

};

//----------------------------------------------------------------------------
znet_error_t ICACHE_FLASH_ATTR znet_fc_send_suc_node_id( uint8_t node_id,
                                                         ZFunction_Callback Callback,
                                                         void* callbackArg ) {
    ZFunction func = ZFunction_Create( &fcSendSUCNodeId, &node_id, Callback, callbackArg );
    if( func == NULL )
        return ZNET_ERR_NOMEM;
    return ZNet_AddFuncToTX( func );
}
