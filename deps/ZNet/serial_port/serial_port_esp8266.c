
#if( !defined( _WINDOWS ) && !defined( __linux__ ) && !defined( ESP_PLATFORM ) && !defined( __APPLE__ ) )

//////////////////////////////////////////////////////////////////////////////

#include <eagle_soc.h>

#include "logger.h"

#include "ets_sys.h"
#include "espmissingincludes.h"
#include "uart_register.h"
//#include "uart.h"

#include "serial_port_impl.h"
#include "serial_port_esp8266.h"
#include "serial_port.h"

//////////////////////////////////////////////////////////////////////////////

#define UART_FIFO_LEN 128  // define the tx fifo length

#define UART0 0
#define UART1 1

#define UART_RXD_INV ( BIT( 19 ) )
#define UART_CTS_INV ( BIT( 20 ) )
#define UART_TXD_INV ( BIT( 22 ) )
#define UART_RTS_INV ( BIT( 23 ) )

//#define UART_PARITY_EN ( BIT( 1 ) )
#define UART_PARITY_EN_M 0x00000001
#define UART_PARITY_EN_S 1
#define UART_PARITY_M 0x00000001
#define UART_PARITY_S 0

typedef enum { FIVE_BITS = 0x0, SIX_BITS = 0x1, SEVEN_BITS = 0x2, EIGHT_BITS = 0x3 } UartBitsNum4Char;

typedef enum { ONE_STOP_BIT = 0x1, ONE_HALF_STOP_BIT = 0x2, TWO_STOP_BIT = 0x3 } UartStopBitsNum;

typedef enum { NONE_BITS = 0x2, ODD_BITS = 1, EVEN_BITS = 0 } UartParityMode;

typedef enum { STICK_PARITY_DIS = 0, STICK_PARITY_EN = 1 } UartExistParity;

typedef enum {
    UART_None_Inverse = 0x0,
    UART_Rxd_Inverse = UART_RXD_INV,
    UART_CTS_Inverse = UART_CTS_INV,
    UART_Txd_Inverse = UART_TXD_INV,
    UART_RTS_Inverse = UART_RTS_INV,
} UART_LineLevelInverse;

typedef enum {
    BIT_RATE_300 = 300,
    BIT_RATE_600 = 600,
    BIT_RATE_1200 = 1200,
    BIT_RATE_2400 = 2400,
    BIT_RATE_4800 = 4800,
    BIT_RATE_9600 = 9600,
    BIT_RATE_19200 = 19200,
    BIT_RATE_38400 = 38400,
    BIT_RATE_57600 = 57600,
    BIT_RATE_74880 = 74880,
    BIT_RATE_115200 = 115200,
    BIT_RATE_230400 = 230400,
    BIT_RATE_460800 = 460800,
    BIT_RATE_921600 = 921600,
    BIT_RATE_1843200 = 1843200,
    BIT_RATE_3686400 = 3686400,
} UartBautRate;

typedef enum { NONE_CTRL, HARDWARE_CTRL, XON_XOFF_CTRL } UartFlowCtrl;

typedef enum {
    USART_HardwareFlowControl_None = 0x0,
    USART_HardwareFlowControl_RTS = 0x1,
    USART_HardwareFlowControl_CTS = 0x2,
    USART_HardwareFlowControl_CTS_RTS = 0x3
} UART_HwFlowCtrl;

//////////////////////////////////////////////////////////////////////////////

/////// RX
//----------------------------------------------------------------------------
inline void UART_RX_EI( int device ) {
    SET_PERI_REG_MASK( UART_INT_ENA( device ), UART_RXFIFO_FULL_INT_ENA );
}
//----------------------------------------------------------------------------
inline void UART_RX_DI( int device ) {
    CLEAR_PERI_REG_MASK( UART_INT_ENA( device ), UART_RXFIFO_FULL_INT_ENA );
}
//----------------------------------------------------------------------------
inline void UART_RX_CI( int device ) {
    WRITE_PERI_REG( UART_INT_CLR( device ), UART_RXFIFO_FULL_INT_CLR );
}
//----------------------------------------------------------------------------
inline bool UART_IS_RXED( int device ) {
    return ( UART_RXFIFO_FULL_INT_ST == ( READ_PERI_REG( UART_INT_ST( device ) ) & UART_RXFIFO_FULL_INT_ST ) );
}

//----------------------------------------------------------------------------
inline int UART_RX_LEN( int device ) {
    return ( ( READ_PERI_REG( UART_STATUS( device ) ) >> UART_RXFIFO_CNT_S ) & UART_RXFIFO_CNT );
}
//----------------------------------------------------------------------------
inline void UART_RX_CLEAR( int device ) {
    SET_PERI_REG_MASK( UART_CONF0( device ), UART_RXFIFO_RST );
    CLEAR_PERI_REG_MASK( UART_CONF0( device ), UART_RXFIFO_RST );
}
//----------------------------------------------------------------------------
inline uint8_t UART_READ( int device ) {
    return ( READ_PERI_REG( UART_FIFO( device ) ) & 0xFF );
}

/////// TX
//----------------------------------------------------------------------------
inline void UART_TX_EI( int device ) {
    SET_PERI_REG_MASK( UART_INT_ENA( device ), UART_TXFIFO_EMPTY_INT_ENA );
}
//----------------------------------------------------------------------------
inline void UART_TX_DI( int device ) {
    CLEAR_PERI_REG_MASK( UART_INT_ENA( device ), UART_TXFIFO_EMPTY_INT_ENA );
}
//----------------------------------------------------------------------------
inline void UART_TX_CI( int device ) {
    WRITE_PERI_REG( UART_INT_CLR( device ), UART_TXFIFO_EMPTY_INT_CLR );
}
//----------------------------------------------------------------------------
inline bool UART_IS_TXED( int device ) {
    return ( UART_TXFIFO_EMPTY_INT_ST == ( READ_PERI_REG( UART_INT_ST( device ) ) & UART_TXFIFO_EMPTY_INT_ST ) );
}

//----------------------------------------------------------------------------
inline int UART_TX_LEN( int device ) {
    return ( ( READ_PERI_REG( UART_STATUS( device ) ) >> UART_TXFIFO_CNT_S ) & UART_TXFIFO_CNT );
}
//----------------------------------------------------------------------------
inline void UART_TX_CLEAR( int device ) {
    SET_PERI_REG_MASK( UART_CONF0( device ), UART_TXFIFO_RST );
    CLEAR_PERI_REG_MASK( UART_CONF0( device ), UART_TXFIFO_RST );
}
//----------------------------------------------------------------------------
inline void UART_WRITE( int device, uint8_t byteToTX ) {
    WRITE_PERI_REG( UART_FIFO( device ), byteToTX );
}

//----------------------------------------------------------------------------

void
// ICACHE_FLASH_ATTR
UART_ISR( void* arg ) {
    //    SerialPortImpl this = (SerialPortImpl)arg;
    extern SerialPort_t serialPort;
    SerialPortImpl this = &serialPort._impl;
    int device = this->_device;
    CyclicBuffer buffer;
    uint8_t byte;

    if( UART_IS_RXED( device ) ) {
        buffer = &this->_rxBuffer;

        while( !CyclicBuffer_IsFull( buffer ) && UART_RX_LEN( device ) > 0 ) {
            byte = UART_READ( device );
            // Log_Write( "%02x", byte );
            CyclicBuffer_PutMem( buffer, &byte, 1 );
        }
        if( UART_RX_LEN( device ) == 0 ) {
            UART_RX_CI( device );
        }
        UART_RX_EI( device );
    }

    if( UART_IS_TXED( device ) ) {
        buffer = &this->_txBuffer;

        while( CyclicBuffer_GetMem( buffer, &byte, 1 ) == 1 && ( UART_FIFO_LEN - UART_TX_LEN( device ) ) > 0 ) {
            UART_WRITE( device, byte );
        }

        UART_TX_CI( device );
        if( CyclicBuffer_IsEmpty( buffer ) )
            UART_TX_DI( device );
        else
            UART_TX_EI( device );
    }
}

//////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------

UartBitsNum4Char ICACHE_FLASH_ATTR SerialPort_DataBits_ConvertToMCU( DataBits_e dataBits ) {
    switch( dataBits ) {
    case DataBits_5:
        return FIVE_BITS;
    case DataBits_6:
        return SIX_BITS;
    case DataBits_7:
        return SEVEN_BITS;
    case DataBits_8:
        return EIGHT_BITS;
    }
    return EIGHT_BITS;
}
//------------------------------------------------------------------------

UartStopBitsNum ICACHE_FLASH_ATTR SerialPort_StopBits_ConvertToMCU( StopBits_e stopBits ) {
    switch( stopBits ) {
    case StopBits_1:
        return ONE_STOP_BIT;
    case StopBits_15:
        return ONE_HALF_STOP_BIT;
    case StopBits_2:
        return TWO_STOP_BIT;
    }
    return ONE_STOP_BIT;
}
//------------------------------------------------------------------------

UartParityMode ICACHE_FLASH_ATTR SerialPort_Parity_ConvertToMCU( Parities_e parity ) {
    switch( parity ) {
    case Parities_None:
        return NONE_BITS;
    case Parities_Odd:
        return ODD_BITS;
    case Parities_Even:
        return EVEN_BITS;
    }
    return NONE_BITS;
}
//------------------------------------------------------------------------

UartBautRate ICACHE_FLASH_ATTR SerialPort_Baudrate_ConvertToMCU( BaudRates_t baudrate ) {
    switch( baudrate ) {
    case 300:
        return BIT_RATE_300;
    case 600:
        return BIT_RATE_600;
    case 1200:
        return BIT_RATE_1200;
    case 2400:
        return BIT_RATE_2400;
    case 4800:
        return BIT_RATE_4800;
    case 9600:
        return BIT_RATE_9600;
    case 19200:
        return BIT_RATE_19200;
    case 38400:
        return BIT_RATE_38400;
    case 57600:
        return BIT_RATE_57600;
    case 74880:
        return BIT_RATE_74880;
    case 115200:
        return BIT_RATE_115200;
    case 230400:
        return BIT_RATE_230400;
    case 460800:
        return BIT_RATE_460800;
    case 921600:
        return BIT_RATE_921600;
    case 1843200:
        return BIT_RATE_1843200;
    case 3686400:
        return BIT_RATE_3686400;
    }
    return BIT_RATE_115200;
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

    int device = this->_device = serialPortId;

    ETS_UART_INTR_DISABLE();
    SET_PERI_REG_MASK( UART_INT_ENA( device ), 0xffff );

    this->_isOpen = FALSE;

    /* rcv_buff == NULL size if 0 */
    ETS_UART_INTR_ATTACH( UART_ISR, this );
    if( device == 0 ) {
        PIN_PULLUP_DIS( PERIPHS_IO_MUX_U0TXD_U );
        PIN_FUNC_SELECT( PERIPHS_IO_MUX_U0TXD_U, FUNC_U0TXD );
    }
    uart_div_modify( device, UART_CLK_FREQ / SerialPort_Baudrate_ConvertToMCU( baudRate ) );  // SET BAUDRATE

    WRITE_PERI_REG(
        UART_CONF0( device ),
        ( ( ( parityEnabled ? 1 : 0 ) & UART_PARITY_EN_M ) << UART_PARITY_EN_S )  // SET BIT AND PARITY MODE
            | ( ( SerialPort_Parity_ConvertToMCU( parity ) & UART_PARITY_M ) << UART_PARITY_S ) |
            ( ( SerialPort_StopBits_ConvertToMCU( stopBits ) & UART_STOP_BIT_NUM ) << UART_STOP_BIT_NUM_S ) |
            ( ( SerialPort_DataBits_ConvertToMCU( dataBits ) & UART_BIT_NUM ) << UART_BIT_NUM_S ) );
    // clear rx and tx fifo,not ready
    UART_RX_CLEAR( device );
    UART_TX_CLEAR( device );

    WRITE_PERI_REG(
        UART_CONF1( device ),
        ( ( 127 & UART_TXFIFO_EMPTY_THRHD ) << UART_TXFIFO_EMPTY_THRHD_S ) |
            ( ( 1 & UART_RXFIFO_FULL_THRHD ) << UART_RXFIFO_FULL_THRHD_S )  // TODO: OR 0 instead of 1 ??????
    );

    WRITE_PERI_REG( UART_INT_CLR( device ),
                    0xffff );  // disable all UART interrapts
    // SET_PERI_REG_MASK( UART_INT_ENA(device), UART_RXFIFO_FULL_INT_ENA |
    // UART_TXFIFO_EMPTY_INT_ENA );

    this->_rxBuffer = ( CyclicBuffer_t ){
        ._data = this->_rxBufferData, ._size = sizeof this->_rxBufferData, ._len = 0, ._begin = 0, ._end = 0
    };
    this->_txBuffer = ( CyclicBuffer_t ){
        ._data = this->_txBufferData, ._size = sizeof this->_txBufferData, ._len = 0, ._begin = 0, ._end = 0
    };

    Log_Write( "SerialPortImpl_Init PASSED\r\n" );

    return TRUE;
}
//----------------------------------------------------------------------------
void ICACHE_FLASH_ATTR SerialPortImpl_DeInit( SerialPortImpl this ) {
    if( this == NULL )
        return;

    SerialPortImpl_Close( this );
}

//----------------------------------------------------------------------------
bool ICACHE_FLASH_ATTR SerialPortImpl_Open( SerialPortImpl this ) {
    if( this == NULL || this->_device < 0 )
        return FALSE;

    if( this->_isOpen )
        return TRUE;

    int device = this->_device;

    // Log_Write( "SerialPortImpl_Open BEGIN\r\n" );
    UART_RX_CLEAR( device );
    // Log_Write( "SerialPortImpl_Open UART_RX_CLEAR\r\n" );
    UART_TX_CLEAR( device );
    // Log_Write( "SerialPortImpl_Open UART_TX_CLEAR\r\n" );

    UART_TX_EI( device );
    // Log_Write( "SerialPortImpl_Open UART_TX_EI\r\n" );
    // UART_RX_EI( device );
    // Log_Write( "SerialPortImpl_Open UART_RX_EI\r\n" );

    ETS_UART_INTR_ENABLE();
    // Log_Write( "SerialPortImpl_Open ETS_UART_INTR_ENABLE\r\n" );

    return ( this->_isOpen = TRUE );
}
//----------------------------------------------------------------------------
bool ICACHE_FLASH_ATTR SerialPortImpl_Close( SerialPortImpl this ) {
    if( this == NULL || this->_device < 0 )
        return FALSE;

    if( !this->_isOpen )
        return TRUE;

    ETS_UART_INTR_DISABLE();

    int device = this->_device;
    UART_RX_DI( device );
    UART_TX_DI( device );

    CyclicBuffer_Clear( &this->_rxBuffer );
    CyclicBuffer_Clear( &this->_txBuffer );

    this->_isOpen = FALSE;

    return TRUE;
}
//----------------------------------------------------------------------------
bool ICACHE_FLASH_ATTR SerialPortImpl_IsOpen( SerialPortImpl this ) {
    return this->_isOpen;
}

//----------------------------------------------------------------------------
int ICACHE_FLASH_ATTR SerialPortImpl_Read( SerialPortImpl this, int lenToRead, uint8_t* buffer ) {
    if( this == NULL || this->_device < 0 || !this->_isOpen )
        return 0;

    ETS_UART_INTR_DISABLE();
    int readLen = CyclicBuffer_GetMem( &this->_rxBuffer, buffer, lenToRead );
    ETS_UART_INTR_ENABLE();

    // LogRawBuffer( "RX", lenToRead, readLen, buffer );

    return readLen;
}
//----------------------------------------------------------------------------
int ICACHE_FLASH_ATTR SerialPortImpl_WriteCount( SerialPortImpl this ) {
    if( this == NULL || this->_device < 0 || !this->_isOpen )
        return 0;

    ETS_UART_INTR_DISABLE();
    int count = this->_txBuffer._size - this->_txBuffer._len;
    ETS_UART_INTR_ENABLE();

    return count;
}
//----------------------------------------------------------------------------
int ICACHE_FLASH_ATTR SerialPortImpl_Write( SerialPortImpl this, int lenToWrite, uint8_t* buffer ) {
    if( this == NULL || !this->_isOpen )
        return 0;
    int device = this->_device;
    if( device < 0 )
        return 0;

    ETS_UART_INTR_DISABLE();
    CyclicBuffer txBuffer = &this->_txBuffer;
    int writtenLen = CyclicBuffer_PutMem( txBuffer, buffer, lenToWrite );
    if( writtenLen > 0 ) {
        if( UART_TX_LEN( device ) == 0 ) {
            uint8_t byte;
            if( CyclicBuffer_GetMem( txBuffer, &byte, 1 ) == 1 ) {
                UART_WRITE( device, byte );
                UART_TX_EI( device );
            }
        }
    }
    ETS_UART_INTR_ENABLE();

    // LogRawBuffer( "TX", lenToWrite, writtenLen, buffer );

    return writtenLen;
}

#endif  // ( ! defined(_WINDOWS) && ! defined(__linux__) && !defined( ESP_PLATFORM ) ) && !defined(__APPLE__)
