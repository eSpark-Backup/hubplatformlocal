#include "typedefs.h"
#include "byte_ops.h"
#include "heap.h"
#include "ZErrors.h"
#include "znet.h"
#include "znet_fc_private.h"
#include "znet_lib_fc.h"
#include "logger.h"
#include "ZPlatform.h"

//----------------------------------------------------------------------------
static FuncResults_e ICACHE_FLASH_ATTR __GetSerialAPICapabilitiesResponse( ZFunction this ) {
    Log_Write( "__GetSerialAPICapabilitiesResponse\r\n" );
    ZNet znet = this->_znet;
    Buffer rxed_frame = &znet->_frame;
    uint8_t* data = rxed_frame->_data;
    int size = rxed_frame->_len;

    if( size < 10 ) {
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
const ZFunctionClass_t fcGetSerialAPICapabilities = {
    0x07,                                // _id
    "GetSerialAPICapabilities",          //
    NULL,                                // _Init
    NULL,                                // _ProcessACK
    __GetSerialAPICapabilitiesResponse,  // _ProcessRESPONSE
    NULL,                                // _ProcessCALLBACK
    NULL                                 // _IsCallbackFrameMine
};
