#include "typedefs.h"
#include "ZErrors.h"
#include "znet.h"
#include "znet_fc_private.h"
#include "znet_lib_fc.h"
#include "logger.h"
#include "ZPlatform.h"

extern void ICACHE_FLASH_ATTR LearnModeInvokedCallback();

//----------------------------------------------------------------------------
static FuncResults_e ICACHE_FLASH_ATTR __SetLearnModeCallback( ZFunction this ) {
    Log_Write( "__SetLearnModeCallback\r\n" );
    ZNet znet = this->_znet;
    Buffer rxed_frame = &znet->_frame;
    uint8_t* data = rxed_frame->_data;
    int size = rxed_frame->_len;

    if( size < 4 )
        return FuncResults_Done;

    switch( data[3] ) {
    case NODE_STATUS_STARTED:
        Log_Write( "Learn started\r\n" );

        if( size < 6 ) {
            Log_Write( "Error: Wrong data size\n" );
            if( this->_Callback )
                ( *this->_Callback )( this, this->_callbackArg, FUNC_FAILURE_CALLBACK_SIZE, NULL, 0 );
            return FuncResults_Done;
        }
        if( this->_Callback ) {
            ( *this->_Callback )( this, this->_callbackArg, FUNC_OK, data + 2, size - 2 );
        }
        this->_timeout = SystemTime_Now() + SystemTime_ms( FUNCTION_CLASS_TIMEOUT_20_S );
        return FuncResults_Repeat;

    case NODE_STATUS_DONE:
        Log_Write( "Learn Done\r\n" );
        if( size < 6 ) {
            Log_Write( "Error: Wrong data size\n" );
            if( this->_Callback )
                ( *this->_Callback )( this, this->_callbackArg, FUNC_FAILURE_CALLBACK_SIZE, NULL, 0 );
            return FuncResults_Done;
        }
        if( this->_Callback ) {
            ( *this->_Callback )( this, this->_callbackArg, FUNC_OK, data + 2, size - 2 );
        }
        return FuncResults_Done;

    case NODE_STATUS_FAILED:
        Log_Write( "Learn Failed\r\n" );
        if( this->_Callback ) {
            ( *this->_Callback )( this, this->_callbackArg, FUNC_FAILURE_GENERAL, NULL, 0 );
        }
        return FuncResults_Done;

    default:
        Log_Write( "Learn Invalid SetLearnMode status: %02x!\r\n", data[3] );
        return FuncResults_Done;
    }
}

//----------------------------------------------------------------------------
static FuncResults_e ICACHE_FLASH_ATTR __SetLearnModeResponse( ZFunction this ) {
    ZNet znet = this->_znet;
    Buffer rxed_frame = &znet->_frame;
    int size = rxed_frame->_len;
    Log_Write( "__SetLearnModeResponse %d\r\n", size );

    if( size != 3 ) {
        if( this->_Callback )
            ( *this->_Callback )( this, this->_callbackArg, FUNC_FAILURE_RESPONSE_SIZE, NULL, 0 );
        return FuncResults_Done;
    }

    CyclicBuffer tx_frame = &this->_tx_frame;
    if( tx_frame->_data[4] != ZW_SET_LEARN_MODE_DISABLE )
        LearnModeInvokedCallback();
    else {
        if( this->_Callback ) {
            ( *this->_Callback )( this, this->_callbackArg, FUNC_OK, NULL, 0 );
        }
        return FuncResults_Done;
    }

    return FuncResults_Next;
}

//----------------------------------------------------------------------------
static znet_error_t ICACHE_FLASH_ATTR __SetLearnModeInit( ZFunction this, void* initArg ) {
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
    // data[ 5 ] = 0xFF; // callback_id
    // data[ 6 ] = 0xFF; // crc
    return ZNET_ERR_OK;
}

//----------------------------------------------------------------------------
const ZFunctionClass_t fcSetLearnMode = {
    0x50,                    // _id
    "SetLearnMode",          //
    __SetLearnModeInit,      // _Init
    NULL,                    // _ProcessACK
    __SetLearnModeResponse,  // _ProcessRESPONSE
    __SetLearnModeCallback,  // _ProcessCALLBACK
    NULL                     // _IsCallbackFrameMine
};

//----------------------------------------------------------------------------
znet_error_t ICACHE_FLASH_ATTR znet_fc_set_learn_mode( uint8_t learnMode,
                                                       ZFunction_Callback Callback,
                                                       void* callbackArg ) {
    //	NOTE: Learn mode should only be enabled when necessary and disabled again as quickly as possible.
    //	It is recommended that learn mode is not enabling for more than 2 second in
    //	ZW_SET_LEARN_MODE_CLASSIC mode and 5 seconds in either ZW_SET_LEARN_MODE_NWI or
    //	ZW_SET_LEARN_MODE_NWE mode.
    uint32_t timeout;
    if( learnMode == ZW_SET_LEARN_MODE_DISABLE )
        timeout = FUNCTION_CLASS_TIMEOUT_500_MS;
    else if( learnMode == ZW_SET_LEARN_MODE_CLASSIC )
        timeout = FUNCTION_CLASS_TIMEOUT_3_S;
    else if( ( learnMode == ZW_SET_LEARN_MODE_NWE ) || ( learnMode == ZW_SET_LEARN_MODE_NWI ) )
        timeout = FUNCTION_CLASS_TIMEOUT_5_S;
    else {
        Log_Write( "znet_fc_set_learn_mode ZNET_ERR_INVAL\r\n" );
        return ZNET_ERR_INVAL;
    }

    ZFunction func = ZFunction_Create( &fcSetLearnMode, &learnMode, Callback, callbackArg );
    if( func == NULL )
        return ZNET_ERR_NOMEM;

    if( learnMode == ZW_SET_LEARN_MODE_DISABLE ) {
        func->_ProcessRESPONSE = NULL;
        func->_ProcessCALLBACK = NULL;
    }

    ZFunction_Free_From_All_List( &fcSetLearnMode );
    return ZNet_AddFuncToTX_timeout( func, timeout );
}
