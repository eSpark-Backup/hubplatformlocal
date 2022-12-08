#include "typedefs.h"

#include "ZErrors.h"
#include "znet.h"
//

#include "znet_fc_private.h"
//#include "ZControllerData.h"

#include "znet_lib_fc.h"

#include "ZPlatform.h"

//----------------------------------------------------------------------------
static znet_error_t ICACHE_FLASH_ATTR __StoreHomeIdInit( ZFunction this, void* initArg ) {
    CyclicBuffer buffer = &this->_tx_frame;
    if( ( buffer->_data = (uint8_t*)Heap_Alloc( 11 ) ) == NULL )
        return ZNET_ERR_NOMEM;

    buffer->_size = 11;
    buffer->_begin = 0;
    buffer->_len = buffer->_end = this->_tx_frame_initial_len = 9;
    // uint8_t* data = buffer->_data;
    // data[ 0 ] = SOF;
    // data[ 1 ] = 0xFF; // len
    // data[ 2 ] = REQUEST;
    // data[ 3 ] = this->_fc->_id;
    memcpy( &buffer->_data[4], initArg, 5 );
    // data[ 9 ] = 0xFF; // callback_id
    // data[ 10 ] = 0xFF; // crc
    return ZNET_ERR_OK;
}

//----------------------------------------------------------------------------
const ZFunctionClass_t fcStoreHomeId = {
    0x84,  // _id
    "StoreHomeId",
    __StoreHomeIdInit,  // _Init
    NULL,               // _ProcessACK
    NULL,               // _ProcessRESPONSE
    NULL,               // _ProcessCALLBACK
    // NULL,                // _ProcessTimeout
    NULL  // _IsCallbackFrameMine
};

//----------------------------------------------------------------------------
znet_error_t ICACHE_FLASH_ATTR znet_fc_store_home_id( uint8_t* home_id,
                                                      uint8_t bNodeID,
                                                      ZFunction_Callback Callback,
                                                      void* callbackArg ) {
    uint8_t* dat = (uint8_t*)Heap_Alloc( 5 );
    if( dat == NULL )
        return ZNET_ERR_NOMEM;

    memcpy( &dat[0], home_id, 4 );
    dat[4] = bNodeID;

    ZFunction func = ZFunction_Create( &fcStoreHomeId, dat, Callback, callbackArg );
    if( func == NULL )
        return ZNET_ERR_NOMEM;

    Heap_Free( dat );
    return ZNet_AddFuncToTX( func );
}
