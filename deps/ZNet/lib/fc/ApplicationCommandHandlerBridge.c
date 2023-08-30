
//////////////////////////////////////////////////////////////////////////////

#include "ZErrors.h"
#include "znet.h"

#include "ZPlatform.h"

#include "znet_lib.h"
#include "znet_lib_cc.h"

#include "logger.h"

#include "serial_port_impl.h"
#include "S2_wrap.h"
#include "znet_fc_private.h"
#include "ZW_transport_api.h"

//////////////////////////////////////////////////////////////////////////////

extern uint8_t learn_mode;

//////////////////////////////////////////////////////////////////////////////

//----------------------------------------------------------------------------
static FuncResults_e ICACHE_FLASH_ATTR __ApplicationCommandHandlerBridgeCallback( ZFunction this ) {
    const uint8_t FC_MODE_OFFSET = 0;
    const uint8_t FC_STATUS_OFFSET = 2;
    const uint8_t FC_NODE_ID_OFFSET = 4;
    const uint8_t FC_CC_LENGTH_OFFSET = 5;
    const uint8_t FC_CC_ID_OFFSET = 6;

    Log_Write( "AppCommHandlerBridge\r\n" );
    Buffer rxed_frame = &this->_znet->_frame;
    int len = rxed_frame->_len;
    if( len >= 7 ) {
        uint8_t* data = rxed_frame->_data;
        if( data[FC_MODE_OFFSET] == REQUEST &&
            ( ( data[FC_STATUS_OFFSET] & RECEIVE_STATUS_TYPE_MASK ) == RECEIVE_STATUS_TYPE_SINGLE ||
              ( data[FC_STATUS_OFFSET] & RECEIVE_STATUS_TYPE_MASK ) ==
                  RECEIVE_STATUS_TYPE_BROAD ) )  // TODO is it safe to process broadcasts the same method?
        {
            uint8_t node_id = data[FC_NODE_ID_OFFSET]; /*nodeId*/
            int cc_report_data_len = data[FC_CC_LENGTH_OFFSET];
            uint8_t* cc_report_data = &data[FC_CC_ID_OFFSET];
            uint8_t message_type = ZCommandClass_FindTypeByCCData( cc_report_data );
            if( MESSAGE_TYPE_TRANSPORT == message_type ) {
                CC_Report cc_report = ZCommandClass_FindCCReportByCCData( cc_report_data );
                if( cc_report )
                    ( *cc_report )( this, node_id, cc_report_data_len, cc_report_data );
            }
            else {
                ZNet_ApplicationCommandHandler( cc_report_data, cc_report_data_len, node_id, Encapsulation_None, 0 );
            }
        }
        else
            Log_Write( "FC_MODE_OFFSET=%d, FC_STATUS_OFFSET=%d\r", data[FC_MODE_OFFSET], data[FC_STATUS_OFFSET] );
    }
    else
        Log_Write( "len > 8. len = %s", len );
    return FuncResults_Done;
}

//////////////////////////////////////////////////////////////////////////////

//----------------------------------------------------------------------------
const ZFunctionClass_t fcApplicationCommandHandlerBridge = {
    0xA8,                                       // _id
    "ApplicationCommandHandlerBridge",          // _name
    NULL,                                       // _Init
    NULL,                                       // _ProcessACK
    NULL,                                       // _ProcessRESPONSE
    __ApplicationCommandHandlerBridgeCallback,  // _ProcessCALLBACK
    // NULL,                                // _ProcessTimeout
    NULL  // _IsCallbackFrameMine
};
