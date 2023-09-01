#include "typedefs.h"

#include "ZErrors.h"
#include "znet.h"
//

#include "znet_fc_private.h"
//#include "ZControllerData.h"

#include "znet_lib_fc.h"

#include "logger.h"

#include "ZPlatform.h"

//----------------------------------------------------------------------------
static FuncResults_e ICACHE_FLASH_ATTR __SendTestFrameCallback( ZFunction this ) {
    ZNet znet = this->_znet;
    Buffer rxed_frame = &znet->_frame;
    uint8_t* data = rxed_frame->_data;
    //  int      size       = rxed_frame->_len;

    Log_Write( "__SendTestFrameCallback %d %d %d %d %d \r\n", data[0], data[1], data[2], data[3], data[4] );

    if( data[3] ) {
        if( this->_Callback )
            ( *this->_Callback )( this, this->_callbackArg, FUNC_FAILURE_GENERAL, NULL, 0 );
    }
    else {
        if( this->_Callback )
            ( *this->_Callback )( this, this->_callbackArg, FUNC_OK, NULL, 0 );
    }

    return FuncResults_Done;
}

//----------------------------------------------------------------------------
static znet_error_t ICACHE_FLASH_ATTR __SendTestFrameInit( ZFunction this, void* initArg ) {
    if( initArg ) {
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
        buffer->_data[4] = initArg ? ( ( (uint8_t*)initArg )[0] ) : 0;
        buffer->_data[5] = initArg ? ( ( (uint8_t*)initArg )[1] ) : 0;
        // data[ 6 ] = 0xFF; // callback_id
        // data[ 7 ] = 0xFF; // crc
        this->_node_id = buffer->_data[4];
    }
    return ZNET_ERR_OK;
}

//----------------------------------------------------------------------------
const ZFunctionClass_t fcSendTestFrame = {
    0xBE,  // _id
    "SendTestFrame",
    __SendTestFrameInit,  // _Init
    NULL,                 // _ProcessACK
    NULL,                 // _ProcessRESPONSE
    // __SendTestFrameResponse, // _ProcessRESPONSE
    __SendTestFrameCallback,  // _ProcessCALLBACK
    // NULL,                // _ProcessTimeout
    NULL  // _IsCallbackFrameMine
};

//----------------------------------------------------------------------------
znet_error_t ICACHE_FLASH_ATTR znet_fc_send_test_frame( uint8_t node_id,
                                                        uint8_t powerLevel,
                                                        ZFunction_Callback Callback,
                                                        void* callbackArg ) {
    uint8_t data[] = { node_id, powerLevel };
    ZFunction func = ZFunction_Create( &fcSendTestFrame, data, Callback, callbackArg );
    if( func == NULL )
        return ZNET_ERR_NOMEM;
    return ZNet_AddFuncToTX_timeout( func, FUNCTION_CLASS_TIMEOUT_60_S );
}
