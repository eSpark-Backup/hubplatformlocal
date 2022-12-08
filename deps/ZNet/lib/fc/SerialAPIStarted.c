
#include "byte_ops.h"

#include "ZErrors.h"
#include "znet.h"
#include "ZPlatform.h"
#include "znet_lib_fc.h"
#include "znet_lib.h"
#include "logger.h"

extern void ICACHE_FLASH_ATTR StartSerialAPI( int len, uint8_t* buf );

//----------------------------------------------------------------------------
static FuncResults_e ICACHE_FLASH_ATTR __SerialAPIStartedCallback( ZFunction this ) {
    ZNet znet = this->_znet;
    Buffer rxed_frame = &znet->_frame;
    uint8_t* data = rxed_frame->_data;
    int size = rxed_frame->_len;

    if( size < 3 )
        return FuncResults_Done;
    StartSerialAPI( size, data );
    return FuncResults_Done;
}

//----------------------------------------------------------------------------
const ZFunctionClass_t fcSerialAPIStarted = {
    0x0A,  // _id
    "SerialAPIStarted",
    NULL,                        // _Init
    NULL,                        // _ProcessACK
    NULL,                        // _ProcessRESPONSE
    __SerialAPIStartedCallback,  // _ProcessCALLBACK
    NULL                         // _IsCallbackFrameMine
};
