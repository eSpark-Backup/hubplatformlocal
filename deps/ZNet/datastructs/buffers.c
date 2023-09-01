//============================================================================

#include <stdlib.h>
#include <string.h>

#include "buffers.h"

//============================================================================

//----------------------------------------------------------------------------
int ICACHE_FLASH_ATTR Buffer_PutMem( Buffer buff, const uint8_t* p, int len ) {
    if( buff == NULL || p == NULL || len < 0 || buff->_size < len )
        return 0;

    if( len )
        memcpy( buff->_data, p, ( size_t )( buff->_len = len ) );
    return len;
}

//----------------------------------------------------------------------------
int ICACHE_FLASH_ATTR Buffer_GetMem( const Buffer buff, uint8_t* p, int size ) {
    if( buff == NULL || p == NULL || size < buff->_len )
        return 0;

    int len = buff->_len;
    if( len )
        memcpy( p, buff->_data, (size_t)len );
    return len;
}

//----------------------------------------------------------------------------
int ICACHE_FLASH_ATTR Buffer_Copy( Buffer dst, const Buffer src ) {
    return Buffer_PutMem( dst, src->_data, src->_len );
}

//----------------------------------------------------------------------------
void ICACHE_FLASH_ATTR Buffer_Clear( Buffer buff ) {
    if( buff == NULL )
        return;

    buff->_len = 0;
}
//----------------------------------------------------------------------------
bool ICACHE_FLASH_ATTR Buffer_IsEmpty( const Buffer buff ) {
    return ( buff == NULL || buff->_len == 0 );
}
//----------------------------------------------------------------------------
bool ICACHE_FLASH_ATTR Buffer_IsFull( const Buffer buff ) {
    return ( buff == NULL || buff->_len >= buff->_size );
}

//============================================================================

//----------------------------------------------------------------------------
int CyclicBuffer_PutMem( CyclicBuffer buff, const uint8_t* p, int len ) {
    if( buff == NULL || p == NULL || len < 0 )
        return 0;

    int count = buff->_size - buff->_len;
    if( count > len )
        count = len;
    len = count;
    while( count-- ) {
        buff->_data[buff->_end++] = *p++;
        if( buff->_end == buff->_size )
            buff->_end = 0;
    }
    buff->_len += len;
    return len;
}

//----------------------------------------------------------------------------
int ICACHE_FLASH_ATTR CyclicBuffer_PutBuff( CyclicBuffer cyclicBuff, const Buffer buff ) {
    return CyclicBuffer_PutMem( cyclicBuff, buff->_data, buff->_len );
}

//----------------------------------------------------------------------------
int CyclicBuffer_GetMem( CyclicBuffer buff, uint8_t* p, int size ) {
    int readed = 0;
    while( readed < size ) {
        int byte = CyclicBuffer_ReadByte( buff );
        if( 0 > byte ) {
            break;
        }
        p[readed] = byte;
        ++readed;
    }
    return readed;
}

//----------------------------------------------------------------------------
int ICACHE_FLASH_ATTR CyclicBuffer_GetBuff( CyclicBuffer cyclicBuff, Buffer buff ) {
    return ( buff->_len = CyclicBuffer_GetMem( cyclicBuff, buff->_data, buff->_size ) );
}

//----------------------------------------------------------------------------
int ICACHE_FLASH_ATTR CyclicBuffer_Move( CyclicBuffer dst, CyclicBuffer src ) {
    if( dst == NULL || src == NULL )
        return 0;

    int size = dst->_size - dst->_len;
    int len = src->_len;
    if( len > size )
        len = size;
    size = len;
    while( size-- ) {
        dst->_data[dst->_end++] = src->_data[src->_begin++];
        if( dst->_end == dst->_size )
            dst->_end = 0;
        if( src->_begin == src->_size )
            src->_begin = 0;
    }
    dst->_len += len;
    src->_len -= len;
    return len;
}

//----------------------------------------------------------------------------
void ICACHE_FLASH_ATTR CyclicBuffer_Clear( CyclicBuffer cyclicBuff ) {
    cyclicBuff->_len = 0;
    cyclicBuff->_end = 0;
    cyclicBuff->_begin = 0;
}

//----------------------------------------------------------------------------
bool ICACHE_FLASH_ATTR CyclicBuffer_IsEmpty( const CyclicBuffer cyclicBuff ) {
    if( cyclicBuff == NULL )
        return true;
    if( cyclicBuff->_len == 0 )
        return true;
    return false;
}
//----------------------------------------------------------------------------

bool ICACHE_FLASH_ATTR CyclicBuffer_IsFull( const CyclicBuffer cyclicBuff ) {
    if( cyclicBuff == NULL )
        return true;
    if( cyclicBuff->_len >= cyclicBuff->_size )
        return true;
    return false;
}

int ICACHE_FLASH_ATTR CyclicBuffer_NumBytes( const CyclicBuffer cyclicBuff ) {
    return cyclicBuff->_len;
}

int ICACHE_FLASH_ATTR CyclicBuffer_FreeSize( const CyclicBuffer cyclicBuff ) {
    return cyclicBuff->_size - cyclicBuff->_len;
}

int ICACHE_FLASH_ATTR CyclicBuffer_PeekByte( const CyclicBuffer cyclicBuff, const int index ) {
    if( index >= CyclicBuffer_NumBytes( cyclicBuff ) ) {
        return -1;
    }

    return cyclicBuff->_data[( cyclicBuff->_begin + index ) % cyclicBuff->_size];
}

int ICACHE_FLASH_ATTR CyclicBuffer_ReadByte( CyclicBuffer cyclicBuff ) {
    if( CyclicBuffer_IsEmpty( cyclicBuff ) ) {
        return -1;
    }

    int val = cyclicBuff->_data[cyclicBuff->_begin];
    cyclicBuff->_begin = ( cyclicBuff->_begin + 1 ) % cyclicBuff->_size;
    cyclicBuff->_len -= 1;
    return val;
}

int ICACHE_FLASH_ATTR CyclicBuffer_WriteByte( CyclicBuffer cyclicBuff, int byte ) {
    if( CyclicBuffer_IsFull( cyclicBuff ) ) {
        return 0;
    }
    cyclicBuff->_data[cyclicBuff->_end] = byte;
    cyclicBuff->_len += 1;
    cyclicBuff->_end = ( cyclicBuff->_end + 1 ) % cyclicBuff->_size;
    return 1;
}
