
#if( defined( _WINDOWS ) )

//////////////////////////////////////////////////////////////////////////////

#include "logger.h"

#include "serial_port_impl.h"
#include "serial_port_windows.h"

//////////////////////////////////////////////////////////////////////////////

//----------------------------------------------------------------------------
bool ICACHE_FLASH_ATTR SerialPortImpl_Init( SerialPortImpl this,
                                            SerialPortIdentifier_t serialPortId,
                                            Handshakings_e handshaking,
                                            BaudRates_t baudRate,
                                            DataBits_e dataBits,
                                            bool parityEnabled,
                                            Parities_e parity,
                                            StopBits_e stopBits ) {
    if( this == NULL || serialPortId == NULL || serialPortId[0] == 0 )
        return FALSE;

    this->_serialPortId = serialPortId;

    this->_device = ( HANDLE )( -1 );

    this->_baudRate = baudRate;
    this->_parityEnabled = parityEnabled;
    this->_parity = parity;
    this->_dataBits = dataBits;
    this->_stopBits = stopBits;
    this->_handshaking = handshaking;

    // this->_rxBuffer = (CyclicBuffer_t){ ._data = this->_rxBufferData, ._size
    // = sizeof this->_rxBufferData, ._len = 0, ._begin = 0, ._end = 0 };
    // this->_txBuffer = (CyclicBuffer_t){ ._data = this->_txBufferData, ._size
    // = sizeof this->_txBufferData, ._len = 0, ._begin = 0, ._end = 0 };

    // if( Activity_Init( &this->_txActivity, this, &SerialPortTXActivityFunc,
    // NULL ) != ActivityErrors_NoError ) return FALSE;  if( Activity_Init(
    // &this->_rxActivity, this, &SerialPortRXActivityFunc, NULL ) !=
    // ActivityErrors_NoError ) { Activity_Init( &this->_txActivity ); return
    // FALSE; }

    return TRUE;
}
//----------------------------------------------------------------------------

// bool ICACHE_FLASH_ATTR SerialPortImpl_Close( SerialPortImpl this );

void ICACHE_FLASH_ATTR SerialPortImpl_DeInit( SerialPortImpl this ) {
    if( this == NULL )
        return;

    SerialPortImpl_Close( this );

    // Activity_DeInit( &this->_rxActivity );
    // Activity_DeInit( &this->_txActivity );
}

//------------------------------------------------------------------------
static DWORD ICACHE_FLASH_ATTR SerialPort_Parity_ConvertToWindows( Parities_e parity ) {
    switch( parity ) {
    case Parities_None:
        return NOPARITY;
    case Parities_Odd:
        return ODDPARITY;
    case Parities_Even:
        return EVENPARITY;
    case Parities_Mark:
        return MARKPARITY;
    case Parities_Space:
        return SPACEPARITY;
    }
    return NOPARITY;
}

//------------------------------------------------------------------------
static BYTE ICACHE_FLASH_ATTR SerialPort_StopBits_ConvertToWindows( StopBits_e stopBits ) {
    switch( stopBits ) {
    case StopBits_1:
        return ONESTOPBIT;
    case StopBits_15:
        return ONE5STOPBITS;
    case StopBits_2:
        return TWOSTOPBITS;
    }
    return ONESTOPBIT;
}

//----------------------------------------------------------------------------
bool ICACHE_FLASH_ATTR SerialPortImpl_Open( SerialPortImpl this ) {
    if( this == NULL )
        return FALSE;

    if( (int)this->_device >= 0 )
        return TRUE;

    if( ( this->_device = CreateFile( this->_serialPortId,
                                      GENERIC_READ | GENERIC_WRITE,
                                      0,
                                      NULL,
                                      OPEN_EXISTING,
                                      FILE_ATTRIBUTE_DEVICE,
                                      NULL ) ) == INVALID_HANDLE_VALUE ) {
        this->_device = ( HANDLE )( -1 );
        return FALSE;
    }

    if( !PurgeComm( this->_device,
                    PURGE_RXABORT | PURGE_RXCLEAR | PURGE_TXABORT |
                        PURGE_TXCLEAR ) ) { /*znet_log(znet, Warning, "Failed to flush device: %s\r\n", e);*/
    }

    struct _DCB cfg;
    memset( &cfg, 0, sizeof( cfg ) );
    cfg.DCBlength = sizeof( cfg );
    if( GetCommState( this->_device, &cfg ) ) {
        cfg.fRtsControl = RTS_CONTROL_DISABLE;  // handshaking
        cfg.fInX = FALSE;                       // handshaking
        cfg.fOutX = FALSE;                      // handshaking
        cfg.BaudRate = this->_baudRate;         // CBR_115200;
        cfg.ByteSize = this->_dataBits;         // 8;
        cfg.fParity = ( this->_parityEnabled ) ? SerialPort_Parity_ConvertToWindows( this->_parity ) : (DWORD)NOPARITY;
        cfg.StopBits = SerialPort_StopBits_ConvertToWindows( this->_stopBits );  // ONESTOPBIT;
        if( !SetCommState( this->_device, &cfg ) ) { /*znet_log(znet, Warning, "Failed to alter device settings:
                                                        %d\r\n", GetLastError());*/
        }
    }
    else { /*znet_log(znet, Warning, "Failed to get device settings: %d\r\n",
              GetLastError());*/
    }

    return TRUE;
}

//----------------------------------------------------------------------------
bool ICACHE_FLASH_ATTR SerialPortImpl_Close( SerialPortImpl this ) {
    if( this == NULL )
        return FALSE;

    if( (int)this->_device < 0 )
        return TRUE;

    CloseHandle( this->_device );
    this->_device = ( HANDLE )( -1 );

    return TRUE;
}
//----------------------------------------------------------------------------
bool ICACHE_FLASH_ATTR SerialPortImpl_IsOpen( SerialPortImpl this ) {
    return ( (int)this->_device > 0 );
}
//----------------------------------------------------------------------------
int ICACHE_FLASH_ATTR SerialPortImpl_Read( SerialPortImpl this, int lenToRead, uint8_t* buffer ) {
    if( this == NULL || (int)this->_device < 0 )
        return 0;

    DWORD readLen = 0;
    if( !ReadFile( this->_device, buffer, (DWORD)lenToRead, &readLen, NULL ) )
        return 0;

    // LogRawBuffer( "RX", lenToRead, (int)readLen, buffer );

    return (int)readLen;
}
//----------------------------------------------------------------------------
int ICACHE_FLASH_ATTR SerialPortImpl_WriteCount( SerialPortImpl this ) {
    if( this == NULL || (int)this->_device < 0 )
        return 0;
    return 512;
}
//----------------------------------------------------------------------------
int ICACHE_FLASH_ATTR SerialPortImpl_Write( SerialPortImpl this, int lenToWrite, uint8_t* buffer ) {
    if( this == NULL || (int)this->_device < 0 )
        return 0;

    DWORD writtenCount;
    if( !WriteFile( this->_device, buffer, (DWORD)lenToWrite, &writtenCount, NULL ) )
        return 0;

    // LogRawBuffer( "TX", lenToWrite, (int)writtenCount, buffer );

    return (int)writtenCount;
}

#endif  // #if ( defined(_WINDOWS) )
