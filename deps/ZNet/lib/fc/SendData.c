#include "typedefs.h"

#include "ZErrors.h"
#include "znet.h"
#include "znet_main.h"
//
//

#include "ZPlatform.h"
#include "znet_fc_private.h"
//#include "znet_cc_private.h"

#include "znet_lib_fc.h"

#include "SendData.h"
#include "S2_wrap.h"
#include "Security_Scheme0.h"

#include "logger.h"

SendDataContext_t send_data_context = { .bundle = NULL, .commandIdx = 0 };

void SendDataSecurity( ZMessage message );

message_send_type_t message_sending = Message_Not_Sending;

//----------------------------------------------------------------------------
FuncResults_e ICACHE_FLASH_ATTR __SendDataResponse( ZFunction this ) {
    ZNetConst znet = this->_znet;
    const Buffer rxed_frame = &znet->_frame;
    uint8_t* data = rxed_frame->_data;
    int size = rxed_frame->_len;

    if( size < 3 ) {
        // if( this->_req_attempts-- > 0 ) return FuncResults_Restart;
        if( this->_Callback )
            ( *this->_Callback )( this, this->_callbackArg, FUNC_FAILURE_RESPONSE_SIZE, NULL, 0 );
        return FuncResults_Done;
    }

    if( data[2] == 1 ) {
        // znet_log(znet, Debug, "Delivered to Z-Wave stack\r\n");
        return FuncResults_Next;
    }
    else {
        if( this->_Callback )
            ( *this->_Callback )( this, this->_callbackArg, FUNC_FAILURE_RESPONSE_INVALID, NULL, 0 );
        return FuncResults_Done;
    }
}

//----------------------------------------------------------------------------
FuncResults_e ICACHE_FLASH_ATTR __SendDataCallback( ZFunction this ) {
    if( !this ) {
        return FuncResults_Done;
    }

    ZNet znet = this->_znet;
    Buffer rxed_frame = &znet->_frame;
    uint8_t* data = rxed_frame->_data;
    int size = rxed_frame->_len;

    SystemTime_t timeout = this->_timeout_delta - ( this->_timeout - SystemTime_Now() );
    Log_Write( "Sending time %llu\r\n", timeout / 1000000 );

    if( size < 4 ) {
        // return ( ( this->_req_attempts-- > 0 ) ? FuncResults_Restart :
        // FuncResults_Done );
        if( this->_Callback )
            ( *this->_Callback )( this, this->_callbackArg, FUNC_FAILURE_CALLBACK_SIZE, NULL, 0 );
        return FuncResults_Done;
    }

    switch( data[3] ) {
    case TRANSMIT_COMPLETE_NO_ACK:
        if( this->_tx_frame._data ) {
            if( this->_Callback ) {
                ( *this->_Callback )( this, this->_callbackArg, FUNC_FAILURE_NO_ACK, data + 2, size - 2 );
            }
        }
        break;

    case TRANSMIT_COMPLETE_FAIL:
        // can't deliver frame
        if( this->_tx_attempt_counter < FUNCTION_CLASS_REQ_ATTEMPTS_COUNT ) {
            Log_Write( "postpone sending, counter  %d\r\n", this->_tx_attempt_counter );
            ++this->_tx_attempt_counter;
            znet->_postpone_sending_time = SystemTime_Now() + FUNCTION_CLASS_TIMEOUT_500_MS;
            return FuncResults_Restart;
        }
        else {
            if( this->_Callback ) {
                ( *this->_Callback )( this, this->_callbackArg, FUNC_FAILURE_NETWORK_BUSY, data + 2, size - 2 );
            }
        }
        break;
    case TRANSMIT_COMPLETE_NOROUTE:
        // can't deliver frame due to no route
        if( this->_Callback ) {
            ( *this->_Callback )( this, this->_callbackArg, FUNC_FAILURE_GENERAL, data + 2, size - 2 );
        }
        break;

    case TRANSMIT_COMPLETE_OK:
        // command reception acknowledged by node
        if( this->_Callback ) {
            ( *this->_Callback )( this, this->_callbackArg, FUNC_OK, data + 2, size - 2 );
        }
        return FuncResults_Done;
        break;

    default:
        // znet_log(znet, Error, "SendData Response with callback 0x%02x
        break;
    }
    return FuncResults_Done;
}

//----------------------------------------------------------------------------
static znet_error_t ICACHE_FLASH_ATTR __SendDataInit( ZFunction this, void* initArg ) {
    if( initArg ) {
        SendDataInfo send_data_info = (SendDataInfo)initArg;
        this->_node_id = send_data_info->_node_id;
        CyclicBuffer buffer = &this->_tx_frame;
        uint8_t dataLen = send_data_info->_length;
        //             0        1              2               3            4
        //             5             6       size-3    size-2           size-1
        //             size
        uint8_t size = 1 /*SOF*/ + 1 /*frame_len*/ + 1 /*frame_type*/ + 1 /*func_id*/ + 1 /*node_id*/ + 1 /*data_len*/ +
                       dataLen + 1 /*mode*/ + 1 /*callback_id*/ + 1 /*CRC*/;
        if( ( buffer->_data = (uint8_t*)Heap_Alloc( size ) ) == NULL )
            return ZNET_ERR_NOMEM;

        buffer->_size = size;
        buffer->_begin = 0;
        buffer->_len = buffer->_end = this->_tx_frame_initial_len = size - 2;

        buffer->_data[4] = send_data_info->_node_id;
        buffer->_data[5] = dataLen;
        memcpy( &buffer->_data[6], send_data_info->_data, dataLen );
        buffer->_data[size - 3] = TRANSMIT_OPTION_ACK | TRANSMIT_OPTION_AUTO_ROUTE |
                                  TRANSMIT_OPTION_EXPLORE;  // | (( device == NULL ||
                                                            // device->_isListening ) ?
                                                            // TRANSMIT_OPTION_EXPLORE : 0); //mode;

        Heap_Free( send_data_info );
    }
    else {
    }

    return ZNET_ERR_OK;
}

//----------------------------------------------------------------------------
static znet_error_t ICACHE_FLASH_ATTR __SendDataBridgeInit( ZFunction this, void* initArg ) {
    if( initArg ) {
        SendDataInfo send_data_info = (SendDataInfo)initArg;
        this->_node_id = send_data_info->_node_id;
        CyclicBuffer buffer = &this->_tx_frame;
        uint8_t dataLen = send_data_info->_length;
        uint8_t size = 1 /*SOF*/ + 1 /*frame_len*/ + 1 /*frame_type*/ + 1 /*func_id*/ + 1 /*srcNodeID*/ +
                       1 /*node_id*/ + 1 /*data_len*/ + dataLen + 1 /*mode*/ + 4 /* pRoute*/ + 1 /*callback_id*/ +
                       1 /*CRC*/;
        if( ( buffer->_data = (uint8_t*)Heap_Alloc( size ) ) == NULL )
            return ZNET_ERR_NOMEM;
        memset( buffer->_data, 0, size );

        buffer->_size = size;
        buffer->_begin = 0;
        buffer->_len = buffer->_end = this->_tx_frame_initial_len = size - 2;

        buffer->_data[4] = MY_NODE_ID;
        buffer->_data[5] = send_data_info->_node_id;
        buffer->_data[6] = dataLen;
        memcpy( &buffer->_data[7], send_data_info->_data, dataLen );
        buffer->_data[size - 7] = TRANSMIT_OPTION_ACK | TRANSMIT_OPTION_AUTO_ROUTE | TRANSMIT_OPTION_EXPLORE;
        Heap_Free( send_data_info );
    }
    else {
    }

    return ZNET_ERR_OK;
}

//----------------------------------------------------------------------------
static bool ICACHE_FLASH_ATTR __SendData_IsCallbackFrameMine( ZFunction this ) {
    ZNetConst znet = this->_znet;
    const Buffer rxed_frame = &znet->_frame;
    int size = rxed_frame->_len;

    return ( size >= 4 );
}

//----------------------------------------------------------------------------
const ZFunctionClass_t fcSendData = {
    0x13,                           // _id
    "SendData",                     // _name
    __SendDataInit,                 // _Init
    NULL,                           // _ProcessACK
    __SendDataResponse,             // _ProcessRESPONSE
    __SendDataCallback,             // _ProcessCALLBACK
    __SendData_IsCallbackFrameMine  // _IsCallbackFrameMine
};

//----------------------------------------------------------------------------
const ZFunctionClass_t fcSendDataBridge = {
    0xA9,                           // _id
    "SendDataBridge",               // _name
    __SendDataBridgeInit,           // _Init
    NULL,                           // _ProcessACK
    __SendDataResponse,             // _ProcessRESPONSE
    __SendDataCallback,             // _ProcessCALLBACK
    __SendData_IsCallbackFrameMine  // _IsCallbackFrameMine
};

extern BOOL ZW_TransportService_SendData( ts_param_t* p,
                                          uint8_t* pData,
                                          uint16_t dataLength,
                                          ZFunction_Callback Callback,
                                          void* callbackArg );

//----------------------------------------------------------------------------
void ICACHE_FLASH_ATTR _SendDataCallbackSecurity( BYTE txStatus, void** user ) {
    message_sending = Message_Not_Sending;
    Log_Write( "Read security message from queue \r\n" );
    if( *user == NULL ) {
        Log_Write( "_SendDataCallbackSecurity message is empty" );
        return;
    }

    ZMessage message = ( ZMessage )( *user );
    /// TODO: check channel_id argument (need recovery ???)
    if( message->_callback ) {
        print_hex_buf( message->_message, message->_length );
        ZFuncFailures_e reason;
        switch( txStatus ) {
        case TRANSMIT_COMPLETE_OK:
            reason = FUNC_OK;
            break;
        case TRANSMIT_COMPLETE_NO_ACK:
            reason = FUNC_FAILURE_NO_ACK;
            break;
        default:
            reason = FUNC_FAILURE_GENERAL;
            break;
        }
        SendDataInfo send_data_info = Heap_Alloc( sizeof( SendDataInfo_t ) );
        if( send_data_info == NULL ) {
            Log_Write( "_SendDataCallbackSecurity not enough memory \r\n" );
            return;
        }

        *send_data_info = ( SendDataInfo_t ){
            ._node_id = message->_node_id, ._length = message->_length, ._data = message->_message
        };
        ZFunction func = ZFunction_Create( &fcSendData, send_data_info, message->_callback, message->_callbackArg );
        if( func == NULL ) {
            Log_Write( "_SendDataCallbackSecurity not enough memory \r\n" );
            return;
        }
        ( message->_callback )( func, message->_callbackArg, reason, NULL, 0 );
        ZFunction_Free( func );
    }
    ZNet_Remove_Security_Message( message );
    *user = NULL;
}

//----------------------------------------------------------------------------
void SendDataSecurity( ZMessage message ) {
    Log_Write( "SendDataSecurity  node_id = %d \r\n", message->_node_id );
    print_hex_buf( message->_message, message->_length );

    ts_param_t p;
    p.snode = MY_NODE_ID;
    p.dnode = message->_node_id;
    p.tx_flags = TRANSMIT_OPTION_ACK | TRANSMIT_OPTION_AUTO_ROUTE;

    znet_error_t err = ZNET_ERR_OK;

    switch( message->_encap ) {
    case Encapsulation_Sec_0:
        p.scheme = SECURITY_SCHEME_0;
        err = sec0_send_data( &p, message->_message, message->_length, _SendDataCallbackSecurity, message );
        break;
    case Encapsulation_Sec_UNAUTHENTICATED:
        p.scheme = SECURITY_SCHEME_2_UNAUTHENTICATED;
        err = sec2_send_data( &p, message->_message, message->_length, _SendDataCallbackSecurity, message );
        break;
    case Encapsulation_Sec_AUTHENTICATED:
        p.scheme = SECURITY_SCHEME_2_AUTHENTICATED;
        err = sec2_send_data( &p, message->_message, message->_length, _SendDataCallbackSecurity, message );
        break;
    case Encapsulation_Sec_2:
        p.scheme = SECURITY_SCHEME_2_ACCESS;
        err = sec2_send_data( &p, message->_message, message->_length, _SendDataCallbackSecurity, message );
        break;
    }
    if( ZNET_ERR_OK == err ) {
        message_sending = Message_Sending;
    }
    else {
        Log_Write( "Can`t send a message, busy. \r\n" );
    }
}

//----------------------------------------------------------------------------
static znet_error_t ICACHE_FLASH_ATTR znet_fc_send_data_impl( uint8_t node_id,
                                                              uint8_t channel_id,
                                                              uint8_t length,
                                                              const uint8_t* data,
                                                              ZFunction_Callback Callback,
                                                              void* callbackArg,
                                                              encap_type_t encap_type ) {
    // TODO Investigate, why this check was added
    if( !( data && length >= 2 ) && !( data && length == 1 && data[0] == COMMAND_CLASS_NO_OPERATION ) ) {
        return ZNET_ERR_INVAL;
    }

    if( send_data_context.bundle ) {
        BundledCommand_t* cmd = &send_data_context.bundle[send_data_context.commandIdx++];
        cmd->length = length;
        cmd->command = malloc( length );
        if( cmd->command == NULL ) {
            ZNET_LOGE( "znet_fc_send_data_impl: malloc failed\n" );
            return ZNET_ERR_NOMEM;
        }

        memcpy( (void*)cmd->command, data, length );
        return ZNET_ERR_OK;
    }

#if ZNET_SUPPORT_PENDING_COMMANDS_FOR_SLEEP_NODES

    uint8_t cc = data[0];
    uint8_t func = data[1];
    if( !ZNet_IsWakingNode( node_id ) && !( cc == COMMAND_CLASS_WAKE_UP && func == WAKE_UP_NO_MORE_INFORMATION ) &&
        !( cc == COMMAND_CLASS_SECURITY && func == SECURITY_NONCE_REPORT ) &&
        !( cc == COMMAND_CLASS_SECURITY_2 && func == SECURITY_2_NONCE_REPORT ) &&
        !( cc == COMMAND_CLASS_DEVICE_RESET_LOCALLY && func == DEVICE_RESET_LOCALLY_NOTIFICATION ) ) {
        ZNET_LOGD( "ZW_SendData queueing call, node_id=%02x, cc=%02x, cmd=%02x\n", node_id, cc, func );
        znet_error_t add_waiting_status = ZNet_AddWaitingCommand(
            ZW_LIB_CONTROLLER_BRIDGE == Z_WAVE_LIBRARY_TYPE ? fcSendDataBridge._id : fcSendData._id,
            node_id,
            channel_id,
            Callback,
            callbackArg,
            encap_type,
            data,
            length );
        return add_waiting_status == ZNET_ERR_OK ? ZNET_ERR_INPROGR : add_waiting_status;
    }
#endif

    SendDataInfo send_data_info = NULL;

    if( 0 != channel_id ) {
        return znet_cc_multichannel_cmd_encapsulate( node_id,
                                                     0,           // src channel
                                                     channel_id,  // dst channel
                                                     length,
                                                     data,
                                                     Callback,
                                                     callbackArg,
                                                     encap_type );
    }
    switch( encap_type ) {
    case Encapsulation_None:

        if( length >= META_DATA_MAX_DATA_SIZE ) {
            Log_Write( "Message is too long, using TransportService \r\n" );

            ts_param_t p;
            p.snode = MY_NODE_ID;
            p.dnode = node_id;
            p.scheme = SECURITY_SCHEME_0;
            p.tx_flags = TRANSMIT_OPTION_ACK | TRANSMIT_OPTION_AUTO_ROUTE;
            return ZW_TransportService_SendData( &p, (uint8_t*)data, length, Callback, callbackArg );
        }

        send_data_info = Heap_Alloc( sizeof( SendDataInfo_t ) );
        if( send_data_info == NULL ) {
            return ZNET_ERR_NOMEM;
        }

        *send_data_info = ( SendDataInfo_t ){ ._node_id = node_id, ._length = length, ._data = data };

        ZFunction func = NULL;
        uint32_t timeout;
        if( Z_WAVE_LIBRARY_TYPE == ZW_LIB_CONTROLLER_BRIDGE ) {
            func = ZFunction_Create( &fcSendDataBridge, send_data_info, Callback, callbackArg );
            timeout = FUNCTION_CLASS_TIMEOUT_60_S;
        }
        else {
            func = ZFunction_Create( &fcSendData, send_data_info, Callback, callbackArg );
            timeout = FUNCTION_CLASS_TIMEOUT_20_S;
        }

        if( func == NULL )
            return ZNET_ERR_NOMEM;
        return ZNet_AddFuncToTX_timeout( func, timeout );
        break;
    case Encapsulation_CRC16:
        return znet_cc_crc16_encap( node_id, length, data, Callback, callbackArg );
        break;
    case Encapsulation_Sec_0:
    case Encapsulation_Sec_UNAUTHENTICATED:
    case Encapsulation_Sec_AUTHENTICATED:
    case Encapsulation_Sec_2: {
        znet_error_t err = ZNET_ERR_FAULT;
        Log_Write( "Add security message to queue \r\n" );

        ZMessage message = ZMessage_Create( data, length, node_id, encap_type, Callback, callbackArg );
        if( message && ZNet_Add_Security_Message( message ) ) {
            err = ZNET_ERR_OK;
        }
        else {
            err = ZNET_ERR_NOMEM;
        }
        return err;
        break;
    }
    }
    return ZNET_ERR_NOMEM;
}

znet_fc_send_data_t znet_fc_send_data = znet_fc_send_data_impl;
