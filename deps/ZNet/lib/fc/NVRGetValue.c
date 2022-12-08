#include "typedefs.h"
#include "ZErrors.h"
#include "znet.h"
#include "znet_fc_private.h"
#include "znet_lib_fc.h"
#include "logger.h"
#include "ZPlatform.h"

//----------------------------------------------------------------------------
static FuncResults_e ICACHE_FLASH_ATTR __NVRGetValueResponse( ZFunction this ) {
    ZNet znet = this->_znet;
    Buffer rxed_frame = &znet->_frame;
    uint8_t* data = rxed_frame->_data;
    int size = rxed_frame->_len;

    Log_Write( "NVRGetValueResponse \r\n" );
    print_hex_buf( data, size );

    if( size < 7 ) {
        Log_Write( "NVRGetValue failure \r\n" );
        if( this->_Callback )
            ( *this->_Callback )( this, this->_callbackArg, FUNC_FAILURE_RESPONSE_SIZE, NULL, 0 );
        return FuncResults_Done;
    }

    if( this->_Callback ) {
        ( *this->_Callback )( this, this->_callbackArg, FUNC_OK, data + 2, size - 2 );
    }

    return FuncResults_Done;
}

//----------------------------------------------------------------------------
static znet_error_t ICACHE_FLASH_ATTR __NVRGetValueInit( ZFunction this, void* initArg ) {
    CyclicBuffer buffer = &this->_tx_frame;
    if( ( buffer->_data = (uint8_t*)Heap_Alloc( 8 ) ) == NULL )
        return ZNET_ERR_NOMEM;

    buffer->_size = 7;
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
    return ZNET_ERR_OK;
}

//----------------------------------------------------------------------------
const ZFunctionClass_t fcNVRGetValue = {
    0x28,                   // _id
    "NVRGetValue",          //
    __NVRGetValueInit,      // _Init
    NULL,                   // _ProcessACK
    __NVRGetValueResponse,  // _ProcessRESPONSE
    NULL,                   // _ProcessCALLBACK
    NULL                    // _IsCallbackFrameMine
};

//----------------------------------------------------------------------------
znet_error_t ICACHE_FLASH_ATTR znet_fc_nvr_get_value( uint8_t offset,
                                                      uint8_t length,
                                                      ZFunction_Callback Callback,
                                                      void* callbackArg ) {
    uint8_t dat[] = { offset, length };
    ZFunction func = ZFunction_Create( &fcNVRGetValue, dat, Callback, callbackArg );
    if( func == NULL )
        return ZNET_ERR_NOMEM;
    return ZNet_AddFuncToTX( func );
}
