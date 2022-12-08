
#include "byte_ops.h"

#include "ZErrors.h"
#include "znet.h"
//

//#include "znet_cc_private.h"
//

#include "ZPlatform.h"

#include "znet_lib_fc.h"

#include "znet_lib.h"

#include "logger.h"

#define UPDATE_STATE_SUC_ID 0x10
#define UPDATE_STATE_DELETE_DONE 0x20
#define UPDATE_STATE_ADD_DONE 0x40

#define UPDATE_STATE_NODE_INFO_RECEIVED 0x84
#define UPDATE_STATE_NODE_INFO_REQ_FAILED 0x81

#define UPDATE_STATE_NODE_INFO_SMARTSTART_HOMEID_RECEIVED 0x85
#define UPDATE_STATE_INCLUDED_NODE_INFO_RECEIVED 0x86

extern void ICACHE_FLASH_ATTR NodeAddCallback( int len, uint8_t* buf );
extern void ICACHE_FLASH_ATTR NodeDeleteCallback( int len, uint8_t* buf );
extern void ICACHE_FLASH_ATTR NodeUpdateCallback( int len, uint8_t* buf );
extern void ICACHE_FLASH_ATTR NodeUpdateSISCallback( int len, uint8_t* buf );
extern void ICACHE_FLASH_ATTR NodeRequestFailedCallback( int len, uint8_t* buf );
extern void ICACHE_FLASH_ATTR NWIHomeIDCallback( int len, uint8_t* buf );
extern void ICACHE_FLASH_ATTR IncludedNIFCallback( int len, uint8_t* buf );

//----------------------------------------------------------------------------
static FuncResults_e ICACHE_FLASH_ATTR __ApplicationNodeUpdateCallback( ZFunction this ) {
    ZNet znet = this->_znet;
    Buffer rxed_frame = &znet->_frame;
    uint8_t* data = rxed_frame->_data;
    int size = rxed_frame->_len;

    if( size < 3 )
        return FuncResults_Done;

    switch( data[2] ) {
    case UPDATE_STATE_NODE_INFO_RECEIVED: {
        if( size < 5 )
            return FuncResults_Done;
        if( data[4] < 3 )
            return FuncResults_Done;
        if( size < 5 + data[4] )
            return FuncResults_Done;

        NodeUpdateCallback( size, data );
        break;
    }

    case UPDATE_STATE_NODE_INFO_REQ_FAILED:
        NodeRequestFailedCallback( size, data );
        break;

    case UPDATE_STATE_ADD_DONE: {
        if( size < 5 )
            return FuncResults_Done;
        if( data[4] < 3 )
            return FuncResults_Done;
        if( size < 5 + data[4] )
            return FuncResults_Done;

        NodeAddCallback( size, data );
        break;
    }

    case UPDATE_STATE_DELETE_DONE:
        if( size < 5 )
            return FuncResults_Done;
        if( size < 5 + data[4] )
            return FuncResults_Done;

        NodeDeleteCallback( size, data );
        break;

    case UPDATE_STATE_SUC_ID:
        if( size < 5 )
            return FuncResults_Done;
        if( data[4] != 0 )
            return FuncResults_Done;

        NodeUpdateSISCallback( size, data );
        break;

    case UPDATE_STATE_NODE_INFO_SMARTSTART_HOMEID_RECEIVED:
        NWIHomeIDCallback( size, data );
        break;

    case UPDATE_STATE_INCLUDED_NODE_INFO_RECEIVED:
        IncludedNIFCallback( size, data );
        break;

    default:
        // znet_log(znet, Debug, "Unhandled ApplicationNodeUpdate frame:
        // %d\r\n", data[2]);
        break;
    }

    return FuncResults_Done;
}

//----------------------------------------------------------------------------
const ZFunctionClass_t fcApplicationNodeUpdate = {
    0x49,  // _id
    "ApplicationNodeUpdate",
    NULL,                             // _Init
    NULL,                             // _ProcessACK
    NULL,                             // _ProcessRESPONSE
    __ApplicationNodeUpdateCallback,  // _ProcessCALLBACK
    // NULL,                            // _ProcessTimeout
    NULL  // _IsCallbackFrameMine

};
