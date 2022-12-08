#include "heap_cfg.h"

#ifdef USE_ORIGINAL_HEAP_MANAGER

//----------------------------------------------------------------------------

//#include <stdlib.h>
#if( defined( __APPLE__ ) || defined( _WINDOWS ) || defined( __linux__ ) )
#include <stdio.h>
#endif
#include <string.h>

//#include "multitasking.h"
//#include "locks.h"
#include "heap.h"
#include "logger.h"

//----------------------------------------------------------------------------
static uint8_t heapBytes[HEAP_SIZE];

//----------------------------------------------------------------------------
struct _BlockHeader;
typedef struct _BlockHeader* BlockHeader;

typedef struct _BlockHeader {
    BlockHeader _nextByAddr;
    BlockHeader _nextBySize;
    int _size;

} BlockHeader_t;

static BlockHeader firstByAddr;
static BlockHeader firstBySize;

#define HEADER_SIZE_REM ( (int)( sizeof( BlockHeader_t ) % DATA_ALIGNMENT ) )
#define HEADER_REAL_SIZE \
    ( (int)sizeof( BlockHeader_t ) + ( HEADER_SIZE_REM ? ( DATA_ALIGNMENT - HEADER_SIZE_REM ) : 0 ) )

#define MIN_DATA_SIZE ( sizeof( BlockHeader ) )

#define MIN_BLOCK_SIZE ( HEADER_REAL_SIZE + MIN_DATA_SIZE )

#define BlockHeader_Child( blockHeader ) ( *(BlockHeader*)( (uint8_t*)( blockHeader ) + HEADER_REAL_SIZE ) )

//----------------------------------------------------------------------------
static bool ICACHE_FLASH_ATTR IsBlockHeaderValid( BlockHeader blockHeader ) {
    uint8_t* highestValidAddr = heapBytes + HEAP_SIZE - MIN_BLOCK_SIZE;
    if( ( (uint8_t*)blockHeader < heapBytes ) || ( (uint8_t*)blockHeader > highestValidAddr ) ||
        ( (int)blockHeader % HEADER_ALIGNMENT ) )
        return FALSE;
    return TRUE;
}

//----------------------------------------------------------------------------
static bool ICACHE_FLASH_ATTR IncludeIntoSizes( BlockHeader prev, BlockHeader blockHeader ) {
    BlockHeader curr = ( prev ) ? prev->_nextBySize : firstBySize;
    while( curr && curr->_size < blockHeader->_size ) {
        prev = curr;
        curr = curr->_nextBySize;
    }

    if( curr == NULL ) {
        blockHeader->_nextBySize = NULL;
        if( prev )
            prev->_nextBySize = blockHeader;
        else
            firstBySize = blockHeader;
        BlockHeader_Child( blockHeader ) = NULL;
    }
    else {  // curr && curr->_size >= blockHeader->_size
        if( curr->_size > blockHeader->_size ) {
            blockHeader->_nextBySize = curr;
            if( prev )
                prev->_nextBySize = blockHeader;
            else
                firstBySize = blockHeader;
            BlockHeader_Child( blockHeader ) = NULL;
        }
        else {  // curr && curr->_size == blockHeader->_size

            BlockHeader upper = NULL;
            while( curr && curr < blockHeader ) {
                upper = curr;
                curr = BlockHeader_Child( curr );
            }

            if( curr == NULL )
                BlockHeader_Child( upper ) = blockHeader;
            else {  // curr >= blockHeader
                if( curr == blockHeader )
                    return FALSE;  // trying to include THE SAME BLOCK !!!
                                   // CATASTROPHY !!!
                // curr > blockHeader
                if( upper )
                    BlockHeader_Child( upper ) = blockHeader;
                else {
                    blockHeader->_nextBySize = curr->_nextBySize;
                    if( prev )
                        prev->_nextBySize = blockHeader;
                    else
                        firstBySize = blockHeader;
                }
            }
            BlockHeader_Child( blockHeader ) = curr;
        }
    }

    return TRUE;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
static BlockHeader ICACHE_FLASH_ATTR Alloc_FindAndExcludeFromSizes( int size, BlockHeader* prevBySizePtr ) {
    BlockHeader prev = NULL;
    BlockHeader curr = firstBySize;
    while( curr && curr->_size < size ) {
        prev = curr;
        curr = curr->_nextBySize;
    }

    if( curr ) {
        BlockHeader next = BlockHeader_Child( curr );
        if( next )
            next->_nextBySize = curr->_nextBySize;
        else
            next = curr->_nextBySize;

        if( ( *prevBySizePtr = prev ) )
            prev->_nextBySize = next;
        else
            firstBySize = next;
    }

    return curr;
}
//----------------------------------------------------------------------------
static bool ICACHE_FLASH_ATTR Alloc_FindAndExcludeFromAddrs( BlockHeader blockHeader, BlockHeader* prevByAddrPtr ) {
    BlockHeader prev = NULL;
    BlockHeader curr = firstByAddr;
    while( curr ) {
        BlockHeader next = curr->_nextByAddr;
        if( curr == blockHeader ) {
            if( ( *prevByAddrPtr = prev ) )
                prev->_nextByAddr = next;
            else
                firstByAddr = next;
            return TRUE;
        }
        prev = curr;
        curr = next;
    }
    return FALSE;
}

//----------------------------------------------------------------------------
void* ICACHE_FLASH_ATTR Heap_Alloc( int size )  // returns NULL if fault
{
    if( size <= 0 )
        return NULL;
    // correct size with its end to be aligned to HEADER_ALIGNMENT;
    int rem = size % HEADER_ALIGNMENT;
    if( rem )
        size += ( HEADER_ALIGNMENT - rem );

    BlockHeader prevBySize, blockHeader;
    if( ( blockHeader = Alloc_FindAndExcludeFromSizes( size, &prevBySize ) ) == NULL )
        return NULL;

    BlockHeader prevByAddr;
    if( !Alloc_FindAndExcludeFromAddrs( blockHeader, &prevByAddr ) ) {
        IncludeIntoSizes( prevBySize, blockHeader );
        return NULL;
    }

    // possible splitting of a new buffer
    int remSize = blockHeader->_size - size;
    if( remSize >= (int)MIN_BLOCK_SIZE ) {
        int blockSize = HEADER_REAL_SIZE + ( blockHeader->_size = size );
        BlockHeader remBlockHeader = ( BlockHeader )( (uint8_t*)blockHeader + blockSize );
        remBlockHeader->_size = remSize - HEADER_REAL_SIZE;

        if( !IncludeIntoSizes( NULL, remBlockHeader ) )
            return NULL;  // !!! VERY BAD !!!
        if( prevByAddr ) {
            remBlockHeader->_nextByAddr = prevByAddr->_nextByAddr;
            prevByAddr->_nextByAddr = remBlockHeader;
        }
        else {
            remBlockHeader->_nextByAddr = firstByAddr;
            firstByAddr = remBlockHeader;
        }
    }

    blockHeader->_nextBySize = blockHeader->_nextByAddr =
        blockHeader;  // just to help in checking during Heap_Free (if it is THE
                      // block and is not an "alien" pointer)

    return ( (uint8_t*)blockHeader + HEADER_REAL_SIZE );
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
static BlockHeader ICACHE_FLASH_ATTR FromPtrToBlockHeader( void* ptr ) {
    if( ( (uint8_t*)ptr < ( heapBytes + HEADER_REAL_SIZE ) ) ||
        ( (uint8_t*)ptr > ( heapBytes + HEAP_SIZE - DATA_ALIGNMENT ) ) || ( (int)ptr % DATA_ALIGNMENT ) )
        return NULL;

    BlockHeader blockHeader = ( BlockHeader )( (uint8_t*)ptr - HEADER_REAL_SIZE );
    if( !IsBlockHeaderValid( blockHeader ) || ( blockHeader->_nextBySize != blockHeader ) ||
        ( blockHeader->_nextByAddr != blockHeader ) )
        return NULL;

    return blockHeader;
}
//----------------------------------------------------------------------------
static bool ICACHE_FLASH_ATTR Free_ExcludeFromSizes( BlockHeader blockHeader, BlockHeader* prevBlockHeader ) {
    // horizontal search - by block->size
    BlockHeader prev = NULL;
    BlockHeader curr = firstBySize;
    int size = blockHeader->_size;
    while( curr && curr->_size < size ) {
        prev = curr;
        curr = curr->_nextBySize;
    }
    if( curr == NULL || curr->_size != size )
        return FALSE;

    BlockHeader next = curr->_nextBySize;

    // vertical search - by addr of block
    BlockHeader upper = NULL;
    while( curr && curr < blockHeader ) {  // while( curr && curr != blockHeader ) {
        upper = curr;
        curr = BlockHeader_Child( curr );
    }
    if( curr != blockHeader )
        return FALSE;

    *prevBlockHeader = prev;

    // exclusion from vertical list and correction of the pointers
    if( upper )
        BlockHeader_Child( upper ) = BlockHeader_Child( curr );
    else {
        BlockHeader below = BlockHeader_Child( curr );
        if( below ) {
            below->_nextBySize = next;
            next = below;
        }

        if( prev )
            prev->_nextBySize = next;
        else
            firstBySize = next;
    }

    return TRUE;
}
/*
//----------------------------------------------------------------------------
static void ICACHE_FLASH_ATTR Free_UniteBlocks( BlockHeader blockHeader1,
BlockHeader blockHeader2 )
{
        blockHeader1->_size += HEADER_REAL_SIZE + blockHeader2->_size;
}
*/
#define Free_UniteBlocks( blockHeader1, blockHeader2 )                 \
    do {                                                               \
        blockHeader1->_size += HEADER_REAL_SIZE + blockHeader2->_size; \
    } while( 0 )
/*
//----------------------------------------------------------------------------
static bool ICACHE_FLASH_ATTR Free_AreBlocksNeighboring( BlockHeader
blockHeader1, BlockHeader blockHeader2 )
{
        return ( (uint8_t*)blockHeader1 + HEADER_REAL_SIZE + blockHeader1->_size
== (uint8_t*)blockHeader2 );
}
*/
#define Free_AreBlocksNeighboring( blockHeader1, blockHeader2 ) \
    ( (uint8_t*)( blockHeader1 ) + HEADER_REAL_SIZE + ( blockHeader1 )->_size == (uint8_t*)( blockHeader2 ) )

//----------------------------------------------------------------------------
void ICACHE_FLASH_ATTR Heap_Free( void* ptr ) {
    if( ptr == NULL )
        return FALSE;

    BlockHeader blockHeader = FromPtrToBlockHeader( ptr );
    if( blockHeader == NULL )
        return FALSE;

    BlockHeader prevByAddr = NULL;
    BlockHeader curr = firstByAddr;
    while( curr && ( curr < blockHeader ) ) {
        prevByAddr = curr;
        curr = curr->_nextByAddr;
    }

    BlockHeader nextByAddr;
    BlockHeader prevBySize = NULL;

    if( prevByAddr ) {
        if( Free_AreBlocksNeighboring( prevByAddr, blockHeader ) ) {
            if( !Free_ExcludeFromSizes( prevByAddr, &prevBySize ) )
                return FALSE;
            Free_UniteBlocks( prevByAddr, blockHeader );

            if( ( nextByAddr = ( blockHeader = prevByAddr )->_nextByAddr ) ) {
                if( Free_AreBlocksNeighboring( blockHeader, nextByAddr ) ) {
                    BlockHeader prevBySize2;
                    if( !Free_ExcludeFromSizes( nextByAddr, &prevBySize2 ) )
                        return FALSE;
                    Free_UniteBlocks( blockHeader, nextByAddr );
                    if( prevBySize == nextByAddr )
                        prevBySize = prevBySize2;
                    else {
                        if( prevBySize ) {
                            if( prevBySize2 && prevBySize->_size < prevBySize2->_size )
                                prevBySize = prevBySize2;
                        }
                        else if( prevBySize2 )
                            prevBySize = prevBySize2;
                    }
                    blockHeader->_nextByAddr = nextByAddr->_nextByAddr;
                }
                else
                    blockHeader->_nextByAddr = nextByAddr;
            }
        }
        else {  // ! Free_AreBlocksNeighboring( prevByAddr, blockHeader )

            if( ( nextByAddr = prevByAddr->_nextByAddr ) ) {
                if( Free_AreBlocksNeighboring( blockHeader, nextByAddr ) ) {
                    if( !Free_ExcludeFromSizes( nextByAddr, &prevBySize ) )
                        return FALSE;
                    Free_UniteBlocks( blockHeader, nextByAddr );
                    blockHeader->_nextByAddr = nextByAddr->_nextByAddr;
                }
                else
                    blockHeader->_nextByAddr = nextByAddr;
            }
            else
                blockHeader->_nextByAddr = NULL;

            prevByAddr->_nextByAddr = blockHeader;
        }
    }
    else {
        if( ( nextByAddr = firstByAddr ) ) {
            if( Free_AreBlocksNeighboring( blockHeader, nextByAddr ) ) {
                if( !Free_ExcludeFromSizes( nextByAddr, &prevBySize ) )
                    return FALSE;
                Free_UniteBlocks( blockHeader, nextByAddr );
                blockHeader->_nextByAddr = nextByAddr->_nextByAddr;
            }
            else
                blockHeader->_nextByAddr = nextByAddr;
        }
        else
            blockHeader->_nextByAddr = NULL;

        firstByAddr = blockHeader;
    }

    return IncludeIntoSizes( prevBySize, blockHeader );
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
void* ICACHE_FLASH_ATTR Heap_ReAlloc( void* ptr,
                                      int size )  // returns NULL if fault
{
    if( ptr == NULL )
        return Heap_Alloc( size );
    if( size == 0 ) {
        Heap_Free( ptr );
        return ptr;
    }

    BlockHeader ptrBlockHeader = FromPtrToBlockHeader( ptr );
    if( ptrBlockHeader == NULL )
        return NULL;

    void* newPtr = Heap_Alloc( size );
    if( newPtr == NULL )
        return NULL;
    BlockHeader newPtrBlockHeader = FromPtrToBlockHeader( newPtr );
    if( newPtrBlockHeader == NULL ) {
        Heap_Free( newPtr );
        return NULL;
    }

    memcpy( newPtr, ptr, ( size_t )( ( ptrBlockHeader->_size < size ) ? ptrBlockHeader->_size : size ) );

    Heap_Free( ptr );

    return newPtr;
}

//////////////////////////////////////////////////////////////////////////////

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
bool ICACHE_FLASH_ATTR Heap_Init( void ) {
    memset( heapBytes, 0, sizeof heapBytes );

    int size = (int)sizeof( heapBytes ) - HEADER_REAL_SIZE;
    if( size < DATA_ALIGNMENT )
        return FALSE;

    BlockHeader startBlock = firstByAddr = firstBySize = (BlockHeader)heapBytes;
    startBlock->_size = size;
    startBlock->_nextByAddr = startBlock->_nextBySize = NULL;

    return TRUE;
}

#if( defined( _WINDOWS ) || defined( __APPLE__ ) || defined( __linux__ ) )

//////////////////////////////////////////////////////////////////////////////

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
#define TEST_BLOCKS_COUNT 10000
//#define TEST_BLOCKS_COUNT  2
static void* testBlocks[TEST_BLOCKS_COUNT];

static int alloced = 0;

//----------------------------------------------------------------------------
#define BYTES_PER_CHAR ( ( HEADER_ALIGNMENT > DATA_ALIGNMENT ) ? HEADER_ALIGNMENT : DATA_ALIGNMENT )

#define MEMORY_MAP_SIZE ( HEAP_SIZE / BYTES_PER_CHAR )

// static char memoryMapBySize[ MEMORY_MAP_SIZE ];
// static char memoryMapByAddr[ MEMORY_MAP_SIZE ];
static char memoryMap[MEMORY_MAP_SIZE];

static const char allocedSigns[4] = { '*', '+', '.', ' ' };

#define MEMORY_MAP_WIDTH 128
#define MEMORY_MAP_HEIGHT ( MEMORY_MAP_SIZE / MEMORY_MAP_WIDTH )

// static char memoryScreen[ MEMORY_MAP_HEIGHT ][ MEMORY_MAP_WIDTH ];

//----------------------------------------------------------------------------
void ICACHE_FLASH_ATTR Heap_Show( void ) {
    for( int i = 0; i < (int)MEMORY_MAP_SIZE; ++i )
        memoryMap[i] = 0;

    BlockHeader bh = firstByAddr;
    while( bh ) {
        // int start = (int)(((uint8_t*)bh - heapBytes) / (int)BYTES_PER_CHAR);
        int start = (int)(uint8_t*)bh;
        start -= (int)heapBytes;
        start /= (int)BYTES_PER_CHAR;
        // int count = (int)((HEADER_REAL_SIZE + bh->_size) /
        // (int)BYTES_PER_CHAR);
        int count = (int)HEADER_REAL_SIZE;
        count += (int)bh->_size;
        count /= (int)BYTES_PER_CHAR;
        for( int i = 0; i < count; ++i )
            ++memoryMap[start + i];
        bh = bh->_nextByAddr;
    }

    bh = firstBySize;
    while( bh ) {
        int start = (int)( ( (uint8_t*)bh - heapBytes ) / (int)BYTES_PER_CHAR );
        int count = (int)( ( HEADER_REAL_SIZE + bh->_size ) / (int)BYTES_PER_CHAR );
        for( int i = 0; i < count; ++i )
            ++memoryMap[start + i];

        BlockHeader bh_ = BlockHeader_Child( bh );
        while( bh_ ) {
            start = (int)( ( (uint8_t*)bh_ - heapBytes ) / (int)BYTES_PER_CHAR );
            count = (int)( ( HEADER_REAL_SIZE + bh_->_size ) / (int)BYTES_PER_CHAR );
            for( int i = 0; i < count; ++i )
                ++memoryMap[start + i];
            bh_ = BlockHeader_Child( bh_ );
        }

        bh = bh->_nextBySize;
    }

    Log_Write( "\r\n" );
    Log_Write( "==============================================================="
               "=\r\n" );

    int x = 0;
    int y = 0;
    for( int i = 0; i < (int)MEMORY_MAP_SIZE; ++i ) {
        // memoryScreen[ y ][ x ] = allocedSigns[ memoryMap[ i ] ];
        char ch = memoryMap[i];
        if( ch < 0 )
            Log_Write( "-" );
        else if( ch > 2 )
            Log_Write( "+" );
        else
            Log_Write( "%c", ( ch ? ( '0' + ch ) : '.' ) );
        if( ++x >= MEMORY_MAP_WIDTH ) {
            x = 0;
            ++y;
            // Log_Write( ":%4x:%d\r\n", i*BYTES_PER_CHAR, i*BYTES_PER_CHAR );
            Log_Write( "\r\n" );
        }
    }
    Log_Write( "==============================================================="
               "=\r\n" );
    Log_Write( "\r\n" );

    /*
    Log_Write( "\r\n" );
    Log_Write( "----------------------------------\r\n" );
    Log_Write( "Heap Blocks:\r\n" );
    int blocksCount = 0;
    bh = firstByAddr;
    while( bh ) {
            ++blocksCount;
            bh = bh->_nextByAddr;
    }
    Log_Write( "ByAddr{%2d}: ", blocksCount );
    char* spaceStr = "";
    bh = firstByAddr;
    while( bh ) {
            Log_Write( "%s%5d[%5d->%5d]\r\n", spaceStr, (uint8_t*)bh -
    heapBytes, bh->_size, bh->_nextByAddr ? (uint8_t*)bh->_nextByAddr -
    heapBytes : -1 ); spaceStr = "            "; bh = bh->_nextByAddr;
    }
    Log_Write( "\r\n" );

    bh = firstBySize;
    int maxBlockSize = 0;
    int maxBlockSizeAddr = -1;
    int freeSize = 0;
    blocksCount = 0;
    while( bh ) {
            ++blocksCount;
            BlockHeader bh_ = BlockHeader_Child( bh );
            while( bh_ ) {
                    ++blocksCount;
                    bh_ = BlockHeader_Child( bh_ );
            }
            bh = bh->_nextBySize;
    }
    Log_Write( "BySize{%2d}: ", blocksCount );
    spaceStr = "";
    bh = firstBySize;
    while( bh ) {
            ++blocksCount;
            if( maxBlockSize < bh->_size ) {
                    maxBlockSize = bh->_size;
                    maxBlockSizeAddr = (uint8_t*)bh - heapBytes;
            }
            freeSize += bh->_size;

            int subBlocksCount = 0;
            BlockHeader bh_ = BlockHeader_Child( bh );
            while( bh_ ) {
                    ++subBlocksCount;
                    bh_ = BlockHeader_Child( bh_ );
            }

            Log_Write( "%s%5d[%5d->%5d:%5d]{%5d}", spaceStr, (uint8_t*)bh -
    heapBytes, bh->_size, bh->_nextBySize ? (uint8_t*)bh->_nextBySize -
    heapBytes : -1, BlockHeader_Child( bh ) ? (uint8_t*)BlockHeader_Child( bh )
    - heapBytes : -1, subBlocksCount ); spaceStr = "            "; bh_ =
    BlockHeader_Child( bh ); while( bh_ ) {
                    ++blocksCount;
                    if( maxBlockSize < bh_->_size ) {
                            maxBlockSize = bh_->_size;
                            maxBlockSizeAddr = (uint8_t*)bh_ - heapBytes;
                    }
                    freeSize += bh_->_size;
                    Log_Write( " %5d[%5d:%5d]", (uint8_t*)bh_ - heapBytes,
    bh_->_size, BlockHeader_Child( bh_ ) ? (uint8_t*)BlockHeader_Child( bh_ ) -
    heapBytes : -1 ); bh_ = BlockHeader_Child( bh_ );
            }
            Log_Write( "\r\n" );
            bh = bh->_nextBySize;
    }
    Log_Write( "\r\n" );

    Log_Write( "\r\n" );
    Log_Write( "MaxBlockSize: %5d[ %5d ]\r\n", maxBlockSizeAddr, maxBlockSize );
    Log_Write( "FreeSize    : %5d\r\n", freeSize );

    Log_Write( "----------------------------------\r\n" );
    */
}

//----------------------------------------------------------------------------
void ICACHE_FLASH_ATTR Heap_Alloc_Blocks( void ) {
    for( ;; ) {
        int i = rand() % TEST_BLOCKS_COUNT;

        int count = TEST_BLOCKS_COUNT;
        while( count && testBlocks[i] ) {
            i = ( i + 1 ) % TEST_BLOCKS_COUNT;
            --count;
        }
        if( count == 0 ) {  // Log_Write( "!!! count == 0 !!!\r\n" );
            Heap_Show();
            return;
        }
        if( ( testBlocks[i] = Heap_Alloc( ( rand() % 257 ) + 1 ) ) == NULL ) {
            // Log_Write( "!!! NULL !!!\r\n" );
            Heap_Show();
            return;
        }
        // Log_Write( "*** allocated ***\r\n" );
        ++alloced;
        Heap_Show();
    }
}

//----------------------------------------------------------------------------
void ICACHE_FLASH_ATTR Heap_Free_Blocks( void ) {
    for( int step = alloced; step; --step ) {
        int i = rand() % TEST_BLOCKS_COUNT;

        int count = TEST_BLOCKS_COUNT;
        while( count && testBlocks[i] == NULL ) {
            i = ( i + 1 ) % TEST_BLOCKS_COUNT;
            --count;
        }
        if( count == 0 ) {  // Log_Write( "!!! count == 0 !!!\r\n" );
            Heap_Show();
            return;
        }
        Heap_Free( testBlocks[i] );
        testBlocks[i] = NULL;
        // Log_Write( "!!! freed !!!\r\n" );
        Heap_Show();
    }
}

//----------------------------------------------------------------------------
void ICACHE_FLASH_ATTR Heap_Test( void ) {
    setvbuf( stdout, NULL, _IONBF, 0 );
    setvbuf( stderr, NULL, _IONBF, 0 );

    memset( testBlocks, 0, sizeof testBlocks );

    int i_finish = 100000;
    int percent = -1;
    for( int i = 0; i < i_finish; ++i ) {
        int newPercent = ( i * 100 ) / i_finish;
        if( newPercent != percent )
            Log_Write( "%d\r\n", ( percent = newPercent ) );

        Heap_Show();

        int op = rand() % 4;
    _L_op_:
        if( op ) {
            int size = ( ( rand() % 257 ) + 1 );
            int count = TEST_BLOCKS_COUNT;
            int testBlocksI = rand() % TEST_BLOCKS_COUNT;
            while( count && testBlocks[testBlocksI] ) {
                testBlocksI = ( testBlocksI + 1 ) % TEST_BLOCKS_COUNT;
                --count;
            }
            if( count == 0 ) {  // Log_Write( "!!! count == 0 !!!\r\n" );
                op = 0;
                goto _L_op_;
            }
            if( ( testBlocks[testBlocksI] = Heap_Alloc( size ) ) == NULL ) {
                op = 0;
                goto _L_op_;
            }
        }
        else {
            int count = TEST_BLOCKS_COUNT;
            int testBlocksI = rand() % TEST_BLOCKS_COUNT;
            while( count && testBlocks[testBlocksI] == NULL ) {
                testBlocksI = ( testBlocksI + 1 ) % TEST_BLOCKS_COUNT;
                --count;
            }
            if( count == 0 ) {  // Log_Write( "!!! count == 0 !!!\r\n" );
                op = 1;
                goto _L_op_;
            }
            Heap_Free( testBlocks[testBlocksI] );
            testBlocks[testBlocksI] = NULL;
        }
    }

    // Log_Write( "\n!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\r\n" );

    for( int testBlocksI = 0; testBlocksI < TEST_BLOCKS_COUNT; ++testBlocksI ) {
        if( testBlocks[testBlocksI] ) {
            Heap_Free( testBlocks[testBlocksI] );
            testBlocks[testBlocksI] = NULL;
            Heap_Show();
        }
    }
}

#endif  // (defined(_WINDOWS) || defined(__APPLE__) || defined(__linux__))

#endif  // USE_ORIGINAL_HEAP_MANAGER
