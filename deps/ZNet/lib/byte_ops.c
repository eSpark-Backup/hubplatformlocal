#include <stdlib.h>
#include <math.h>

#include "ZPlatform.h"

#include "byte_ops.h"
#include "heap.h"

#include "znet_lib.h"

//////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------
static const char tetradChars[16] ICACHE_RODATA_ATTR STORE_ATTR = { '0', '1', '2', '3', '4', '5', '6', '7',
                                                                    '8', '9', 'A', 'B', 'C', 'D', 'E', 'F' };

//////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------
void ICACHE_FLASH_ATTR ByteIntoHexStr( uint8_t b, char* str ) {
    *str++ = tetradChars[( b >> 4 ) & 0x0F];
    *str = tetradChars[b & 0x0F];
}
//----------------------------------------------------------------------------
bool ICACHE_FLASH_ATTR ByteIntoDecStr( uint8_t value, char* str, int places, ByteStrAlign_e align, char fillingChar ) {
    if( value < 10 ) {
        if( places < 1 )
            return FALSE;
        if( align == ByteStrAlign_Left ) {
            *str++ = tetradChars[value];
            --places;
            while( places-- )
                *str++ = fillingChar;
        }
        else {
            --places;
            while( places-- )
                *str++ = fillingChar;
            *str = tetradChars[value];
        }
        return TRUE;
    }

    if( value < 100 ) {
        if( places < 2 )
            return FALSE;
        if( align == ByteStrAlign_Left ) {
            *str++ = tetradChars[value / 10];
            *str++ = tetradChars[value % 10];
            places -= 2;
            while( places-- )
                *str++ = fillingChar;
        }
        else {
            places -= 2;
            while( places-- )
                *str++ = fillingChar;
            *str++ = tetradChars[value / 10];
            *str = tetradChars[value % 10];
        }
        return TRUE;
    }

    if( places < 3 )
        return FALSE;
    if( align == ByteStrAlign_Left ) {
        *str++ = tetradChars[value / 100];
        *str++ = tetradChars[( value % 100 ) / 10];
        *str++ = tetradChars[value % 10];
        places -= 3;
        while( places-- )
            *str++ = fillingChar;
    }
    else {
        places -= 3;
        while( places-- )
            *str++ = fillingChar;
        *str++ = tetradChars[value / 100];
        *str++ = tetradChars[( value % 100 ) / 10];
        *str = tetradChars[value % 10];
    }
    return TRUE;
}

//////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------
/*int ICACHE_FLASH_ATTR bytes_to_int( const uint8_t* bytes, uint8_t length ){
    unsigned int v = 0;
    for( int i = 0; i < MIN(length, 4); i++ ) v = (v << 8) | bytes[ i ];
    return (int)v;
}*/
//----------------------------------------------------------------------------
uint16_t ICACHE_FLASH_ATTR bytes_to_int16( const uint8_t* bytes ) {
    return ( uint16_t )( ( (uint16_t)bytes[0] << 8 ) | (uint16_t)bytes[1] );
}
//----------------------------------------------------------------------------
int32_t ICACHE_FLASH_ATTR bytes_to_sint24( const uint8_t* bytes ) {
    int32_t b0 = ( int32_t )(uint32_t)bytes[0];
    return ( ( ( b0 & 0x80 ) ? 0xFF : 0 ) << 24 ) | ( ( b0 << 16 ) | ( bytes[1] << 8 ) | bytes[2] );
}
//----------------------------------------------------------------------------
uint32_t ICACHE_FLASH_ATTR bytes_to_uint24( const uint8_t* bytes ) {
    return ( (uint32_t)bytes[0] << 16 ) | ( (uint32_t)bytes[1] << 8 ) | (uint32_t)bytes[2];
}
//----------------------------------------------------------------------------
uint32_t ICACHE_FLASH_ATTR bytes_to_int32( const uint8_t* bytes ) {
    return ( (uint32_t)bytes[0] << 24 ) | ( (uint32_t)bytes[1] << 16 ) | ( (uint32_t)bytes[2] << 8 ) |
           (uint32_t)bytes[3];
}
//----------------------------------------------------------------------------
uint32_t ICACHE_FLASH_ATTR bytes_to_int( const uint8_t* bytes, int bytesLen ) {
    switch( bytesLen ) {
    case 1:
        return (uint32_t)bytes[0];
    case 2:
        return (uint32_t)bytes_to_int16( bytes );
    case 3:
        return bytes_to_uint24( bytes );
    case 4:
        return bytes_to_int32( bytes );
    default:
        break;
    }
    return 0;
}
//////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------
/*int ICACHE_FLASH_ATTR bytes_to_int_le(const uint8_t* bytes, uint8_t length){
    unsigned int v = 0;
    for( int i = 0; i < MIN(length, 4); i++ ) v |= (bytes[ i ] << (8 * i));
    return (int)v;
}*/
//----------------------------------------------------------------------------
int16_t ICACHE_FLASH_ATTR bytes_to_int16_le( const uint8_t* bytes ) {
    return ( int16_t )( ( (uint16_t)bytes[1] << 8 ) | (uint16_t)bytes[0] );
}
//----------------------------------------------------------------------------
int32_t ICACHE_FLASH_ATTR bytes_to_sint24_le( const uint8_t* bytes ) {
    int32_t b2 = ( int32_t )(uint32_t)bytes[2];
    return ( ( ( b2 & 0x80 ) ? 0xFF : 0 ) << 24 ) | ( ( b2 << 16 ) | ( bytes[1] << 8 ) | bytes[0] );
}
//----------------------------------------------------------------------------
uint32_t ICACHE_FLASH_ATTR bytes_to_uint24_le( const uint8_t* bytes ) {
    return ( (uint32_t)bytes[2] << 16 ) | ( (uint32_t)bytes[1] << 8 ) | (uint32_t)bytes[0];
}
//----------------------------------------------------------------------------
uint32_t ICACHE_FLASH_ATTR bytes_to_int32_le( const uint8_t* bytes ) {
    return ( (uint32_t)bytes[3] << 24 ) | ( (uint32_t)bytes[2] << 16 ) | ( (uint32_t)bytes[1] << 8 ) |
           (uint32_t)bytes[0];
}
//----------------------------------------------------------------------------
uint32_t ICACHE_FLASH_ATTR bytes_to_int_le( const uint8_t* bytes, int bytesLen ) {
    switch( bytesLen ) {
    case 1:
        return (uint32_t)bytes[0];
    case 2:
        return (uint32_t)bytes_to_int16_le( bytes );
    case 3:
        return bytes_to_uint24_le( bytes );
    case 4:
        return bytes_to_int32_le( bytes );
    default:
        return 0;
    }
    return 0;
}
//////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------
/*void ICACHE_FLASH_ATTR int_to_bytes(int value, uint8_t* bytes, uint8_t
length){ unsigned int v = (unsigned int)value; for( int i = MIN(MAX(length, 1) -
1, 3); i >= 0; i--, v >>= 8 ) bytes[ i ] = (v & 0xFF);
}*/
//----------------------------------------------------------------------------
void ICACHE_FLASH_ATTR int16_to_bytes( uint16_t value, uint8_t* bytes ) {
    bytes[0] = ( uint8_t )( ( value >> 8 ) & 0xFF );
    bytes[1] = ( uint8_t )( value & 0xFF );
}
//----------------------------------------------------------------------------
void ICACHE_FLASH_ATTR int24_to_bytes( uint32_t value, uint8_t* bytes ) {
    bytes[0] = ( value >> 16 ) & 0xFF;
    bytes[1] = ( value >> 8 ) & 0xFF;
    bytes[2] = value & 0xFF;
}
//----------------------------------------------------------------------------
void ICACHE_FLASH_ATTR int32_to_bytes( uint32_t value, uint8_t* bytes ) {
    bytes[0] = ( uint8_t )( ( value >> 24 ) & 0xFF );
    bytes[1] = ( uint8_t )( ( value >> 16 ) & 0xFF );
    bytes[2] = ( uint8_t )( ( value >> 8 ) & 0xFF );
    bytes[3] = ( uint8_t )( value & 0xFF );
}
//----------------------------------------------------------------------------
bool ICACHE_FLASH_ATTR int_to_bytes( uint32_t value, int size, uint8_t* bytes ) {
    switch( size ) {
    case 1:
        bytes[0] = ( uint8_t )( value & 0xFF );
        break;
    case 2:
        int16_to_bytes( ( uint16_t )( value & 0xFFFF ), bytes );
        break;
    case 3:
        int24_to_bytes( value, bytes );
        break;
    case 4:
        int32_to_bytes( value, bytes );
        break;
    default:
        return FALSE;
    }
    return TRUE;
}

//////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------
/*void ICACHE_FLASH_ATTR int_to_bytes_le(int value, uint8_t* bytes, uint8_t
length){ unsigned int v = (unsigned int)value; for( int i = 0; i < MIN(length,
4); i++, v >>= 8 ) bytes[ i ] = (v & 0xFF);
}*/
//----------------------------------------------------------------------------
void ICACHE_FLASH_ATTR int16_to_bytes_le( uint16_t value, uint8_t* bytes ) {
    bytes[0] = ( uint8_t )( value & 0xFF );
    bytes[1] = ( uint8_t )( ( value >> 8 ) & 0xFF );
}
//----------------------------------------------------------------------------
void ICACHE_FLASH_ATTR int24_to_bytes_le( uint32_t value, uint8_t* bytes ) {
    bytes[0] = value & 0xFF;
    bytes[1] = ( value >> 8 ) & 0xFF;
    bytes[2] = ( value >> 16 ) & 0xFF;
}
//----------------------------------------------------------------------------
void ICACHE_FLASH_ATTR int32_to_bytes_le( uint32_t value, uint8_t* bytes ) {
    bytes[0] = ( uint8_t )( value & 0xFF );
    bytes[1] = ( uint8_t )( ( value >> 8 ) & 0xFF );
    bytes[2] = ( uint8_t )( ( value >> 16 ) & 0xFF );
    bytes[3] = ( uint8_t )( ( value >> 24 ) & 0xFF );
}
//----------------------------------------------------------------------------
bool ICACHE_FLASH_ATTR int_to_bytes_le( uint32_t value, int size, uint8_t* bytes ) {
    switch( size ) {
    case 1:
        bytes[0] = ( uint8_t )( value & 0xFF );
        break;
    case 2:
        int16_to_bytes_le( ( uint16_t )( value & 0xFFFF ), bytes );
        break;
    case 3:
        int24_to_bytes_le( value, bytes );
        break;
    case 4:
        int32_to_bytes_le( value, bytes );
        break;
    default:
        return FALSE;
    }
    return TRUE;
}

//////////////////////////////////////////////////////////////////////////////

//----------------------------------------------------------------------------
uint8_t ICACHE_FLASH_ATTR float_to_bytes( float value, uint8_t* bytes, uint8_t* detectedPrecision ) {
    // determine precision
    uint8_t precision = 7;  // precision is 3 bits, so we expect up to 7 digits
                            // after decimal point
    // int64_t val = (int64_t)round( value * pow(10.0,precision) );
    // int64_t val = (int64_t)round( value * Math_pow10[ precision ] );
    int64_t val = ( int64_t )( value * (float)Math_pow10[precision] );

    // decrease precision while least significant digit is zero
    while( precision > 0 && ( val % 10 ) == 0 ) {
        precision--;
        val /= 10;
    }
    // decrease precision while number is too big to fit 4 bytes
    while( precision > 0 && ( val > INT_MAX || val < INT_MIN ) ) {
        precision--;
        val /= 10;
    }

    // determine size
    uint8_t size;
    if( val >= SCHAR_MIN && val <= SCHAR_MAX ) {
        size = 1;
        bytes[0] = ( uint8_t )( val & 0xFF );
    }
    else if( val >= SHRT_MIN && val <= SHRT_MAX ) {
        size = 2;
        int16_to_bytes( *( (uint16_t*)&val ), bytes );
    }
    else {
        size = 4;
        int32_to_bytes( *( (uint32_t*)&val ), bytes );
    }

    if( detectedPrecision )
        *detectedPrecision = precision;

    return size;
}

//////////////////////////////////////////////////////////////////////////////

//----------------------------------------------------------------------------
int ICACHE_FLASH_ATTR strsize_for_bytes( int length ) {
    return ( 1 + length * 3 + 1 + 1 + 1 );  //  '['+length*(' '+tetr[1]+tetr[0])+' '+' ]'+'\0'
}

//----------------------------------------------------------------------------
void ICACHE_FLASH_ATTR bytes_to_string( const uint8_t* bytes, int length, char* str ) {
    *( str++ ) = '[';
    for( int i = 0; i < length; ++i ) {
        *( str++ ) = ' ';
        uint8_t b = bytes[i];
        *( str++ ) = tetradChars[( b >> 4 ) & 0x0F];
        *( str++ ) = tetradChars[b & 0x0F];
    }
    *( str++ ) = ' ';
    *( str++ ) = ']';
    *str = 0;
}

//----------------------------------------------------------------------------
int ICACHE_FLASH_ATTR string_to_bytes( const char* str, uint8_t* bytes ) {
    int len = (int)str_length( str );
    if( len < 3 )
        return 0;

    if( str[0] == '[' && str[len - 1] == ']' ) {
        str++;
        len -= 2;
    }
    // skip heading spaces
    while( len > 0 && str[0] == ' ' ) {
        str++;
        len--;
    }
    // skip trailing spaces
    while( len > 0 && str[len - 1] == ' ' ) {
        len--;
        if( len == 0 )
            return 0;
    }

    int size = 0;

    while( len > 0 ) {
        char t1 = *str++;
        if( t1 >= tetradChars[10] )
            t1 = (char)( t1 - tetradChars[10] + 10 );
        else
            t1 = (char)( t1 - '0' );
        char t0 = *str++;
        if( t0 >= tetradChars[10] )
            t0 = (char)( t0 - tetradChars[10] + 10 );
        else
            t0 = (char)( t0 - '0' );

        *( bytes )++ = ( uint8_t )( ( t1 << 4 ) | t0 );
        ++size;

        len -= 2;
        while( len > 0 && str[0] == ' ' ) {
            str++;
            len--;
        }
    }

    return size;
}

//////////////////////////////////////////////////////////////////////////////

//----------------------------------------------------------------------------
void ICACHE_FLASH_ATTR clear_bitmask( uint8_t* bitmask ) {
    memset( bitmask, 0, 32 );
}

//////////////////////////////////////////////////////////////////////////////

//----------------------------------------------------------------------------
uint16_t ICACHE_FLASH_ATTR crc16_with_init_value( const uint8_t* bytes, size_t length, uint16_t crc ) {
    for( size_t i = 0; i < length; i++ ) {
        crc ^= ( uint16_t )( bytes[i] << 8 );
        for( uint8_t bit = 0; bit < 8; bit++ )
            crc = ( uint16_t )( ( crc & 0x8000 ) ? ( crc << 1 ) ^ 0x1021 : ( crc << 1 ) );
    }

    return crc;
}

uint16_t ICACHE_FLASH_ATTR crc16( const uint8_t* bytes, size_t length ) {
    return crc16_with_init_value( bytes, length, CRC16_DEFAULT_INIT_VALUE );
}

uint16_t set_last_2_bits( uint16_t value ) {
    return value | 0xC000;
}

uint16_t reset_first_bit( uint16_t value ) {
    return value & 0xFFFE;
}
