
//////////////////////////////////////////////////////////////////////////////

#include "serial_port.h"
#include <stdio.h>

//////////////////////////////////////////////////////////////////////////////

//----------------------------------------------------------------------------
bool ICACHE_FLASH_ATTR SerialPort_Init( SerialPort this,
                                        SerialPortIdentifier_t serialPortId,
                                        Handshakings_e handshaking,
                                        BaudRates_t baudRate,
                                        DataBits_e dataBits,
                                        bool parityEnabled,
                                        Parities_e parity,
                                        StopBits_e stopBits ) {
    return ( this &&
             SerialPortImpl_Init(
                 &this->_impl, serialPortId, handshaking, baudRate, dataBits, parityEnabled, parity, stopBits ) );
}
//----------------------------------------------------------------------------
void ICACHE_FLASH_ATTR SerialPort_DeInit( SerialPort this ) {
    if( this )
        SerialPortImpl_DeInit( &this->_impl );
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
bool ICACHE_FLASH_ATTR SerialPort_Open( SerialPort this ) {
    return ( this && SerialPortImpl_Open( &this->_impl ) );
}
//----------------------------------------------------------------------------
bool ICACHE_FLASH_ATTR SerialPort_Close( SerialPort this ) {
    return ( this && SerialPortImpl_Close( &this->_impl ) );
}
//----------------------------------------------------------------------------
bool ICACHE_FLASH_ATTR SerialPort_ReOpen( SerialPort this ) {
    if( this == NULL )
        return FALSE;
    if( SerialPortImpl_IsOpen( &this->_impl ) )
        SerialPortImpl_Close( &this->_impl );
    return SerialPortImpl_Open( &this->_impl );
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
/// lenToRead : <  0 - read all possible count of butes from UART's rx buffer (
/// <= buffer->_size )
///           : == 0 - return 0
///           : >  0 - read exactly as noted
int ICACHE_FLASH_ATTR SerialPort_ReadCyclicBuffer( SerialPort this, CyclicBuffer buffer ) {
    if( this == NULL )
        return 0;

    int readed = 0;
    while( !CyclicBuffer_IsFull( buffer ) ) {
        uint8_t byte;
        if( SerialPortImpl_Read( &this->_impl, 1, &byte ) != 1 ) {
            break;
        }
        CyclicBuffer_WriteByte( buffer, byte );
        ++readed;
    }

    return readed;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------

///----------------------------------------------------------------------------
/// lenToWrite : <  0 - write all possible count of bytes from UART's rx buffer
/// ( <= buffer->_size )
///            : == 0 - return 0
///            : >  0 - write exactly as noted
static int ICACHE_FLASH_ATTR SerialPort_CorrectLenToWrite( SerialPort this, int lenToWrite ) {
    if( lenToWrite < 0 )
        return 0;
    /// lenToWrite >= 0
    if( lenToWrite > SerialPortImpl_WriteCount( &this->_impl ) )
        return 0;
    return lenToWrite;
}

//----------------------------------------------------------------------------
int ICACHE_FLASH_ATTR SerialPort_WriteMem( SerialPort this, int lenToWrite, uint8_t* buffer ) {
    if( this == NULL || buffer == NULL )
        return 0;

    if( ( lenToWrite = SerialPort_CorrectLenToWrite( this, lenToWrite ) ) == 0 )
        return 0;
    int writtenLen = SerialPortImpl_Write( &this->_impl, lenToWrite, buffer );
    if( writtenLen != lenToWrite )
        return 0;
    return writtenLen;
}
//----------------------------------------------------------------------------
int ICACHE_FLASH_ATTR SerialPort_WriteBuffer( SerialPort this, int lenToWrite, Buffer buffer ) {
    if( this == NULL || buffer == NULL )
        return 0;
    uint8_t* bufferData = buffer->_data;
    if( bufferData == NULL )
        return 0;
    int buffSize = buffer->_size;
    if( buffSize <= 0 )
        return 0;
    int buffLen = buffer->_len;
    if( buffLen <= 0 )
        return 0;
    if( buffLen > buffSize )
        return 0;

    if( ( lenToWrite = SerialPort_CorrectLenToWrite( this, lenToWrite ) ) == 0 )
        return 0;
    int writtenLen = SerialPortImpl_Write( &this->_impl, lenToWrite, bufferData );
    if( writtenLen != lenToWrite )
        return 0;
    return writtenLen;
}
//----------------------------------------------------------------------------
int ICACHE_FLASH_ATTR SerialPort_WriteCyclicBuffer( SerialPort this, int lenToWrite, CyclicBuffer buffer ) {
    if( this == NULL || buffer == NULL )
        return 0;
    uint8_t* bufferData = buffer->_data;
    if( bufferData == NULL )
        return 0;
    int buffBegin = buffer->_begin;
    int buffEnd = buffer->_end;
    int buffSize = buffer->_size;
    if( buffBegin < 0 || buffBegin >= buffSize || buffEnd < 0 || buffEnd > buffSize )
        return 0;
    int buffLen = buffer->_len;
    if( buffLen <= 0 || buffLen > buffSize )
        return 0;
    if( lenToWrite > buffLen )
        return 0;

    if( ( lenToWrite = SerialPort_CorrectLenToWrite( this, lenToWrite ) ) == 0 )
        return 0;

    int len1, len2;
    if( buffEnd <= buffBegin ) {
        len1 = buffSize - buffBegin;
        if( len1 > lenToWrite )
            len1 = lenToWrite;
        lenToWrite -= len1;
        len2 = buffEnd;
        if( len2 > lenToWrite )
            len2 = lenToWrite;
        lenToWrite += len1;
    }
    else {
        len1 = lenToWrite;
        len2 = 0;
    }

    if( len1 > 0 ) {
        if( SerialPortImpl_Write( &this->_impl, len1, bufferData + buffBegin ) != len1 )
            return 0;
        buffBegin = ( buffBegin + len1 ) % buffSize;
    }

    if( len2 > 0 ) {
        if( SerialPortImpl_Write( &this->_impl, len2, bufferData + buffBegin ) != len2 )
            return 0;
        buffBegin = ( buffBegin + len2 ) % buffSize;
    }

    buffer->_begin = buffBegin;
    buffer->_len -= lenToWrite;

    return lenToWrite;
}
