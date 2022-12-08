
//////////////////////////////////////////////////////////////////////////////

#include "ZErrors.h"
#include "znet.h"

#include "ZPlatform.h"

#include "znet_lib.h"
#include "znet_lib_cc.h"

#include "logger.h"

#include "serial_port_impl.h"
#include "S2_wrap.h"
#include "ZW_transport_api.h"

//////////////////////////////////////////////////////////////////////////////

extern uint8_t learn_mode;
extern uint8_t znet_init_finished;
//////////////////////////////////////////////////////////////////////////////

//----------------------------------------------------------------------------
static FuncResults_e ICACHE_FLASH_ATTR __ApplicationCommandHandlerCallback( ZFunction this ) {
#define FC_MODE_OFFSET 0
#define FC_ID_OFFSET 1
#define FC_STATUS_OFFSET 2
#define FC_NODE_ID_OFFSET 3
#define FC_CC_LENGTH_OFFSET 4
#define FC_CC_ID_OFFSET 5
#define FC_CC_CMD_OFFSET 6
#define FC_CC_DATA_OFFSET 7

    Log_Write( "AppCommHandler\r\n" );

    if( znet_init_finished == 0 ) {
        return FuncResults_Done;
    }

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
        Log_Write( "len > 8" );
    return FuncResults_Done;
}

//////////////////////////////////////////////////////////////////////////////

//----------------------------------------------------------------------------
const ZFunctionClass_t fcApplicationCommandHandler = {
    0x04,                                 // _id
    "ApplicationCommandHandler",          // _name
    NULL,                                 // _Init
    NULL,                                 // _ProcessACK
    NULL,                                 // _ProcessRESPONSE
    __ApplicationCommandHandlerCallback,  // _ProcessCALLBACK
    // NULL,                                // _ProcessTimeout
    NULL  // _IsCallbackFrameMine
};
