
//////////////////////////////////////////////////////////////////////////////

#include "logger.h"

#include "serial_port_impl.h"
#include "buffers.h"
#include "ZPlatform.h"

#include "ZFunctionClass.h"

/// INFO: for esp32 and linux use callback functions
#if( defined( __APPLE__ ) || defined( __linux__ ) || defined( ESP_PLATFORM ) )
#include "znet_uart.h"
#endif

/// INFO: for esp32 and linux use callback functions
#if( defined( __APPLE__ ) || defined( __linux__ ) || defined( ESP_PLATFORM ) )

bool ICACHE_FLASH_ATTR SerialPortImpl_Init( SerialPortImpl seriapPortImpl,
                                            SerialPortIdentifier_t serialPortId,
                                            Handshakings_e handshaking,
                                            BaudRates_t baudRate,
                                            DataBits_e dataBits,
                                            bool parityEnabled,
                                            Parities_e parity,
                                            StopBits_e stopBits ) {
    if( seriapPortImpl == NULL )
        return FALSE;

    if( seriapPortImpl->_isInit )
        SerialPortImpl_DeInit( seriapPortImpl );

    return ( seriapPortImpl->_isInit = !0 );
    return TRUE;
}

void ICACHE_FLASH_ATTR SerialPortImpl_DeInit( SerialPortImpl serialPortImpl ) {
    if( serialPortImpl == NULL || !serialPortImpl->_isInit )
        return;

    if( serialPortImpl->_isOpen )
        SerialPortImpl_Close( serialPortImpl );

    serialPortImpl->_isInit = 0;
}

bool ICACHE_FLASH_ATTR SerialPortImpl_Open( SerialPortImpl serialPortImpl ) {
    if( serialPortImpl == NULL || !serialPortImpl->_isInit )
        return FALSE;

    if( serialPortImpl->_isOpen )
        return TRUE;

    /// TODO: znet_uart_reset ?

    serialPortImpl->_isOpen = !0;
    return TRUE;
}

bool ICACHE_FLASH_ATTR SerialPortImpl_Close( SerialPortImpl serialPortImpl ) {
    if( serialPortImpl == NULL || !serialPortImpl->_isInit || !serialPortImpl->_isOpen )
        return FALSE;

    serialPortImpl->_isOpen = FALSE;

    return TRUE;
}

bool ICACHE_FLASH_ATTR SerialPortImpl_IsOpen( SerialPortImpl serialPortImpl ) {
    if( serialPortImpl->_isOpen )
        return TRUE;

    return FALSE;
}

int ICACHE_FLASH_ATTR SerialPortImpl_Read( SerialPortImpl serialPortImpl, int lenToRead, uint8_t* buffer ) {
    size_t count;
    if( znet_uart_read( buffer, lenToRead, &count ) )
        return 0;

    return (int)count;
}

int ICACHE_FLASH_ATTR SerialPortImpl_WriteCount( SerialPortImpl serialPortImpl ) {
    size_t count;
    if( znet_uart_write( NULL, 0, &count ) )
        return 0;

    return (int)count;
}

int ICACHE_FLASH_ATTR SerialPortImpl_Write( SerialPortImpl serialPortImpl, int lenToWrite, uint8_t* buffer ) {
    size_t count;
    if( znet_uart_write( buffer, lenToWrite, &count ) )
        return 0;

    return (int)count;
}

#endif

//////////////////////////////////////////////////////////////////////////////

//----------------------------------------------------------------------------
void ICACHE_FLASH_ATTR LogRawBuffer( const char* prefix, int orderedLen, int actualLen, uint8_t* buffer ) {
    Log_Write( "[%s: %d/%d :", prefix, actualLen, orderedLen );
    for( int i = 0; i < actualLen; ++i )
        Log_Write( " %02X", buffer[i] );
    Log_Write( " ]\r\n" );
}

//----------------------------------------------------------------------------
void ICACHE_FLASH_ATTR LogBuffer( const char* prefix, Buffer buff, int len ) {
    uint8_t* data = buff->_data;
    if( len > buff->_len )
        len = buff->_len;
    uint8_t* dataEnd = data + len;

    if( prefix[0] == '>' )
        Log_Write( ">>>" );
    if( prefix[0] == '<' )
        Log_Write( "<<<" );

    /*0*/
    if( data < dataEnd ) {
        uint8_t b = *data++;
        Log_Write( " <%02x:%s>",
                   b,
                   ( ( b == 0x01 ) ?
                         "SOF" :
                         ( ( b == 0x06 ) ? "ACK" : ( ( b == 0x15 ) ? "NAK" : ( ( b == 0x18 ) ? "CAN" : "???" ) ) ) ) );
        /*1*/
        if( data < dataEnd ) {
            b = *data++;
            Log_Write( " {len:%02x|%03d}", b, b );
            /*2*/
            if( data < dataEnd ) {
                b = *data++;
                Log_Write( " (%02x:%s)", b, ( ( b ) ? ( ( b == 1 ) ? "RES" : "???" ) : "REQ" ) );
                /*3*/
                if( data < dataEnd ) {
                    b = *data++;
                    const ZFunctionClass fc = ZFunctionClass_GetById( b );
                    Log_Write( " [%02x:%s]", b, ( ( fc ) ? fc->_name : "fc???" ) );
                    /*4++++*/
                    while( data < dataEnd - 1 ) {
                        Log_Write( " %02x", *data++ );
                    }
                    /*CRC:len-1*/
                    if( data < dataEnd )
                        Log_Write( " [CRC:%02x]", *data );
                }
            }
        }
    }
    Log_Write( "\r\n" );
}

//----------------------------------------------------------------------------
void ICACHE_FLASH_ATTR LogCyclicBuffer( const char* prefix, CyclicBuffer buff, int len ) {
    if( 0 < len ) {
        uint8_t type = *( buff->_data + buff->_begin );
        CyclicBuffer_t buff2 = *buff;
        buff2._len = buff->_len - 1;
        buff2._begin = ( buff->_begin + 1 ) % buff->_size;

        LogCyclicBuffer2( prefix, type, &buff2, len - 1 );
    }
}

void ICACHE_FLASH_ATTR LogCyclicBuffer2( const char* prefix, const uint8_t type, CyclicBuffer buff, int len ) {
    uint8_t* data = buff->_data;
    int begin = buff->_begin;
    int size = buff->_size;
    if( len > buff->_len )
        len = buff->_len;

    if( prefix[0] == '>' )
        Log_Write( ">>>" );
    if( prefix[0] == '<' )
        Log_Write( "<<<" );

    Log_Write( " (%d) ", (uint32_t)SystemTime_Decode_ms( SystemTime_Now() ) );

    Log_Write(
        " <%02x:%s>",
        type,
        ( ( type == 0x01 ) ?
              "SOF" :
              ( ( type == 0x06 ) ? "ACK" : ( ( type == 0x15 ) ? "NAK" : ( ( type == 0x18 ) ? "CAN" : "???" ) ) ) ) );

    int i = 0;
    if( i < len ) {
        uint8_t b = *( data + ( ( begin + i ) % size ) );
        ++i;
        Log_Write( " {len:%02x|%03d}", b, b );

        if( i < len ) {
            b = *( data + ( ( begin + i ) % size ) );
            ++i;
            Log_Write( " (%02x:%s)", b, ( ( b ) ? ( ( b == 1 ) ? "RES" : "???" ) : "REQ" ) );
            /*3*/
            if( i < len ) {
                b = *( data + ( ( begin + i ) % size ) );
                ++i;
                const ZFunctionClass fc = ZFunctionClass_GetById( b );
                Log_Write( " [%02x:%s]", b, ( ( fc ) ? fc->_name : "fc???" ) );
                while( i < len - 1 ) {
                    Log_Write( " %02x", *( data + ( ( begin + i ) % size ) ) );
                    ++i;
                }
                if( i < len )
                    Log_Write( " [CRC:%02x]", *( data + ( ( begin + i ) % size ) ) );
            }
        }
    }

    Log_Write( "\r\n" );
}
