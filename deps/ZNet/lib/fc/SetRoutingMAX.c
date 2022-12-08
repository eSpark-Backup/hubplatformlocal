#include "typedefs.h"

#include "ZErrors.h"
#include "znet.h"

#include "ZPlatform.h"

#include "znet_fc_private.h"

#include "znet_lib_fc.h"
#include "logger.h"
//----------------------------------------------------------------------------
static FuncResults_e ICACHE_FLASH_ATTR __SetRoutingMAXRESPONSE( ZFunction this ) {
    ZNet znet = this->_znet;
    Buffer rxed_frame = &znet->_frame;
    uint8_t* data = rxed_frame->_data;
    int size = rxed_frame->_len;

    if( size < 3 ) {
        if( this->_Callback )
            ( *this->_Callback )( this, this->_callbackArg, FUNC_FAILURE_RESPONSE_SIZE, NULL, 0 );
        return FuncResults_Done;
    }

    Log_Write( "__SetRoutingMAXRESPONSE %d\r\n", data[2] );
    if( data[2] ) {
        if( this->_Callback )
            ( *this->_Callback )( this, this->_callbackArg, FUNC_OK, NULL, 0 );
    }
    else {
        if( this->_Callback )
            ( *this->_Callback )( this, this->_callbackArg, FUNC_FAILURE_GENERAL, NULL, 0 );
    }

    return FuncResults_Done;
}

//----------------------------------------------------------------------------
static znet_error_t ICACHE_FLASH_ATTR __SetRoutingMAXInit( ZFunction this, void* initArg ) {
    CyclicBuffer buffer = &this->_tx_frame;

    if( ( buffer->_data = (uint8_t*)Heap_Alloc( 7 ) ) == NULL )
        return ZNET_ERR_NOMEM;

    buffer->_size = 7;
    buffer->_begin = 0;
    buffer->_len = buffer->_end = this->_tx_frame_initial_len = 5;

    // data[ 0 ] = SOF;
    // data[ 1 ] = 0xFF; // len
    // data[ 2 ] = REQUEST;
    // data[ 3 ] = this->_fc->_id;
    buffer->_data[4] = initArg ? ( *(uint8_t*)initArg ) : 0;  // maxRoutesTries
    // data[ 5 ] = 0xFF; // crc

    return ZNET_ERR_OK;
}

//----------------------------------------------------------------------------
const ZFunctionClass_t fcSetRoutingMAX = {
    0xD4,                     // _id
    "SetRoutingMAX",          // _name
    __SetRoutingMAXInit,      // _Init
    NULL,                     // _ProcessACK
    __SetRoutingMAXRESPONSE,  // _ProcessRESPONSE
    NULL,                     // _ProcessCALLBACK
    // __SetDefaultCallback, // _ProcessCALLBACK
    // NULL,                 // _ProcessTimeout
    NULL  // _IsCallbackFrameMine
};

//----------------------------------------------------------------------------
znet_error_t ICACHE_FLASH_ATTR znet_fc_set_routing_max( uint8_t routingMax,
                                                        ZFunction_Callback Callback,
                                                        void* callbackArg ) {
    ZFunction func = ZFunction_Create( &fcSetRoutingMAX, &routingMax, Callback, callbackArg );
    if( func == NULL )
        return ZNET_ERR_NOMEM;
    return ZNet_AddFuncToTX( func );
}
