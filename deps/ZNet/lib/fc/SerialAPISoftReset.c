#include "typedefs.h"

#include "ZErrors.h"
#include "znet.h"
//

#include "znet_fc_private.h"
#include "ZPlatform.h"

#include "znet_lib_fc.h"
#include "znet_log.h"

//----------------------------------------------------------------------------
//{ 0x01, 0x03, 0x00, 0x08 };
static znet_error_t __SerialAPISoftResetInit( ZFunction this, void* UNUSED( initArg ) ) {
    CyclicBuffer buffer = &this->_tx_frame;

    if( ( buffer->_data = (uint8_t*)Heap_Alloc( 4 ) ) == NULL )
        return ZNET_ERR_NOMEM;

    buffer->_size = 4;
    buffer->_begin = 0;
    buffer->_len = buffer->_end = this->_tx_frame_initial_len = 4;

    return ZNET_ERR_OK;
}

static FuncResults_e ICACHE_FLASH_ATTR __SerialAPISoftResetCALLBACK( ZFunction this ) {
    ZNET_LOGI( "%s\n", __func__ );

    if( this->_Callback )
        ( *this->_Callback )( this, this->_callbackArg, FUNC_OK, NULL, 0 );

    return FuncResults_Done;
}

static FuncResults_e ICACHE_FLASH_ATTR __SerialAPISoftResetRESPONSE( ZFunction this ) {
    ZNET_LOGI( "%s\n", __func__ );

    return FuncResults_Done;
}

//----------------------------------------------------------------------------
const ZFunctionClass_t fcSerialAPISoftReset = {
    0x08  // _id
    ,
    "SerialAPISoftReset",
    __SerialAPISoftResetInit  // _Init
    ,
    NULL  // _ProcessACK
    ,
    __SerialAPISoftResetRESPONSE  // _ProcessRESPONSE
    ,
    __SerialAPISoftResetCALLBACK  // _ProcessCALLBACK
    //, NULL                    // _ProcessTimeout
    ,
    NULL  // _IsCallbackFrameMine
};

//----------------------------------------------------------------------------
znet_error_t ICACHE_FLASH_ATTR znet_fc_serial_api_soft_reset( ZFunction_Callback Callback, void* callbackArg ) {
    ZFunction func = ZFunction_Create( &fcSerialAPISoftReset, NULL, Callback, callbackArg );
    if( func == NULL )
        return ZNET_ERR_NOMEM;

    ZNET_LOGI( "%s\n", __func__ );
    return ZNet_AddFuncToTX( func );
}
