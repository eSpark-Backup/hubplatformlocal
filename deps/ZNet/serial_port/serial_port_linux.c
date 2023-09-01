/// INFO: for esp32 and linux use callback functions
#if( 0 && ( defined( __APPLE__ ) || defined( __linux__ ) ) )

//////////////////////////////////////////////////////////////////////////////

#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <termios.h>

//#include <asm-generic/ioctls.h>
//#include <asm-generic/fcntl.h>
//#include <asm-generic/termbits.h>

#include "logger.h"

#include "serial_port_impl.h"
#include "serial_port_linux.h"

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

    this->_device = -1;

    this->_baudRate = baudRate;
    this->_parityEnabled = parityEnabled;
    this->_parity = parity;
    this->_dataBits = dataBits;
    this->_stopBits = stopBits;
    this->_handshaking = handshaking;

    //    this->_rxBuffer = ( CyclicBuffer_t ){ ._data = this->_rxBufferData,
    //                                          ._size = sizeof this->_rxBufferData,
    //                                          ._len = 0,
    //                                          ._begin = 0,
    //                                          ._end = 0 };
    //    this->_txBuffer = ( CyclicBuffer_t ){ ._data = this->_txBufferData,
    //                                          ._size = sizeof this->_txBufferData,
    //                                          ._len = 0,
    //                                          ._begin = 0,
    //                                          ._end = 0 };

    //    if( Activity_Init( &this->_txActivity, this, &SerialPortTXActivityFunc,
    //                       NULL ) != ActivityErrors_NoError )
    //        return FALSE;
    //    if( Activity_Init( &this->_rxActivity, this, &SerialPortRXActivityFunc,
    //                       NULL ) != ActivityErrors_NoError )
    //    {
    //        Activity_Init( &this->_txActivity );
    //        return FALSE;
    //    }

    return TRUE;
}
//----------------------------------------------------------------------------
void ICACHE_FLASH_ATTR SerialPortImpl_DeInit( SerialPortImpl this ) {
    if( this == NULL )
        return;

    SerialPortImpl_Close( this );

    //    Activity_DeInit( &this->_rxActivity );
    //    Activity_DeInit( &this->_txActivity );
}

//------------------------------------------------------------------------
static tcflag_t ICACHE_FLASH_ATTR SerialPort_DataBits_ConvertToLinux( DataBits_e dataBits ) {
    switch( dataBits ) {
    case DataBits_5:
        return CS5;
    case DataBits_6:
        return CS6;
    case DataBits_7:
        return CS7;
    case DataBits_8:
        return CS8;
    }
    return CS8;
}

//----------------------------------------------------------------------------
bool ICACHE_FLASH_ATTR SerialPortImpl_Open( SerialPortImpl this ) {
    if( this == NULL )
        return FALSE;

    if( this->_device >= 0 )
        return TRUE;

    if( ( this->_device = open( this->_serialPortId,
                                /*O_BINARY  TODO !!! |*/ O_RDWR | O_NOCTTY | O_CLOEXEC ) ) == -1 ) {
        this->_device = -1;
        return FALSE;
    }

    //    if( flock( this->_device, LOCK_EX | LOCK_NB ) == -1 )
    //    {
    //        close( this->_device );
    //        return FALSE;
    //    }
    if( tcflush( this->_device, TCIOFLUSH ) ) { /*znet_log(znet, Warning, "Failed to flush device: %s\r\n", e);*/
    }

    struct termios cfg;
    memset( &cfg, 0, sizeof( cfg ) );
    tcgetattr( this->_device, &cfg );
    cfg.c_cflag = CLOCAL | CREAD | SerialPort_DataBits_ConvertToLinux( this->_dataBits );
    cfg.c_oflag = 0;
    cfg.c_lflag = 0;  // ICANON;
    cfg.c_cc[VMIN] = 1;
    cfg.c_cc[VTIME] = 0;
    cfsetspeed( &cfg, this->_baudRate /*B115200*/ );
    cfg.c_iflag = IGNPAR;
    cfmakeraw( &cfg );
    if( tcsetattr( this->_device,
                   TCSAFLUSH,
                   &cfg ) ) { /*znet_log(znet, Warning, "Failed to alter device settings: %s\r\n", e);*/
    }

    //    Activity

    return TRUE;
}
//----------------------------------------------------------------------------
bool ICACHE_FLASH_ATTR SerialPortImpl_Close( SerialPortImpl this ) {
    if( this == NULL )
        return FALSE;

    if( this->_device < 0 )
        return TRUE;

    //    flock( this->_device, LOCK_UN );
    close( this->_device );
    this->_device = -1;

    return TRUE;
}
//----------------------------------------------------------------------------
bool ICACHE_FLASH_ATTR SerialPortImpl_IsOpen( SerialPortImpl this ) {
    return ( this->_device > 0 );
}

//----------------------------------------------------------------------------
int ICACHE_FLASH_ATTR SerialPortImpl_Read( SerialPortImpl this, int lenToRead, uint8_t* buffer ) {
    if( this == NULL || this->_device < 0 )
        return 0;

    int readLen = read( this->_device, buffer, lenToRead );
    if( readLen <= 0 )
        return 0;

    LogRawBuffer( "RX", lenToRead, readLen, buffer );

    return lenToRead;
}
//----------------------------------------------------------------------------
int ICACHE_FLASH_ATTR SerialPortImpl_WriteCount( SerialPortImpl this ) {
    if( this == NULL || this->_device < 0 )
        return 0;
    return 512;
}
//----------------------------------------------------------------------------
int ICACHE_FLASH_ATTR SerialPortImpl_Write( SerialPortImpl this, int lenToWrite, uint8_t* buffer ) {
    if( this == NULL || this->_device < 0 )
        return 0;

    int writtenCount = write( this->_device, buffer, lenToWrite );
    if( writtenCount <= 0 )
        return 0;

    LogRawBuffer( "TX", lenToWrite, writtenCount, buffer );

    return writtenCount;
}

#endif  //  #if ( defined(__linux__) )
