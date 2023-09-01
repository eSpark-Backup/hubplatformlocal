#include "typedefs.h"

#include "ZErrors.h"
#include "znet.h"
//

#include "znet_fc_private.h"

#include "znet_lib_fc.h"
#include "znet_lib_cc_codes.h"

#include "logger.h"
#include "ZPlatform.h"

/*============================================================================
**    Defines for ApplicationNodeInformation field deviceOptionMask
**--------------------------------------------------------------------------*/
#define APPLICATION_NODEINFO_NOT_LISTENING 0x00
#define APPLICATION_NODEINFO_LISTENING 0x01
#define APPLICATION_NODEINFO_OPTIONAL_FUNCTIONALITY 0x02
#define APPLICATION_FREQ_LISTENING_MODE_1000ms 0x10
#define APPLICATION_FREQ_LISTENING_MODE_250ms 0x20

//----------------------------------------------------------------------------
static void ICACHE_FLASH_ATTR InitSendNodeInformationBuffer( ZFunction this ) {
    CyclicBuffer const buffer = &this->_tx_frame;

    buffer->_size = 8;
    buffer->_begin = 0;
    buffer->_len = buffer->_end = this->_tx_frame_initial_len = 6;
    // uint8_t* data = buffer->_data;
    // data[ 0 ] = SOF;
    // data[ 1 ] = 0xFF; // len
    // data[ 2 ] = REQUEST;
    // data[ 3 ] = this->_fc->_id;
    buffer->_data[4] = NODE_BROADCAST | APPLICATION_NODEINFO_OPTIONAL_FUNCTIONALITY;
    buffer->_data[5] = TRANSMIT_OPTION_NO_ROUTE;

    // data[ 6 ] = 0xFF; // callback_id
    // data[ 7 ] = 0xFF; // crc
}

//----------------------------------------------------------------------------
static FuncResults_e ICACHE_FLASH_ATTR __SendNodeInformationCallback( ZFunction this ) {
    ZNetConst znet = this->_znet;
    const Buffer rxed_frame = &znet->_frame;
    uint8_t* dat = rxed_frame->_data;
    int size = rxed_frame->_len;

    if( size < 4 )
        return FuncResults_Done;

    switch( dat[3] ) {
    case TRANSMIT_COMPLETE_NO_ACK:
    case TRANSMIT_COMPLETE_FAIL:
        // can't deliver frame
        Log_Write( "Not delivered to recipient \r\n" );
        break;

    case TRANSMIT_COMPLETE_NOROUTE:
        // can't deliver frame due to no route
        Log_Write( "Not delivered to recipient due to no route \r\n" );
        break;

    case TRANSMIT_COMPLETE_OK:
        // command reception acknowledged by node
        Log_Write( "Delivered \r\n" );
        break;

    default:
        /// Log_Write( "SendNodeInformation Response with callback 0x%02x
        break;
    }

    return FuncResults_Done;
}

//----------------------------------------------------------------------------
static znet_error_t ICACHE_FLASH_ATTR __SendNodeInformationInit( ZFunction this, void* UNUSED( initArg ) ) {
    CyclicBuffer buffer = &this->_tx_frame;
    if( ( buffer->_data = (uint8_t*)Heap_Alloc( 8 ) ) == NULL )
        return ZNET_ERR_NOMEM;

    InitSendNodeInformationBuffer( this );
    return ZNET_ERR_OK;
}

//----------------------------------------------------------------------------
const ZFunctionClass_t fcSendNodeInformation = {
    0x12,  // _id
    "SendNodeInformation",
    __SendNodeInformationInit,      // _Init
    NULL,                           // _ProcessACK
    NULL,                           // _ProcessRESPONSE
    __SendNodeInformationCallback,  // _ProcessCALLBACK
    NULL                            // _IsCallbackFrameMine

};

//----------------------------------------------------------------------------
znet_error_t ICACHE_FLASH_ATTR znet_fc_send_node_information( bool startStop,
                                                              ZFunction_Callback Callback,
                                                              void* callbackArg ) {
    ZFunction func = ZFunction_Create( &fcSendNodeInformation, &startStop, Callback, callbackArg );
    if( func == NULL )
        return ZNET_ERR_NOMEM;
    return ZNet_AddFuncToTX( func );
}
