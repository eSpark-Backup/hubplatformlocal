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
static FuncResults_e ICACHE_FLASH_ATTR __SetThresholdResponse( ZFunction this ) {
    ZNet znet = this->_znet;

    if( this->_Callback )
        ( *this->_Callback )( this, this->_callbackArg, FUNC_FAILURE_GENERAL, NULL, 0 );

    return FuncResults_Done;
}

//----------------------------------------------------------------------------
static znet_error_t ICACHE_FLASH_ATTR __SetThresholdInit( ZFunction this, void* initArg ) {
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
    buffer->_data[4] = ( (uint8_t*)initArg )[0];
    buffer->_data[5] = ( (uint8_t*)initArg )[1];
    // data[ 6 ] = 0xFF; // callback_id
    // data[ 7 ] = 0xFF; // crc
    return ZNET_ERR_OK;
}

//----------------------------------------------------------------------------
const ZFunctionClass_t fcSetThreshold = {
    0x3C,  // _id
    "SetThreshold",
    __SetThresholdInit,      // _Init
    NULL,                    // _ProcessACK
    __SetThresholdResponse,  // _ProcessRESPONSE
    NULL,                    // _ProcessCALLBACK
    NULL                     // _IsCallbackFrameMine
};

//----------------------------------------------------------------------------
znet_error_t ICACHE_FLASH_ATTR znet_fc_set_threshold( uint8_t chanel_id,
                                                      uint8_t threshold,
                                                      ZFunction_Callback Callback,
                                                      void* callbackArg ) {
    if( ( chanel_id < 3 ) && ( threshold >= 34 ) && ( threshold <= 78 ) ) {
        uint8_t dat[2] = { chanel_id, threshold };
        ZFunction func = ZFunction_Create( &fcSetThreshold, &dat, Callback, callbackArg );
        if( func == NULL )
            return ZNET_ERR_NOMEM;
        return ZNet_AddFuncToTX( func );
    }
    else
        return ZNET_ERR_INVAL;
}
