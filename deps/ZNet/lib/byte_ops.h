
#ifndef __BYTE_OPS_H__
#define __BYTE_OPS_H__

#include "typedefs.h"

void ICACHE_FLASH_ATTR clear_bitmask( uint8_t* bitmask );
#define SET_BIT( bitmask, index )                             \
    do {                                                      \
        ( bitmask )[( index ) / 8] |= 1 << ( ( index ) % 8 ); \
    } while( 0 )
#define CLR_BIT( bitmask, index )                                  \
    do {                                                           \
        ( bitmask )[( index ) / 8] &= ~( 1 << ( ( index ) % 8 ) ); \
    } while( 0 )
#define TEST_BIT( bitmask, index ) ( ( bitmask )[( index ) / 8] & ( 1 << ( ( index ) % 8 ) ) )

uint16_t ICACHE_FLASH_ATTR bytes_to_int16( const uint8_t* bytes );
int32_t ICACHE_FLASH_ATTR bytes_to_sint24( const uint8_t* bytes );
uint32_t ICACHE_FLASH_ATTR bytes_to_uint24( const uint8_t* bytes );
uint32_t ICACHE_FLASH_ATTR bytes_to_int32( const uint8_t* bytes );
uint32_t ICACHE_FLASH_ATTR bytes_to_int( const uint8_t* bytes, int bytesLen );

int16_t ICACHE_FLASH_ATTR bytes_to_int16_le( const uint8_t* bytes );
int32_t ICACHE_FLASH_ATTR bytes_to_sint24_le( const uint8_t* bytes );
uint32_t ICACHE_FLASH_ATTR bytes_to_uint24_le( const uint8_t* bytes );
uint32_t ICACHE_FLASH_ATTR bytes_to_int32_le( const uint8_t* bytes );
uint32_t ICACHE_FLASH_ATTR bytes_to_int_le( const uint8_t* bytes, int bytesLen );

void ICACHE_FLASH_ATTR int16_to_bytes( uint16_t value, uint8_t* bytes );
void ICACHE_FLASH_ATTR int24_to_bytes( uint32_t value, uint8_t* bytes );
void ICACHE_FLASH_ATTR int32_to_bytes( uint32_t value, uint8_t* bytes );
bool ICACHE_FLASH_ATTR int_to_bytes( uint32_t value, int size, uint8_t* bytes );

void ICACHE_FLASH_ATTR int16_to_bytes_le( uint16_t value, uint8_t* bytes );
void ICACHE_FLASH_ATTR int24_to_bytes_le( uint32_t value, uint8_t* bytes );
void ICACHE_FLASH_ATTR int32_to_bytes_le( uint32_t value, uint8_t* bytes );
bool ICACHE_FLASH_ATTR int_to_bytes_le( uint32_t value, int size, uint8_t* bytes );

int ICACHE_FLASH_ATTR strsize_for_bytes( int length );
void ICACHE_FLASH_ATTR bytes_to_string( const uint8_t* bytes, int len, char* str );
int ICACHE_FLASH_ATTR string_to_bytes( const char* str, uint8_t* bytes );

void ICACHE_FLASH_ATTR ByteIntoHexStr( uint8_t b, char* str );
typedef enum { ByteStrAlign_Left, ByteStrAlign_Right } ByteStrAlign_e;
bool ICACHE_FLASH_ATTR ByteIntoDecStr( uint8_t value, char* str, int places, ByteStrAlign_e align, char fillingChar );

uint8_t ICACHE_FLASH_ATTR float_to_bytes( float value, uint8_t* bytes, uint8_t* detectedPrecision );

#define CRC16_DEFAULT_INIT_VALUE 0x1D0F
uint16_t ICACHE_FLASH_ATTR crc16_with_init_value( const uint8_t* bytes, size_t length, uint16_t init_crc );

uint16_t ICACHE_FLASH_ATTR crc16( const uint8_t* bytes, size_t length );

uint16_t set_last_2_bits( uint16_t );
uint16_t reset_first_bit( uint16_t );

#endif  // #ifndef __BYTE_OPS_H__
