#include "typedefs.h"

#include "znet.h"
//

#include "znet_fc_private.h"
//#include "ZControllerData.h"

#include "znet_lib_fc.h"
#include <S2_wrap.h>

#include "logger.h"

#define ZW_SUC_SET_SUCCEEDED 0x05
#define ZW_SUC_SET_FAILED 0x06

/* SUC capabilities used in ZW_SetSUCNodeID */
#define ZW_SUC_FUNC_NODEID_SERVER 0x01

//----------------------------------------------------------------------------
static FuncResults_e ICACHE_FLASH_ATTR __SetSucNodeIdCallback( ZFunction this ) {
    ZNetConst znet = this->_znet;
    const Buffer rxed_frame = &znet->_frame;
    uint8_t* data = rxed_frame->_data;
    int size = rxed_frame->_len;

    Log_Write( "__SetSucNodeIdCallback\r\n" );
    print_hex_buf( data, size );

    if( ( size == 4 ) && ( data[4] == ZW_SUC_SET_SUCCEEDED ) ) {
        if( this->_Callback ) {
            ( *this->_Callback )( this, this->_callbackArg, FUNC_OK, NULL, 0 );
        }
    }
    else {
        if( this->_Callback ) {
            ( *this->_Callback )( this, this->_callbackArg, FUNC_FAILURE_GENERAL, NULL, 0 );
        }
    }

    return FuncResults_Done;
}

//----------------------------------------------------------------------------
static FuncResults_e ICACHE_FLASH_ATTR __SetSucNodeIdResponse( ZFunction this ) {
    ZNetConst znet = this->_znet;
    const Buffer rxed_frame = &znet->_frame;
    uint8_t* data = rxed_frame->_data;
    int size = rxed_frame->_len;

    Log_Write( "__SetSucNodeIdResponse\r\n" );
    print_hex_buf( data, size );

    if( ( size != 3 ) || ( data[2] == FALSE ) ) {
        Log_Write( "__SetSucNodeIdResponse failure\r\n" );
        if( this->_Callback ) {
            ( *this->_Callback )( this, this->_callbackArg, FUNC_FAILURE_GENERAL, NULL, 0 );
        }
        return FuncResults_Done;
    }

    // According to the docs, in case ZW_SetSUCNodeID is called locally with the controller's own node ID then only the
    // response is returned, so it can be interpreted as the command is now executed successfully.
    if( this->_node_id == MY_NODE_ID ) {
        if( this->_Callback ) {
            ( *this->_Callback )( this, this->_callbackArg, FUNC_OK, NULL, 0 );
        }
        return FuncResults_Done;
    }

    return FuncResults_Next;
}

//----------------------------------------------------------------------------
static znet_error_t ICACHE_FLASH_ATTR __SetSucNodeIdInit( ZFunction this, void* initArg ) {
    CyclicBuffer buffer = &this->_tx_frame;
    if( ( buffer->_data = (uint8_t*)Heap_Alloc( 10 ) ) == NULL )
        return ZNET_ERR_NOMEM;

    buffer->_size = 10;
    buffer->_begin = 0;
    buffer->_len = buffer->_end = this->_tx_frame_initial_len = 8;
    // data[ 0 ] = SOF;
    // data[ 1 ] = 0xFF; // len
    // data[ 2 ] = REQUEST;
    // data[ 3 ] = this->_fc->_id;
    buffer->_data[4] = initArg ? ( *(uint8_t*)initArg ) : 0;  // nodeID
    buffer->_data[5] = TRUE;                                  // SUCState
    buffer->_data[6] = FALSE;                                 // bTxOption
    buffer->_data[7] = ZW_SUC_FUNC_NODEID_SERVER;             // 0      //capabilities
    // data[ 8 ] = 0xFF; // callback_id
    // data[ 9 ] = 0xFF; // crc
    this->_node_id = buffer->_data[4];
    return ZNET_ERR_OK;
}

//----------------------------------------------------------------------------
const ZFunctionClass_t fcSetSUCNodeId = {
    0x54,  // _id
    "SetSUCNodeId",
    __SetSucNodeIdInit,      // _Init
    NULL,                    // _ProcessACK
    __SetSucNodeIdResponse,  // _ProcessRESPONSE
    __SetSucNodeIdCallback,  // _ProcessCALLBACK
    // NULL,                // _ProcessTimeout
    NULL  // _IsCallbackFrameMine
};

//----------------------------------------------------------------------------
znet_error_t ICACHE_FLASH_ATTR znet_fc_set_suc_node_id( uint8_t node_id,
                                                        ZFunction_Callback Callback,
                                                        void* callbackArg ) {
    ZFunction func = ZFunction_Create( &fcSetSUCNodeId, &node_id, Callback, callbackArg );
    if( func == NULL )
        return ZNET_ERR_NOMEM;
    return ZNet_AddFuncToTX( func );
}
