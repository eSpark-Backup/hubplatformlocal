#include "../cc/TransportService.h"

#include <iostream>
#include "utils/plog.h"
#include <assert.h>
#include <sstream>

static const char* logtag = "TransportService";
#define LOG_LOCAL_LEVEL P_LOG_VERBOSE

TransportService::TransportService()
: CommandClass( CommandClass::CommandClassId::transportService, 1 ) {}

bool TransportService::run( std::vector<std::vector<uint8_t>>& rev,
                            std::vector<uint8_t>& ret,
                            std::vector<uint8_t> const& data ) {
    std::cout << "SendData\n";

    // Check CRC
    std::uint16_t crc = tcrc16( data.data(), data.size() );
    if( crc != 0 ) {
        assert( !"Error CRC" );
    }

    uint8_t thisCommandClassId = data[0];
    uint8_t type = data[1];
    uint8_t len = data[2];
    uint8_t sessionId = data[3] & 0xF0;
    uint8_t commandClassId_offset = data[4];

    if( type == packType::start ) {
        m_datapack.clear();
        m_datapack.insert( m_datapack.end(), data.begin() + 4, data.end() - 2 /*CRC*/ );
    }
    else if( type == packType::subsequent ) {
        m_datapack.insert( m_datapack.end(), data.begin() + 5, data.end() - 2 /*CRC*/ );
    }
    else {
        P_LOGE( logtag, "Error package type" );
        return false;
    }

    if( m_datapack.size() >= len ) {
        size_t i = 0;
        std::stringstream ss;
        ss.setf( std::ios::hex, std::ios::basefield );
        for( auto x : m_datapack ) {
            if( ( i % 32 ) == 0 ) {
                ss << "\n\t";
            }
            ss << (int)x << " ";
            i++;
        }
        P_LOGV( logtag, "package: %s", ss.str().c_str() );

        rev.push_back( m_datapack );
        m_datapack.clear();

        ret = { get_id(), packType::end, sessionId };
    }

    return true;
}

std::uint16_t TransportService::tcrc16( const uint8_t* bytes, int length ) {
    uint16_t crc = 0x1D0F;  // IV

    for( int i = 0; i < length; i++ ) {
        crc ^= ( uint16_t )( bytes[i] << 8 );
        for( uint8_t bit = 0; bit < 8; bit++ )
            crc = ( uint16_t )( ( crc & 0x8000 ) ? ( crc << 1 ) ^ 0x1021 : ( crc << 1 ) );
    }

    return crc;
}
