/// INFO: for esp32 and linux use callback functions
#if( 0 && defined( ESP_PLATFORM ) )

//////////////////////////////////////////////////////////////////////////////

#include "logger.h"
#include <esp_log.h>

#include "driver/uart.h"

#include "serial_port_impl.h"

//////////////////////////////////////////////////////////////////////////////

#ifndef MAX
#define MAX( x, y ) ( ( x ) > ( y ) ? ( x ) : ( y ) )
#endif
#define BUF_SIZE MAX( 512, UART_FIFO_LEN )

//////////////////////////////////////////////////////////////////////////////

bool inline SerialPort_DataBits_Convert( DataBits_e inDataBits, uart_word_length_t* outDataBits ) {
    switch( inDataBits ) {
    case DataBits_5:
        *outDataBits = UART_DATA_5_BITS;
        return TRUE;
    case DataBits_6:
        *outDataBits = UART_DATA_6_BITS;
        return TRUE;
    case DataBits_7:
        *outDataBits = UART_DATA_7_BITS;
        return TRUE;
    case DataBits_8:
        *outDataBits = UART_DATA_8_BITS;
        return TRUE;
    }

    return FALSE;
}

bool inline SerialPort_Parity_Convert( Parities_e inParity, uart_parity_t* outParity ) {
    switch( inParity ) {
    case Parities_None:
        *outParity = UART_PARITY_DISABLE;
        return TRUE;
    case Parities_Odd:
        *outParity = UART_PARITY_ODD;
        return TRUE;
    case Parities_Even:
        *outParity = UART_PARITY_EVEN;
        return TRUE;
    case Parities_Mark:
        return FALSE;
    case Parities_Space:
        return FALSE;
    }

    return FALSE;
}

bool inline SerialPort_StopBits_Convert( StopBits_e inStopBits, uart_stop_bits_t* outStopBits ) {
    switch( inStopBits ) {
    case StopBits_1:
        *outStopBits = UART_STOP_BITS_1;
        return TRUE;
    case StopBits_15:
        *outStopBits = UART_STOP_BITS_1_5;
        return TRUE;
    case StopBits_2:
        *outStopBits = UART_STOP_BITS_2;
        return TRUE;
    }

    return FALSE;
}

//----------------------------------------------------------------------------

bool ICACHE_FLASH_ATTR SerialPortImpl_Init( SerialPortImpl this,
                                            SerialPortIdentifier_t serialPortId,
                                            Handshakings_e handshaking,
                                            BaudRates_t baudRate,
                                            DataBits_e dataBits,
                                            bool parityEnabled,
                                            Parities_e parity,
                                            StopBits_e stopBits ) {
    if( this == NULL )
        return FALSE;

    if( this->_isInit )
        SerialPortImpl_DeInit( this );

    if( serialPortId >= UART_NUM_MAX )
        return FALSE;
    this->_serialPortId = serialPortId;

    uart_config_t uart_config = {};
    uart_config.baud_rate = baudRate;
    if( !SerialPort_DataBits_Convert( dataBits, &uart_config.data_bits ) )
        return FALSE;
    if( !SerialPort_Parity_Convert( parity, &uart_config.parity ) )
        return FALSE;
    if( !SerialPort_StopBits_Convert( stopBits, &uart_config.stop_bits ) )
        return FALSE;
    /// TODO: implement others Handshakings_e
    if( handshaking != Handshakings_None ) {
        return FALSE;
    }
    uart_config.flow_ctrl = UART_HW_FLOWCTRL_DISABLE;

    if( uart_param_config( serialPortId, &uart_config ) != ESP_OK )
        return FALSE;
    /// INFO: default pins
    ///       uart_set_pin( UART_NUM_0, GPIO_NUM_1, GPIO_NUM_3, GPIO_NUM_22, GPIO_NUM_19 );
    ///       uart_set_pin( UART_NUM_1, GPIO_NUM_10, GPIO_NUM_9, GPIO_NUM_11, GPIO_NUM_6 );
    ///       uart_set_pin( UART_NUM_2, GPIO_NUM_17, GPIO_NUM_16, GPIO_NUM_7, GPIO_NUM_8 );
    switch( serialPortId ) {
    case UART_NUM_0:
        if( uart_set_pin( serialPortId, GPIO_NUM_1, GPIO_NUM_3, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE ) != ESP_OK )
            return FALSE;
        break;

    case UART_NUM_1:
        if( uart_set_pin( serialPortId, GPIO_NUM_10, GPIO_NUM_9, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE ) != ESP_OK )
            return FALSE;
        break;

    case UART_NUM_2:
        if( uart_set_pin( serialPortId, GPIO_NUM_17, GPIO_NUM_16, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE ) != ESP_OK )
            return FALSE;
        break;

    default:
        return FALSE;
    }

    if( uart_driver_install( serialPortId, BUF_SIZE + UART_FIFO_LEN, BUF_SIZE + UART_FIFO_LEN, 0, NULL, 0 ) != ESP_OK )
        return FALSE;

    return ( this->_isInit = TRUE );
}

//----------------------------------------------------------------------------

void ICACHE_FLASH_ATTR SerialPortImpl_DeInit( SerialPortImpl this ) {
    if( this == NULL || !this->_isInit )
        return;

    if( this->_isOpen )
        SerialPortImpl_Close( this );

    uart_driver_delete( this->_serialPortId );

    this->_isInit = FALSE;
}

//----------------------------------------------------------------------------

bool ICACHE_FLASH_ATTR SerialPortImpl_Open( SerialPortImpl this ) {
    if( this == NULL || !this->_isInit )
        return FALSE;

    if( this->_isOpen )
        return TRUE;

    /// TODO: investigate... maybe it's better to move the code here? uart_driver_install

    return ( this->_isOpen = TRUE );
}

//----------------------------------------------------------------------------

bool ICACHE_FLASH_ATTR SerialPortImpl_Close( SerialPortImpl this ) {
    if( this == NULL || !this->_isInit || !this->_isOpen )
        return FALSE;

    uart_wait_tx_done( this->_serialPortId, 20 / portTICK_RATE_MS );  /// TODO: maybe need drop this call?
    uart_flush( this->_serialPortId );

    /// TODO: investigate... maybe it's better to move the code here? uart_driver_delete

    this->_isOpen = FALSE;

    return TRUE;
}

//----------------------------------------------------------------------------

bool ICACHE_FLASH_ATTR SerialPortImpl_IsOpen( SerialPortImpl this ) {
    return this->_isOpen;
}

//----------------------------------------------------------------------------
int ICACHE_FLASH_ATTR SerialPortImpl_Read( SerialPortImpl this, int lenToRead, uint8_t* buffer ) {
    if( this == NULL || !this->_isOpen || buffer == NULL || lenToRead <= 0 )
        return 0;

    int readLen = uart_read_bytes( this->_serialPortId, buffer, lenToRead, 0 /*no wait*/ );
    if( readLen < 0 ) {
        return 0;
    }
    esp_log_buffer_hexdump_internal( "uart read ", buffer, readLen, ESP_LOG_DEBUG );

    return readLen;
}

//----------------------------------------------------------------------------

int ICACHE_FLASH_ATTR SerialPortImpl_WriteCount( SerialPortImpl this ) {
    if( this == NULL || !this->_isOpen )
        return 0;

    return BUF_SIZE;
}

//----------------------------------------------------------------------------

int ICACHE_FLASH_ATTR SerialPortImpl_Write( SerialPortImpl this, int lenToWrite, uint8_t* buffer ) {
    if( this == NULL || !this->_isOpen || buffer == NULL || lenToWrite <= 0 )
        return 0;

    esp_log_buffer_hexdump_internal( "uart write", buffer, lenToWrite, ESP_LOG_DEBUG );
    int writeLen = uart_write_bytes( this->_serialPortId, (const char*)buffer, lenToWrite );
    if( writeLen < 0 ) {
        return 0;
    }

    return writeLen;
}

#endif  // ( defined( ESP_PLATFORM ) )
