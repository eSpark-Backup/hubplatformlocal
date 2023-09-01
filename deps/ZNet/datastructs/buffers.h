#ifndef __BUFFERS_H__
#define __BUFFERS_H__

//----------------------------------------------------------------------------
#include "typedefs.h"

//----------------------------------------------------------------------------
typedef struct _Buffer {
    uint8_t* _data;
    int _size;
    int _len;

} Buffer_t;
// typedef Buffer_t* Buffer;
#define Buffer Buffer_t*

//----------------------------------------------------------------------------
typedef struct _CyclicBuffer {
    uint8_t* _data;
    int _size;
    int _len;
    int _begin;
    int _end;

} CyclicBuffer_t;
// typedef CyclicBuffer_t* CyclicBuffer;
#define CyclicBuffer CyclicBuffer_t*

//----------------------------------------------------------------------------
int ICACHE_FLASH_ATTR Buffer_PutMem( Buffer buff, const uint8_t* p, int len );
int ICACHE_FLASH_ATTR Buffer_GetMem( const Buffer buff, uint8_t* p, int size );

int ICACHE_FLASH_ATTR Buffer_Copy( Buffer dst, const Buffer src );

void ICACHE_FLASH_ATTR Buffer_Clear( Buffer buff );

bool ICACHE_FLASH_ATTR Buffer_IsEmpty( const Buffer buff );
bool ICACHE_FLASH_ATTR Buffer_IsFull( const Buffer buff );

//----------------------------------------------------------------------------
int CyclicBuffer_PutMem( CyclicBuffer cyclicBuff, const uint8_t* p, int len );
int ICACHE_FLASH_ATTR CyclicBuffer_PutBuff( CyclicBuffer cyclicBuff, const Buffer buff );

int CyclicBuffer_GetMem( CyclicBuffer cyclicBuff, uint8_t* p, int size );
int ICACHE_FLASH_ATTR CyclicBuffer_GetBuff( CyclicBuffer cyclicBuff, Buffer buff );

int ICACHE_FLASH_ATTR CyclicBuffer_Move( CyclicBuffer dst, CyclicBuffer src );

#define CYCLICBUFFER_MOVE( dst, src )                                        \
    do {                                                                     \
        while( ( ( dst )->_size - ( dst )->_len ) && ( src )->_len ) {       \
            ( dst )->_data[( dst )->_end] = ( src )->_data[( src )->_begin]; \
            if( ++( ( dst )->_end ) == ( dst )->_size )                      \
                ( dst )->_end = 0;                                           \
            ++( ( dst )->_len );                                             \
            if( ++( ( src )->_begin ) == ( src )->_size )                    \
                ( src )->_begin = 0;                                         \
            --( ( src )->_len );                                             \
        }                                                                    \
    } while( 0 )

void ICACHE_FLASH_ATTR CyclicBuffer_Clear( CyclicBuffer cyclicBuff );

bool ICACHE_FLASH_ATTR CyclicBuffer_IsEmpty( const CyclicBuffer cyclicBuff );
bool ICACHE_FLASH_ATTR CyclicBuffer_IsFull( const CyclicBuffer cyclicBuff );
int ICACHE_FLASH_ATTR CyclicBuffer_NumBytes( const CyclicBuffer cyclicBuff );
int ICACHE_FLASH_ATTR CyclicBuffer_FreeSize( const CyclicBuffer cyclicBuff );
int ICACHE_FLASH_ATTR CyclicBuffer_PeekByte( const CyclicBuffer cyclicBuff, const int index );
int ICACHE_FLASH_ATTR CyclicBuffer_ReadByte( CyclicBuffer cyclicBuff );
int ICACHE_FLASH_ATTR CyclicBuffer_WriteByte( CyclicBuffer cyclicBuff, int byte );

//============================================================================

#endif  // __BUFFERS_H__
