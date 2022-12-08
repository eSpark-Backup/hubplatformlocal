#include "stdio.h"

#include "byte_ops.h"

#include "znet.h"
#include "znet_lib.h"
#include "znet_lib_fc.h"
#include "znet_private.h"
#include "znet_fc_private.h"
//
#include "znet_main.h"

#include "states.h"
#include "system_time.h"
#include "znet_lib_cc.h"
#include "ZPlatform.h"
#include "logger.h"

#include "serial_port_impl.h"
#include "S2_wrap.h"
#include "user_interface.h"
#include "typedefs.h"

StateContext_t _context;

#define ENABLE_ZNA_LOGS

extern void SendDataSecurity( ZMessage message );

//----------------------------------------------------------------------------

static int ICACHE_FLASH_ATTR CalcChecksum( const CyclicBuffer buffer, const int _lenght ) {
    uint8_t checksum = 0xFF;
    for( int i = 0; i < _lenght; ++i ) {
        checksum ^= CyclicBuffer_PeekByte( buffer, i );
    }
    return checksum;
}

//////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------
static void ICACHE_FLASH_ATTR ZNet_SendFuncTXFrame( ZNet this ) {
    CyclicBuffer buffer;
    SystemTime_t now = SystemTime_Now();
    if( now > this->_postpone_sending_time && ZFunction_Check_is_Lists_Empty() ) {
        if( this->_func_to_TX )
            buffer = &this->_func_to_TX->_tx_frame;
        else {
            List_Entry entry = List_First( &this->_funcs_to_TX );
            if( entry == NULL )
                return;  // FALSE;
            this->_func_to_TX = (ZFunction)entry->_object;
            List_Remove( &this->_funcs_to_TX, entry, NULL );
            buffer = &this->_func_to_TX->_tx_frame;
#ifdef ENABLE_ZNA_LOGS
            LogCyclicBuffer( "<<< ", buffer, buffer->_len );
#endif
        }
        this->_func_to_TX->_timeout = now + SystemTime_ms( FUNCTION_CLASS_ACK_TIMEOUT_MS );
        SerialPort_WriteCyclicBuffer( this->_serialPort, buffer->_len, buffer );
        if( buffer->_len > 0 )
            return;  // FALSE;

        buffer->_begin = 0;
        buffer->_len = buffer->_end;

        ZNet_AddFuncToACK( this->_func_to_TX );

        this->_func_to_TX = NULL;
    }

    ZNet_RefreshTimeout();
}

//----------------------------------------------------------------------------
void ICACHE_FLASH_ATTR ZNetRXActivity_init() {
    STATES_CONTEXT_RESET( &_context );

    extern void ICACHE_FLASH_ATTR ZW_TransportService_Init( ZW_CommandHandler_Callback_t commandHandler );
    ZW_TransportService_Init( NULL );
}

//----------------------------------------------------------------------------
// DECLARE_ACTIVITY_FUNC( ZNetRXActivityFunc )
uint8_t ZNetRXActivityFunc() {
    if( !SystemTime_Refresh() )
        return FALSE;

    znet_timer_process();

    StateContext ___stateContext___ = &_context;
    extern ZNet_t znet_instance;
    ZNet this = &znet_instance;

    ZNet_ProcessTimers();

    if( !CyclicBuffer_IsFull( &this->_rxedBuffer ) ) {
        SerialPort_ReadCyclicBuffer( this->_serialPort, &this->_rxedBuffer );
    }

    if( CyclicBuffer_IsEmpty( &this->_rxedBuffer ) ) {
        if( Message_Not_Sending == message_sending ) {
            List_Entry entry = List_First( &znet_instance._security_message );
            if( entry ) {
                ZMessage message = (ZMessage)entry->_object;
                Log_Write( "sec message from queue. Node: %d \r\n", message->_node_id );
                SendDataSecurity( message );
            }
        }
        ZNet_SendFuncTXFrame( this );
    }

    STATES_BEGIN;

_L_loop_:

    STATES_WAIT( ( !CyclicBuffer_IsEmpty( &this->_rxedBuffer ) || ( SystemTime_Now() >= this->_timeout ) ) );
    if( this->_rxedBuffer._len <= 0 )
        goto _E_timeout_;

    int type = CyclicBuffer_ReadByte( &this->_rxedBuffer );
    if( type != SOF ) {
#ifdef ENABLE_ZNA_LOGS
        LogCyclicBuffer2( ">>> ", type, &this->_rxedBuffer, 1 );
#endif
        switch( type ) {
        case NAK:
        case CAN:
            ZNet_ProcessNAKandCAN( this );
            break;
        case ACK:
            ZNet_ProcessACK( this );
            break;
        default:
            // znet_log(znet, Error, "RECEIVED UNKNOWN PACKET TYPE: 0x%02x\r\n", buffer[0]);
            break;
        }
        goto _L_loop_;
    }

    // wait for length byte
    STATES_WAIT_TIMED( ( !CyclicBuffer_IsEmpty( &this->_rxedBuffer ) ), SystemTime_ms( 1600 ), _E_timeout_ );

    this->_frameLenToRX = CyclicBuffer_PeekByte( &this->_rxedBuffer, 0 );  // "Payload" bytes + "Lenght" byte
    if( this->_frameLenToRX - 1 > this->_frame._size ) {                   // -1 because "Length" byte does not copying
        goto _E_send_NAK_;
    }

    // wait for the tail of a packet and +1 means "Checksum" byte
    STATES_WAIT_TIMED( ( CyclicBuffer_NumBytes( &this->_rxedBuffer ) >= this->_frameLenToRX + 1 ),
                       SystemTime_ms( 1600 ),
                       _E_timeout_ );
#ifdef ENABLE_ZNA_LOGS
    LogCyclicBuffer2( ">>> ", SOF, &this->_rxedBuffer, this->_rxedBuffer._len );
#endif
    const int checksum = CalcChecksum( &this->_rxedBuffer, this->_frameLenToRX );
    if( checksum == CyclicBuffer_PeekByte( &this->_rxedBuffer, this->_frameLenToRX ) ) {
        // send ACK ----------------
        this->_frame._data[0] = ACK;
        this->_frame._len = 1;
        if( SerialPort_WriteBuffer( this->_serialPort, 1, &this->_frame ) != 1 ) {
            Log_Write( "Fail to send  ACK! \r\n" );
            goto _E_timeout_;
        }
#ifdef ENABLE_ZNA_LOGS
        LogBuffer( "<<< ", &this->_frame, this->_frame._len );
#endif
        CyclicBuffer_ReadByte( &this->_rxedBuffer );  // Skip "Lenght"
        this->_frameLenToRX -= 1;
        this->_frame._len = CyclicBuffer_GetMem( &this->_rxedBuffer, this->_frame._data, this->_frameLenToRX );
        CyclicBuffer_ReadByte( &this->_rxedBuffer );  // Skip "Checksum"
        ZNet_ProcessSOF( this );
    }
    else {
        goto _E_send_NAK_;
    }

    goto _L_loop_;

_E_send_NAK_:
    // send NAK ----------------
    this->_frame._data[0] = NAK;
    this->_frame._len = 1;
    SerialPort_WriteBuffer( this->_serialPort, 1, &this->_frame );
#ifdef ENABLE_ZNA_LOGS
    LogBuffer( "<<< ", &this->_frame, this->_frame._len );
#endif
_E_timeout_:
#ifdef ENABLE_ZNA_LOGS
    Log_Write( "\r\n=== _E_timeout_ ===\r\n" );
#endif
    CyclicBuffer_Clear( &this->_rxedBuffer );
    ZNet_ProcessTimeout();

    goto _L_loop_;

    return ( -1 );
}
//----------------------------------------------------------------------------
