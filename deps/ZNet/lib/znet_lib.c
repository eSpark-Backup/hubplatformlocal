#include "heap.h"

#include "byte_ops.h"

#include "znet_lib.h"
#include "znet_main.h"
//
//#include "ZNonces.h"
//#include "ZWorkerThread.h"
#include "znet_private.h"  //added by PM
#include "znet.h"
#include "znet_fc_private.h"
//#include "ZControllerData.h"

#include "ZPlatform.h"
#include "znet_lib_fc.h"
#include "znet_lib_cc.h"

//#include "flash_device_classes.h"

#include "ZNetActivities.h"

#include "system_time.h"

#include "logger.h"
#include "S2_wrap.h"

extern ZNet_t znet_instance;
//////////////////////////////////////////////////////////////////////////////

/* clang-format off */
const int Math_pow10[ MATH_POW10_LEN ] =
{
  /* 00 */ 1,
  /* 01 */ 10,
  /* 02 */ 100,
  /* 03 */ 1000,
  /* 04 */ 10000,
  /* 05 */ 100000,
  /* 06 */ 1000000,
  /* 07 */ 10000000,
  /* 08 */ 100000000,
  /* 09 */ 1000000000
};
/* clang-format on */

//////////////////////////////////////////////////////////////////////////////

typedef struct _ZDeviceCallbackListItem {
    ZDeviceCallback _callback;
    void* _arg;
    ZWDeviceChangeType _mask;

} ZDeviceCallbackListItem_t;
typedef ZDeviceCallbackListItem_t* ZDeviceCallbackListItem;

//----------------------------------------------------------------------------
static ZDeviceCallbackListItem ICACHE_FLASH_ATTR ZDeviceCallbackListItem_Create( ZWDeviceChangeType mask,
                                                                                 ZDeviceCallback callback,
                                                                                 void* arg ) {
    ZDeviceCallbackListItem item;

    if( callback == NULL )
        return NULL;

    item = (ZDeviceCallbackListItem)Heap_Alloc( sizeof( ZDeviceCallbackListItem_t ) );
    if( item ) {
        item->_callback = callback;
        item->_arg = arg;
        item->_mask = mask;
    }
    return item;
}
//----------------------------------------------------------------------------
static void ICACHE_FLASH_ATTR ZDeviceCallbackListItem_Free( void* object ) {
    ZDeviceCallbackListItem this = (ZDeviceCallbackListItem)object;
    Heap_Free( this );
    // return TRUE;
}

//----------------------------------------------------------------------------

static bool ICACHE_FLASH_ATTR ZDeviceCallbackListItem_CheckCB( const void* object, const void* arg ) {
    return ( ( (ZDeviceCallbackListItem)object )->_callback == (ZDeviceCallback)arg /*callback*/ );
}

ZWError ICACHE_FLASH_ATTR ZNet_AddCallback( ZWDeviceChangeType mask, ZDeviceCallback callback, void* arg ) {
    ZDeviceCallbackListItem item;

    if( callback == NULL )
        return InvalidArg;

    if( List_Find( &znet_instance._device_callbacks, &ZDeviceCallbackListItem_CheckCB, (void*)callback ) )
        return InvalidArg;
    if( ( item = ZDeviceCallbackListItem_Create( mask, callback, arg ) ) == NULL )
        return BadAllocation;
    if( List_Add( &znet_instance._device_callbacks, item ) == NULL ) {
        ZDeviceCallbackListItem_Free( item );
        return BadAllocation;
    }

    return NoError;
}
//----------------------------------------------------------------------------
ZWError ICACHE_FLASH_ATTR ZNet_RemoveCallback( ZDeviceCallback callback ) {
    if( callback == NULL )
        return InvalidArg;

    List_Entry entry = List_Find( &znet_instance._device_callbacks, &ZDeviceCallbackListItem_CheckCB, (void*)callback );
    if( entry == NULL )
        return InvalidArg;
    if( !List_Remove( &znet_instance._device_callbacks, entry, &ZDeviceCallbackListItem_Free ) )
        return BadAllocation;

    return NoError;
}

//----------------------------------------------------------------------------

typedef struct {
    ZDeviceCallback _callback;
    void* _arg;
} ZDeviceCallbackListItem_CheckCBAndArgArg_t;
typedef ZDeviceCallbackListItem_CheckCBAndArgArg_t* ZDeviceCallbackListItem_CheckCBAndArgArg;

static bool ICACHE_FLASH_ATTR ZDeviceCallbackListItem_CheckCBAndArg( const void* object, const void* arg ) {
    ZDeviceCallbackListItem item = (ZDeviceCallbackListItem)object;
    ZDeviceCallbackListItem_CheckCBAndArgArg args = (ZDeviceCallbackListItem_CheckCBAndArgArg)arg;
    return ( item->_callback == args->_callback && item->_arg == args->_arg );
}

ZWError ICACHE_FLASH_ATTR ZNet_AddCallbackEx( ZWDeviceChangeType mask, ZDeviceCallback callback, void* arg ) {
    ZDeviceCallbackListItem item;
    ZDeviceCallbackListItem_CheckCBAndArgArg_t CBarg;

    if( callback == NULL )
        return InvalidArg;

    CBarg._callback = callback;
    CBarg._arg = arg;
    if( List_Find( &znet_instance._device_callbacks, &ZDeviceCallbackListItem_CheckCBAndArg, &CBarg ) )
        return InvalidArg;
    if( ( item = ZDeviceCallbackListItem_Create( mask, callback, arg ) ) == NULL )
        return BadAllocation;
    if( List_Add( &znet_instance._device_callbacks, item ) == NULL ) {
        ZDeviceCallbackListItem_Free( item );
        return BadAllocation;
    }

    return NoError;
}
//----------------------------------------------------------------------------
ZWError ICACHE_FLASH_ATTR ZNet_RemoveCallbackEx( ZDeviceCallback callback, void* arg ) {
    // ZDeviceCallbackListItem item;
    ZDeviceCallbackListItem_CheckCBAndArgArg_t CBarg;

    if( callback == NULL )
        return InvalidArg;

    CBarg._callback = callback;
    CBarg._arg = arg;
    List_Entry entry = List_Find( &znet_instance._device_callbacks, &ZDeviceCallbackListItem_CheckCBAndArg, &CBarg );
    if( entry == NULL )
        return InvalidArg;
    if( !List_Remove( &znet_instance._device_callbacks, entry, &ZDeviceCallbackListItem_Free ) )
        return BadAllocation;

    return NoError;
}

//----------------------------------------------------------------------------

typedef struct {
    ZNet _znet;
    ZWDeviceChangeType _type;
    uint8_t _node_id;
    uint8_t _instance_id;
    uint8_t _command_id;
} ZDeviceCallback_CallArg_t;
typedef ZDeviceCallback_CallArg_t* ZDeviceCallback_CallArg;

static void ICACHE_FLASH_ATTR ZDeviceCallback_Call( List_Entry entry, void* itemFuncArg ) {
    ZDeviceCallbackListItem item = (ZDeviceCallbackListItem)entry->_object;
    ZDeviceCallback_CallArg arg = (ZDeviceCallback_CallArg)itemFuncArg;
    if( ( item->_mask & arg->_type ) == arg->_type )
        ( *item->_callback )( arg->_type, arg->_node_id, arg->_instance_id, arg->_command_id, item->_arg );
}

void ICACHE_FLASH_ATTR ZNet_CallDeviceCB( ZWDeviceChangeType type,
                                          uint8_t node_id,
                                          uint8_t instance_id,
                                          uint8_t command_id ) {
    ZDeviceCallback_CallArg_t arg;
    arg._znet = &znet_instance;
    arg._type = type;
    arg._node_id = node_id;
    arg._instance_id = instance_id;
    arg._command_id = command_id;
    List_ForEach( &znet_instance._device_callbacks, &ZDeviceCallback_Call, &arg );
}
//////////////////////////////////////////////////////////////////////////////

//----------------------------------------------------------------------------
ZTimer ICACHE_FLASH_ATTR ZTimer_Create( SystemTime_t timeout, ZTimer_Callback const Callback, void* callbackArg ) {
    ZTimer timer = Heap_Alloc( sizeof( ZTimer_t ) );
    if( timer ) {
        *( (ZNet*)&timer->_znet ) = &znet_instance;
        *( (ZTimer_Callback*)&timer->_Callback ) = Callback;
        timer->_callbackArg = callbackArg;
        timer->_timeout = SystemTime_Now() + timeout;
    }
    else if( Callback ) {
        ( *Callback )( NULL, callbackArg, !0 );
    }
    return timer;
}
//----------------------------------------------------------------------------
void ICACHE_FLASH_ATTR ZTimer_Free( void* timer ) {
    if( timer == NULL )
        return;

    ZTimer this = (ZTimer)timer;
    if( this->_Callback )
        ( *this->_Callback )( this, this->_callbackArg, !0 );
    Heap_Free( this );
}
//////////////////////////////////////////////////////////////////////////////

//----------------------------------------------------------------------------
#if ZNET_SUPPORT_PENDING_COMMANDS_FOR_SLEEP_NODES
znet_error_t ICACHE_FLASH_ATTR ZWaitingCommand_Call( ZWaitingCommand waiting_command ) {
    if( !waiting_command )
        return ZNET_ERR_INVAL;

    if( fcSendData._id == waiting_command->_func_class_id || fcSendDataBridge._id == waiting_command->_func_class_id ) {
        return znet_fc_send_data( waiting_command->_node_id,
                                  waiting_command->_channel_id,
                                  waiting_command->_data_size,
                                  waiting_command->_data,
                                  waiting_command->_cb,
                                  waiting_command->_cb_arg,
                                  waiting_command->_encap );
    }
    else if( fcRequestNodeNeighborUpdate._id == waiting_command->_func_class_id ) {
        return znet_fc_request_node_neighbor_update(
            waiting_command->_node_id, waiting_command->_cb, waiting_command->_cb_arg );
    }
    return ZNET_ERR_FAULT;
}
#endif

//----------------------------------------------------------------------------
#if ZNET_SUPPORT_PENDING_COMMANDS_FOR_SLEEP_NODES
ZWaitingCommand ICACHE_FLASH_ATTR ZWaitingCommand_Create( uint8_t func_class_id,
                                                          uint8_t node_id,
                                                          uint8_t channel_id,
                                                          ZFunction_Callback cb,
                                                          void* cb_arg,
                                                          encap_type_t encap,
                                                          const void* data,
                                                          size_t data_size ) {
    ZWaitingCommand waiting_command = Heap_Alloc( sizeof( ZWaitingCommand_t ) + data_size );
    if( waiting_command ) {
        waiting_command->_znet = &znet_instance;
        waiting_command->_func_class_id = func_class_id;
        waiting_command->_node_id = node_id;
        waiting_command->_channel_id = channel_id;
        waiting_command->_cb = cb;
        waiting_command->_cb_arg = cb_arg;
        waiting_command->_encap = encap;
        waiting_command->_data_size = data_size;
        if( data_size )
            memcpy( waiting_command->_data, data, data_size );
    }
    return waiting_command;
}
#endif

//----------------------------------------------------------------------------
#if ZNET_SUPPORT_PENDING_COMMANDS_FOR_SLEEP_NODES
void ICACHE_FLASH_ATTR ZWaitingCommand_Free( void* waiting_command ) {
    if( waiting_command == NULL )
        return;

    Heap_Free( waiting_command );
}
#endif
//////////////////////////////////////////////////////////////////////////////

//----------------------------------------------------------------------------
ZWError ICACHE_FLASH_ATTR ZNet_Init( SerialPort serialPort ) {
    if( serialPort == NULL )
        return InvalidArg;

    memset( &znet_instance, 0, sizeof znet_instance );

    znet_instance._serialPort = serialPort;
    znet_instance._serial_api_ininitalized = FALSE;

    znet_instance._rxedBuffer = ( CyclicBuffer_t ){ ._data = znet_instance._rxedBufferData,
                                                    ._size = sizeof znet_instance._rxedBufferData,
                                                    ._len = 0,
                                                    ._begin = 0,
                                                    ._end = 0 };
    znet_instance._frame =
        ( Buffer_t ){ ._data = znet_instance._frameData, ._size = sizeof znet_instance._frameData, ._len = 0 };
    znet_instance._frameLenToRX = 0;
    znet_instance._postpone_sending_time = SystemTime_Now();

    ZWError result = BadAllocation;

    // if( ! ZControllerData_Init( &this->_controllerData, this ) ) goto
    // _E_controllerData_;

    // if( ! List_Init( &this->_queue            ) ) goto _E_queue_;
    // if( ! List_Init( &this->_devices          ) ) goto _E_devices_;
    // if( ! List_Init( &this->_nonces           ) ) goto _E_nonces_;

    --znet_instance._timeout;  // now this->_timeout == 0xFFF...F
    if( !List_Init( &znet_instance._funcs_to_TX ) )
        goto _E_funcs_to_TX_;
    if( !List_Init( &znet_instance._funcs_to_ACK ) )
        goto _E_funcs_to_ACK_;
    if( !List_Init( &znet_instance._funcs_to_RESPONSE ) )
        goto _E_funcs_to_RESPONSE_;
    if( !List_Init( &znet_instance._funcs_to_CALLBACK ) )
        goto _E_funcs_to_CALLBACK_;

    if( !List_Init( &znet_instance._timers ) )
        goto _E_timers_;

    if( !List_Init( &znet_instance._security_message ) )
        goto _E_security_message_;
#if ZNET_SUPPORT_PENDING_COMMANDS_FOR_SLEEP_NODES
    if( !List_Init( &znet_instance._waiting_commands ) )
        goto _E_waiting_commands_;
#endif
    if( !List_Init( &znet_instance._device_callbacks ) )
        goto _E_device_callbacks_;

    // TrackedTimes_Init( &this->_devices_times );

    ZFunctionClass_FillBySupportedFC( znet_instance._supported_function_classes );

    // timeouts
    // this->_timeout_channel        = 0.1f; // depends on port type
    // this->_timeout_ack            = 0.1f;
    // this->_timeout_response       = 20;
    // this->_timeout_callback_first = 20;
    // this->_timeout_callback_more  = 20;
    // this->_timeout_done           = 20;

    // this->_max_resend_count = 3;

    result = ThreadingError;
    return NoError;

    ZNetRXActivity_init();
    return NoError;

//_E_txActivity_:
//    Activity_DeInit( &this->_rxActivity );
//_E_rxActivity_:
//    List_DeInit( &this->_device_callbacks, &ZDeviceCallbackListItem_Free );
_E_device_callbacks_:
#if ZNET_SUPPORT_PENDING_COMMANDS_FOR_SLEEP_NODES
    List_DeInit( &znet_instance._waiting_commands, &ZWaitingCommand_Free );
_E_waiting_commands_:
#endif
    List_DeInit( &znet_instance._security_message, &ZMessage_Free );
_E_security_message_:
    List_DeInit( &znet_instance._timers, &ZTimer_Free );
_E_timers_:
    List_DeInit( &znet_instance._funcs_to_CALLBACK, &ZFunction_Free );
_E_funcs_to_CALLBACK_:
    List_DeInit( &znet_instance._funcs_to_RESPONSE, &ZFunction_Free );
_E_funcs_to_RESPONSE_:
    List_DeInit( &znet_instance._funcs_to_ACK, &ZFunction_Free );
_E_funcs_to_ACK_:
    List_DeInit( &znet_instance._funcs_to_TX, &ZFunction_Free );
_E_funcs_to_TX_:

    return result;
}

//----------------------------------------------------------------------------
ZWError ICACHE_FLASH_ATTR ZNet_Start( ZTerminationCallback termination_callback ) {
    if( znet_instance._is_running )
        return NoError;

    znet_instance._is_running = TRUE;

    return NoError;

    //_E_rxActivity_Start_:
    //_E_rxActivity_Stop_:
    //    return ThreadingError;
}
//----------------------------------------------------------------------------
ZWError ICACHE_FLASH_ATTR ZNet_Stop( ZNet this ) {
    if( this == NULL )
        return InvalidArg;

    if( !this->_is_running )
        return NoError;

    this->_is_running = FALSE;

    return NoError;
}

//----------------------------------------------------------------------------
bool ICACHE_FLASH_ATTR ZNet_DeInit( ZNet this ) {
    ZNet_Stop( this );  // just in case it is still running
    // save data before terminating
    /// zddx_save_to_xml( this, 0, 0 );

    List_DeInit( &this->_device_callbacks, &ZDeviceCallbackListItem_Free );

#if ZNET_SUPPORT_PENDING_COMMANDS_FOR_SLEEP_NODES
    List_DeInit( &this->_waiting_commands, &ZWaitingCommand_Free );
#endif
    List_DeInit( &this->_security_message, &ZMessage_Free );
    List_DeInit( &this->_timers, &ZTimer_Free );

    List_DeInit( &this->_funcs_to_CALLBACK, &ZFunction_Free );
    List_DeInit( &this->_funcs_to_RESPONSE, &ZFunction_Free );
    List_DeInit( &this->_funcs_to_ACK, &ZFunction_Free );
    List_DeInit( &this->_funcs_to_TX, &ZFunction_Free );
    sec2_deinit();

    if( this->_func_to_TX )
        ZFunction_Free( this->_func_to_TX );

    return TRUE;
}

//----------------------------------------------------------------------------
static void ICACHE_FLASH_ATTR FindMinTimeout( List_Entry entry, void* arg ) {
    ZFunction func = (ZFunction)entry->_object;
    if( *(SystemTime)arg > func->_timeout )
        *(SystemTime)arg = func->_timeout;
}

void ICACHE_FLASH_ATTR ZNet_RefreshTimeout() {
    SystemTime_t timeout;
    timeout ^= timeout;
    --timeout;
    List_ForEach( &znet_instance._funcs_to_ACK, &FindMinTimeout, &timeout );
    List_ForEach( &znet_instance._funcs_to_RESPONSE, &FindMinTimeout, &timeout );
    List_ForEach( &znet_instance._funcs_to_CALLBACK, &FindMinTimeout, &timeout );
    znet_instance._timeout = timeout;
}

//-----------------------------------------------------------------------------------
uint8_t ICACHE_FLASH_ATTR CalcFrameChecksum( uint8_t* frame ) {
    uint8_t len = *( ++frame );
    uint8_t checksum = ( uint8_t )( 0xFF ^ len );
    uint8_t* frame_end = frame++ + len;
    while( frame < frame_end )
        checksum ^= *frame++;
    return checksum;
}
//----------------------------------------------------------------------------
static void ICACHE_FLASH_ATTR TuneFuncTXFrame( ZFunction func ) {
    CyclicBuffer buffer = &func->_tx_frame;
    uint8_t* data = buffer->_data;
    int data_len = func->_tx_frame_initial_len;

    data[0] = 0x01;  // SOF;
    // data[ 1 ] ----> see below !!!
    data[2] = REQUEST;
    data[3] = func->_fc->_id;

    if( func->_ProcessCALLBACK ) {
        ZNet znet = func->_znet;
        if( znet->_callback_id == 255 || znet->_callback_id == 0 )
            znet->_callback_id = 1;
        if( func->_fc->_id == fcAddNodeToNetwork._id && data[4] == ADD_NODE_HOME_ID ) {
            data[5] = znet->_callback_id++;
            data_len++;
        }
        else {
            data[data_len++] = znet->_callback_id++;
        }
        if( data[3] == 0x51 )
            data[5] = znet->_callback_id - 1;
    }

    data[1] = ( uint8_t )( data_len - 1 );
    data[data_len++] = CalcFrameChecksum( data );
    buffer->_end = buffer->_len = data_len;
}

//----------------------------------------------------------------------------
static bool ICACHE_FLASH_ATTR CheckFuncTimeoutIsGT( const void* object, const void* arg ) {
    return ( ( (ZFunction)object )->_timeout > *(SystemTime)arg );
}
//----------------------------------------------------------------------------
static bool ICACHE_FLASH_ATTR CheckTimerTimeoutIsGT( const void* object, const void* arg ) {
    return ( ( (ZTimer)object )->_timeout > *(SystemTime)arg );
}
//----------------------------------------------------------------------------
static znet_error_t ICACHE_FLASH_ATTR FuncList_Add( List funcs,
                                                    ZFunction func,
                                                    SystemTime_t timeout_delta,
                                                    ZFuncFailures_e failure_reason ) {
    if( timeout_delta ) {
        func->_timeout = SystemTime_Now() + timeout_delta;
        func->_timeout_delta = timeout_delta;
    }
    List_Entry entry = List_Find( funcs, &CheckFuncTimeoutIsGT, &func->_timeout );
    if( ( entry ? List_Insert( funcs, func, entry, List_Insert_Positions_Before ) : List_Add( funcs, func ) ) ==
        NULL ) {
        if( func->_Callback )
            ( *func->_Callback )( func, func->_callbackArg, failure_reason, NULL, 0 );
        ZFunction_Free( func );
        return ZNET_ERR_NOMEM;
    }
    return ZNET_ERR_OK;
}
//----------------------------------------------------------------------------
znet_error_t ICACHE_FLASH_ATTR ZNet_AddFuncToTX( ZFunction func ) {
    TuneFuncTXFrame( func );
    return FuncList_Add(
        &znet_instance._funcs_to_TX, func, SystemTime_ms( FUNCTION_CLASS_TIMEOUT_20_S ), FUNC_FAILURE_ADD_TO_TX );
}
//----------------------------------------------------------------------------
znet_error_t ICACHE_FLASH_ATTR ZNet_AddFuncToTX_timeout( ZFunction func, uint32_t timeout ) {
    TuneFuncTXFrame( func );
    // return FuncList_Add( &this->_funcs_to_TX, func, 0, FUNC_FAILURE_ADD_TO_TX
    // );
    return FuncList_Add( &znet_instance._funcs_to_TX, func, SystemTime_ms( timeout ), FUNC_FAILURE_ADD_TO_TX );
}

//----------------------------------------------------------------------------
void ICACHE_FLASH_ATTR ZNet_RemoveAllFuncToTxForNode( const uint8_t node_id ) {
    bool res;
    List_Entry current_entry;
    List_Entry check_entry;
    current_entry = List_First( &znet_instance._funcs_to_TX );
    uint16_t removed = 0, left = 0;
    while( current_entry ) {
        check_entry = current_entry;
        res = CheckFuncToNodeID( check_entry->_object, &node_id );
        current_entry = current_entry->_next;
        if( res ) {
            List_Remove( &znet_instance._funcs_to_TX, check_entry, ZFunction_Free );
            ++removed;
        }
        else {
            ++left;
        }
    }
}
//----------------------------------------------------------------------------
void ICACHE_FLASH_ATTR ZNet_AddFuncToACK( ZFunction func ) {
    (void)FuncList_Add( &znet_instance._funcs_to_ACK, func, 0, FUNC_FAILURE_ADD_TO_ACK );
}
//----------------------------------------------------------------------------
void ICACHE_FLASH_ATTR ZNet_AddFuncToRESPONSE( ZFunction func ) {
    (void)FuncList_Add( &znet_instance._funcs_to_RESPONSE, func, 0, FUNC_FAILURE_ADD_TO_RESPONSE );
}
//----------------------------------------------------------------------------
void ICACHE_FLASH_ATTR ZNet_AddFuncToCALLBACK( ZFunction func ) {
    (void)FuncList_Add( &znet_instance._funcs_to_CALLBACK, func, 0, FUNC_FAILURE_ADD_TO_CALLBACK );
}
//----------------------------------------------------------------------------
void ICACHE_FLASH_ATTR ZNet_ProcessTimers() {
    for( ;; ) {
        List_Entry entry = List_First( &znet_instance._timers );
        if( !entry ) {
            break;
        }
        ZTimer timer = (ZTimer)entry->_object;
        SystemTime_t now = SystemTime_Now();
        if( now < timer->_timeout ) {
            break;
        }
        if( List_Remove( &znet_instance._timers, entry, NULL ) ) {
            if( timer->_Callback )
                ( *timer->_Callback )( timer, timer->_callbackArg, 0 );  // timeout
            ZTimer_Free( timer );
        }
    }
}
//----------------------------------------------------------------------------
bool ICACHE_FLASH_ATTR ZNet_AddTimer( ZTimer timer ) {
    List timers = &znet_instance._timers;
    SystemTime_t now = SystemTime_Now();
    if( timer->_timeout < now ) {
        List_Entry entry = List_Last( timers );
        if( ( entry ? List_Insert( timers, timer, entry, List_Insert_Positions_After ) : List_Add( timers, timer ) ) ==
            NULL ) {
            return FALSE;
        }
    }
    else {
        List_Entry entry = List_Find( timers, &CheckTimerTimeoutIsGT, &timer->_timeout );
        if( ( entry ? List_Insert( timers, timer, entry, List_Insert_Positions_Before ) : List_Add( timers, timer ) ) ==
            NULL ) {
            return FALSE;
        }
    }
    return TRUE;
}
//----------------------------------------------------------------------------
bool ICACHE_FLASH_ATTR ZNet_RemoveTimer( ZTimer timer ) {
    if( timer ) {
        List_Entry entry = List_FindByObject( &znet_instance._timers, timer );
        if( entry ) {
            return List_Remove( &znet_instance._timers, entry, NULL );
        }
    }

    return false;
}
//----------------------------------------------------------------------------
static void ICACHE_FLASH_ATTR ZNet_MoveFuncFromRESPONSEToNextState( ZFunction func ) {
    if( func->_ProcessCALLBACK )
        ZNet_AddFuncToCALLBACK( func );
    else
        ZFunction_Free( func );
}
//----------------------------------------------------------------------------
static void ICACHE_FLASH_ATTR ZNet_MoveFuncFromACKToNextState( ZFunction func ) {
    if( func->_ProcessRESPONSE )
        ZNet_AddFuncToRESPONSE( func );
    else
        ZNet_MoveFuncFromRESPONSEToNextState( func );
}

//----------------------------------------------------------------------------
void ICACHE_FLASH_ATTR ZNet_ProcessACK() {
    List_Entry entry = List_First( &znet_instance._funcs_to_ACK );
    if( entry ) {
        ZFunction func = (ZFunction)entry->_object;

        List_Remove( &znet_instance._funcs_to_ACK, entry, NULL );

        ZFunctionProcessACK ProcessACK = func->_fc->_ProcessACK;

        func->_timeout = SystemTime_Now() + func->_timeout_delta;

        if( ProcessACK == NULL )
            ZNet_MoveFuncFromACKToNextState( func );
        else
            switch( ( *ProcessACK )( func ) ) {
            case FuncResults_Done:
                ZFunction_Free( func );
                break;
            case FuncResults_Restart:
                ZNet_AddFuncToTX( func );
                break;
            case FuncResults_Repeat:
                ZNet_AddFuncToACK( func );
                break;
            case FuncResults_Next:
                ZNet_MoveFuncFromACKToNextState( func );
                break;
            default:
                break;
            }
    }

    ZNet_RefreshTimeout();
}

//----------------------------------------------------------------------------
void ICACHE_FLASH_ATTR ZNet_ProcessNAKandCAN() {
    List_Entry entry = List_First( &znet_instance._funcs_to_ACK );
    if( entry ) {
        ZFunction func = (ZFunction)entry->_object;

        List_Remove( &znet_instance._funcs_to_ACK, entry, NULL );

        ZNet_AddFuncToTX( func );
    }

    ZNet_RefreshTimeout();
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
static bool ICACHE_FLASH_ATTR CheckFuncToRESPONSE( const void* object, const void* arg ) {
    ZFunction func = (ZFunction)object;
    Buffer rx_frame = (Buffer)arg;
    return ( func->_fc->_id == rx_frame->_data[1] /*fc_id*/ );
}

//----------------------------------------------------------------------------
static void ICACHE_FLASH_ATTR ZNet_ProcessRESPONSE() {
    Buffer rxed_frame = &znet_instance._frame;
    List_Entry entry = List_Find( &znet_instance._funcs_to_RESPONSE, &CheckFuncToRESPONSE, rxed_frame );
    if( entry == NULL )
        return;

    ZFunction func = (ZFunction)entry->_object;

    List_Remove( &znet_instance._funcs_to_RESPONSE, entry, NULL );

    ZFunctionProcessRESPONSE ProcessRESPONSE = func->_ProcessRESPONSE;
    if( ProcessRESPONSE == NULL )
        ZNet_MoveFuncFromRESPONSEToNextState( func );
    else
        switch( ( *ProcessRESPONSE )( func ) ) {
        case FuncResults_Done:
            ZFunction_Free( func );
            break;
        case FuncResults_Restart:
            ZNet_AddFuncToTX( func );
            break;
        case FuncResults_Repeat:
            ZNet_AddFuncToRESPONSE( func );
            break;
        case FuncResults_Next:
            ZNet_MoveFuncFromRESPONSEToNextState( func );
            break;
        default:
            break;
        }
}

//----------------------------------------------------------------------------
static bool ICACHE_FLASH_ATTR CheckFuncToCALLBACK( const void* object, const void* arg ) {
    ZFunction func = (ZFunction)object;
    Buffer rx_frame = (Buffer)arg;
    uint8_t callback_id = func->_tx_frame._data[func->_tx_frame._size - 2];
    if( func->_fc->_id == fcAddNodeToNetwork._id && func->_tx_frame._data[4] == ADD_NODE_HOME_ID )
        callback_id = func->_tx_frame._data[5];
    return ( /*fc_id*/ rx_frame->_data[1] == func->_fc->_id && rx_frame->_data[2] == callback_id &&
             ( func->_fc->_IsCallbackFrameMine == NULL || ( *func->_fc->_IsCallbackFrameMine )( func ) ) );
}

//----------------------------------------------------------------------------
static void ICACHE_FLASH_ATTR ZNet_ProcessCALLBACK() {
    Buffer rxed_frame = &znet_instance._frame;
    List_Entry entry;
    ZFunction func;
    if( ( entry = List_Find( &znet_instance._funcs_to_CALLBACK, &CheckFuncToCALLBACK, rxed_frame ) ) ) {
        func = (ZFunction)entry->_object;
        List_Remove( &znet_instance._funcs_to_CALLBACK, entry, NULL );
    }
    else {
        const ZFunctionClass fc = ZFunctionClass_GetById( rxed_frame->_data[1] /*fc_id*/ );
        if( fc == NULL )
            return;
        if( ( func = ZFunction_Create( fc, NULL, NULL, NULL ) ) == NULL )
            return;
    }

    ZFunctionProcessCALLBACK ProcessCALLBACK = func->_ProcessCALLBACK;
    if( ProcessCALLBACK == NULL ) {
        ZFunction_Free( func );
    }
    else {
        switch( ( *ProcessCALLBACK )( func ) ) {
        case FuncResults_Done:
            ZFunction_Free( func );
            Log_Write( "FuncResults_Done \r\n" );
            break;
        case FuncResults_Restart:
            ZNet_AddFuncToTX( func );
            Log_Write( "FuncResults_Restart \r\n" );
            break;
        case FuncResults_Repeat:
            ZNet_AddFuncToCALLBACK( func );
            Log_Write( "FuncResults_Repeat \r\n" );
            break;
        case FuncResults_Next:
            ZNet_AddFuncToCALLBACK( func );
            Log_Write( "FuncResults_Next \r\n" );
            break;
        default:
            break;
        }
    }
}

//----------------------------------------------------------------------------
void ICACHE_FLASH_ATTR ZNet_ProcessSOF() {
    switch( znet_instance._frame._data[0] ) {
    case RESPONSE:
        ZNet_ProcessRESPONSE();
        break;
    case REQUEST:
        ZNet_ProcessCALLBACK();
        break;
    default:
        break;
    }
    ZNet_RefreshTimeout();
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
static bool ICACHE_FLASH_ATTR CheckFunctionTimeout( const void* object, const void* UNUSED( arg ) ) {
    ZFunction func = (ZFunction)object;
    return ( func->_timeout <= func->_znet->_timeout );
}

//----------------------------------------------------------------------------
static bool FuncList_CheckTimeouts( List funcs /*, SystemTime_t timeout_delta, ZFuncFailures_e failure_reason*/ ) {
    List_Entry entry;
    ZFunction func = NULL;

    while( ( entry = List_Find( funcs, &CheckFunctionTimeout, NULL ) ) ) {
        func = (ZFunction)entry->_object;
        // ZFunctionProcessTimeout ProcessTimeout = func->_fc->_ProcessTimeout;

        List_Remove( funcs, entry, NULL );

        if( funcs == &znet_instance._funcs_to_ACK && func->_tx_attempt_counter < FUNCTION_CLASS_REQ_ATTEMPTS_COUNT ) {
            Log_Write( "FuncList_CheckTimeouts: %s have not receive ACK  %d\r\n",
                       func->_fc->_name,
                       func->_tx_attempt_counter );
            func->_znet->_postpone_sending_time =
                SystemTime_Now() +
                SystemTime_ms( FUNCTION_CLASS_TIMEOUT_100_MS + FUNCTION_CLASS_TIMEOUT_1_S * func->_tx_attempt_counter );
            func->_tx_attempt_counter++;
            ZNet_AddFuncToTX( func );
        }
        else {
            Log_Write( "FuncList_CheckTimeouts: 0x%02x - %s  node_id %d\r\n",
                       func->_fc->_id,
                       func->_fc->_name,
                       func->_node_id );
            if( func->_Callback )
                ( *func->_Callback )( func, func->_callbackArg, FUNC_FAILURE_TIMEOUT, NULL, 0 );
            ZFunction_Free( func );
        }
    }

    return ( func != NULL );
}

void ICACHE_FLASH_ATTR ZNet_ProcessTimeout() {
    bool found = FuncList_CheckTimeouts(
        &znet_instance
             ._funcs_to_ACK /*, SystemTime_ms( FUNCTION_CLASS_ACK_TIMEOUT_MS      ), FUNC_FAILURE_ADD_TO_ACK     */ );
    found |= FuncList_CheckTimeouts(
        &znet_instance._funcs_to_RESPONSE /*, SystemTime_ms( FUNCTION_CLASS_RESPONSE_TIMEOUT_MS ), FUNC_FAILURE_ADD_TO_RESPONSE*/ );
    found |= FuncList_CheckTimeouts(
        &znet_instance._funcs_to_CALLBACK /*, SystemTime_ms( FUNCTION_CLASS_CALLBACK_TIMEOUT_MS ), FUNC_FAILURE_ADD_TO_CALLBACK*/ );

    if( found ) {
        ZNet_RefreshTimeout();
    }
    else {
        znet_instance._timeout ^= znet_instance._timeout;
        znet_instance._timeout--;
    }
}

//----------------------------------------------------------------------------
/*

        static
        void
        ICACHE_FLASH_ATTR
        OutputJob( const List_Entry const UNUSED(entry), const void* UNUSED(arg)
   )
        {
                ZJob job = (ZJob)object;
        char status;
             if( job->_done          ) status = 'D';
        else if( job->_wait_wakeup   ) status = 'W';
        else if( job->_wait_security ) status = 'S';
        else                          status = ' ';

        const uint8_t* payload = job->_payload;

        char buf[ MAX_PACKET_SIZE * 3 + 1 ];
        for( int i = 0; i < job->_payload_size; i++ ) {
                sprintf( &buf[3 * i], "%02X ", payload[i] );
                }

        buf[ 3 * job->_payload_size - 1 ] = '\0'; // strip last space

        Log_Write( "%d %c %c %c %c %3.2f %3i ( %02X %02X %s %02X) %s\r\n",
                         job->_send_count,
                              status,
                                 job->_await_ack ? (job->_received_ack ? '+' :
   '-') : ' ', job->_await_response ? (job->_received_response ? '+' : '-') : '
   ', job->_await_callback ? (job->_received_callback ? '+' : '-') : ' ',
                                          job->_timeout,
                                            job->_node_id,
                                                        job->_frame_type,
                                                               job->_function_id,
                                                                      buf,
                                                                         job->_callback_id,
                                                                                 ZJob_GetDesc( job )
                         );
        }
                 */

/*
//----------------------------------------------------------------------------
void
ICACHE_FLASH_ATTR
znet_queue_inspect( const FILE* stream )
{
    if( this == NULL ) return;

    LOCK_ENTER( &this->_queue_mutex );
            List_ForEach( &this->_queue, OutputJob, stream );
    LOCK_EXIT( &this->_queue_mutex );

    fprintf(stream, "================================================\r\n");
}
*/

//----------------------------------------------------------------------------
void ICACHE_FLASH_ATTR ZNet_ApplicationCommandHandler( const uint8_t* buf,
                                                       uint8_t len,
                                                       uint8_t node_id,
                                                       encap_type_t security,
                                                       uint8_t endpoint ) {
    uint8_t message_type = ZCommandClass_FindTypeByCCData( buf );
    uint8_t call_report = 0;
    if( MESSAGE_TYPE_SET_GET == message_type || MESSAGE_TYPE_REPORT == message_type ) {
        if( MY_NODE_ID == 1 ) {
            encap_type_t node_security = get_security_by_id( node_id );
            if( ( node_security == security ) ||
                ( ( Encapsulation_None == node_security ) && ( Encapsulation_CRC16 == security ) ) ) {
                call_report = 1;
            }
            // CC:0074.01.01.11.005: This command MUST be sent through highest common Security Class
            else if( !is_inclusion_controller_initiate_command( buf, len ) ) {
                uint8_t find_in_sec_NIF = 0;
                for( uint8_t i = 0; i < nSecureClasses; i++ ) {
                    if( buf[0] == SecureClasses[i] ) {
                        find_in_sec_NIF = 1;
                        break;
                    }
                }

                const bool free_ok =
                    ( security == Encapsulation_None || security == Encapsulation_CRC16 ||
                      ( ( node_security == Encapsulation_None || node_security == Encapsulation_CRC16 ) &&
                        ( security != Encapsulation_None || security != Encapsulation_CRC16 ) ) );

                const bool secured_ok =
                    node_security == security && ( security != Encapsulation_None || security != Encapsulation_CRC16 );

                if( ( !find_in_sec_NIF && free_ok ) || ( find_in_sec_NIF && secured_ok ) ) {
                    call_report = 1;
                }
            }
        }
        else {
            encap_type_t my_security = get_security_by_id( MY_NODE_ID );
            encap_type_t node_security = get_security_by_id( node_id );
            if( ( my_security == security ) || ( node_security == security ) ||
                ( ( Encapsulation_None == my_security ) && ( Encapsulation_CRC16 == security ) ) ) {
                call_report = 1;
            }
            // Corner case for Manufacturer Specific command class with S0 encapsulation
            else if( my_security == Encapsulation_Sec_0 && security == Encapsulation_None &&
                     buf[0] == COMMAND_CLASS_MANUFACTURER_SPECIFIC_V2 ) {
                call_report = 1;
            }
            else {
                uint8_t find_in_sec_NIF = 0;
                for( uint8_t i = 0; i < nSecureClasses; i++ ) {
                    if( buf[0] == SecureClasses[i] ) {
                        find_in_sec_NIF = 1;
                        break;
                    }
                }
                if( find_in_sec_NIF == 0 )
                    call_report = 1;
            }
        }
    }
    else {
        call_report = 1;
    }

    if( call_report ) {
        ZFunction func;
        const ZFunctionClass fc = ZFunctionClass_GetById( 0x04 /*fc_id*/ );  // 0x04
        if( fc == NULL )
            return;
        if( ( func = ZFunction_Create( fc, NULL, NULL, NULL ) ) == NULL )
            return;
        func->_security = security;
        func->_endpoint = endpoint;

#if ZNET_SUPPORT_PENDING_COMMANDS_FOR_SLEEP_NODES
        ZNet_WakingNode( node_id, !0 );
#endif
        const uint8_t* cc_report_data = &buf[0];
        CC_Report cc_report = ZCommandClass_FindCCReportByCCData( cc_report_data );
        if( cc_report ) {
            ( *cc_report )( func, node_id, len, buf );
        }
        ZFunction_Free( func );
    }
    else {
        Log_Write( "=== wrong security ===\r\n" );
    }
}

//----------------------------------------------------------------------------
ZMessage ICACHE_FLASH_ATTR ZMessage_Create( const uint8_t* data,
                                            uint8_t data_length,
                                            uint8_t node_id,
                                            uint8_t encap,
                                            ZFunction_Callback callback,
                                            void* callbackArg ) {
    ZMessage message = Heap_Alloc( sizeof( ZMessage_t ) );
    if( message ) {
        *( (ZNet*)&message->_znet ) = &znet_instance;
        message->_message = (void*)Heap_Alloc( data_length );
        memcpy( message->_message, data, data_length );
        message->_length = data_length;
        message->_node_id = node_id;
        message->_callback = callback;
        message->_callbackArg = callbackArg;
        message->_encap = encap;
    }
    return message;
}

//----------------------------------------------------------------------------
void ICACHE_FLASH_ATTR ZMessage_Free( void* message ) {
    if( message == NULL )
        return;

    ZMessage this = (ZMessage)message;
    if( this->_message ) {
        Heap_Free( this->_message );
    }
    Heap_Free( this );
}
//----------------------------------------------------------------------------
bool ICACHE_FLASH_ATTR ZNet_Add_Security_Message( ZMessage message ) {
    if( message == NULL )
        return FALSE;

    if( List_Add( &znet_instance._security_message, message ) == NULL )
        return FALSE;
    else
        return TRUE;
}

//----------------------------------------------------------------------------
void ICACHE_FLASH_ATTR ZNet_Remove_Security_Message( ZMessage message ) {
    if( message ) {
        List_Entry entry = List_FindByObject( &znet_instance._security_message, message );
        if( entry ) {
            List_Remove( &znet_instance._security_message, entry, ZMessage_Free );
        }
    }
}

#if ZNET_SUPPORT_PENDING_COMMANDS_FOR_SLEEP_NODES
//----------------------------------------------------------------------------
static void ICACHE_FLASH_ATTR _ZNet_ProcessWaitingCommands_Timer( ZTimer func, void* arg, int destroy ) {
    if( destroy )
        return;

    List_Entry entry = (List_Entry)arg;
    ZWaitingCommand cmd = (ZWaitingCommand)entry->_object;

    ZWaitingCommand_Call( cmd );
    List_Remove( &znet_instance._waiting_commands, entry, NULL );
    ZNet_ProcessWaitingCommands( cmd->_node_id );
    ZWaitingCommand_Free( cmd );
}

static bool ICACHE_FLASH_ATTR _ZNet_ProcessWaitingCommands_CheckForProcess( const void* object, const void* arg ) {
    const ZWaitingCommand x = (const ZWaitingCommand)object;
    const uint8_t node_id = *( (const uint8_t*)arg );
    return ( x->_node_id == node_id );
}

void ICACHE_FLASH_ATTR ZNet_ProcessWaitingCommands( uint8_t node_id ) {
    if( !ZNet_IsWakingNode( node_id ) )
        return;
    /// TODO: investigate variant with loop
    List_Entry entry = List_Find(
        &znet_instance._waiting_commands, _ZNet_ProcessWaitingCommands_CheckForProcess, (const void*)&node_id );
    if( entry ) {
        ZTimer timer = ZTimer_Create( 0, _ZNet_ProcessWaitingCommands_Timer, entry );
        if( timer ) {
            if( ZNet_AddTimer( timer ) )
                return;
            ZTimer_Free( timer );
        }
    }

    // TODO it causes intarrapting interview if a wakeup notification has been received too
    // TODO  early (for some devices ) and a queue is empty (it sends the sleep command to a device)
    // force sleep node
    // encap_type_t encap = get_security_by_id_and_cc( node_id, COMMAND_CLASS_WAKE_UP );

    // znet_cc_wakeup_sleep( node_id, NULL, NULL, encap );
}
#endif

#if ZNET_SUPPORT_PENDING_COMMANDS_FOR_SLEEP_NODES
//----------------------------------------------------------------------------
static bool ICACHE_FLASH_ATTR _ZNet_AddWaitingCommand_CheckForReplace( const void* object, const void* arg ) {
    const ZWaitingCommand x = (const ZWaitingCommand)object;
    const ZWaitingCommand y = (const ZWaitingCommand)arg;
    bool result =
        ( x->_func_class_id == y->_func_class_id && x->_node_id == y->_node_id && x->_channel_id == y->_channel_id );
    if( fcSendData._id == x->_func_class_id || fcSendDataBridge._id == x->_func_class_id ) {
        result = result && ( x->_data_size >= 2 && y->_data_size >= 2 && x->_data[0] == y->_data[0] &&
                             x->_data[1] == y->_data[1] );
        if( result && ZNET_COMMAND_CLASS_CONFIGURATION == x->_data[0] ) {
            result = znet_cc_confguration_get_param_number( x->_data, x->_data_size ) ==
                     znet_cc_confguration_get_param_number( y->_data, y->_data_size );
        }
    }
    return result;
}

znet_error_t ICACHE_FLASH_ATTR ZNet_AddWaitingCommand( uint8_t func_class_id,
                                                       uint8_t node_id,
                                                       uint8_t channel_id,
                                                       ZFunction_Callback cb,
                                                       void* cb_arg,
                                                       encap_type_t encap,
                                                       const void* data,
                                                       size_t data_size ) {
    ZWaitingCommand waiting_command =
        ZWaitingCommand_Create( func_class_id, node_id, channel_id, cb, cb_arg, encap, data, data_size );
    if( !waiting_command ) {
        return ZNET_ERR_NOMEM;
    }

    List_Entry entry = List_Find(
        &znet_instance._waiting_commands, _ZNet_AddWaitingCommand_CheckForReplace, (const void*)waiting_command );
    if( entry ) {
        List_Remove( &znet_instance._waiting_commands, entry, ZWaitingCommand_Free );
    }

    if( List_Add( &znet_instance._waiting_commands, waiting_command ) == NULL ) {
        return ZNET_ERR_FAULT;
    }

    return ZNET_ERR_OK;
}
#endif

#if ZNET_SUPPORT_PENDING_COMMANDS_FOR_SLEEP_NODES
//----------------------------------------------------------------------------
void ICACHE_FLASH_ATTR ZNet_WakingNode( uint8_t node_id, int waking ) {
    const size_t idx = node_id / 8;
    const size_t sh = node_id % 8;

    if( waking ) {
        znet_instance._waking_nodes[idx] |= (uint8_t)0x01 << sh;
    }
    else {
        znet_instance._waking_nodes[idx] &= ~( (uint8_t)0x01 << sh );
    }
}
#endif

#if ZNET_SUPPORT_PENDING_COMMANDS_FOR_SLEEP_NODES
//----------------------------------------------------------------------------
int ICACHE_FLASH_ATTR ZNet_IsWakingNode( uint8_t node_id ) {
    if( !node_is_wakeup( node_id ) )
        return !0;

    const size_t idx = node_id / 8;
    const size_t sh = node_id % 8;

    return ( ( znet_instance._waking_nodes[idx] >> sh ) & 0x01 ) == 0x01;
}
#endif

void ZNet_CheckCallback( ZFunction func, ZFuncFailures_e reason, uint8_t* nodeId, uint8_t* endpoint ) {
    *nodeId = ZNET_NODE_ID_INVALID;
    if( endpoint != NULL ) {
        *endpoint = 0;
    }
    if( func != NULL && func->_tx_frame._size > 4 && func->_node_id >= ZNET_NODE_ID_MIN &&
        func->_node_id <= ZNET_NODE_ID_MAX ) {
        *nodeId = func->_node_id;
        if( endpoint != NULL ) {
            *endpoint = func->_endpoint;
        }
    }
}
