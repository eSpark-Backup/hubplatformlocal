#include "Msg.h"

Msg::Msg( zdef::Type type, zdef::FunctionClassId fClassId ) {
    m_buffer.push_back( zdef::Sof::SOF );
    m_buffer.push_back( 1 );  // Length
    m_buffer.push_back( type );
    m_buffer.push_back( fClassId );
}

void Msg::append( std::vector<uint8_t>&& data ) {
    m_buffer.insert( m_buffer.end(), data.begin(), data.end() );
}

void Msg::append( uint8_t data ) {
    m_buffer.push_back( data );
}

void Msg::finalize() {
    m_buffer[1] = m_buffer.size() - 1 /*SOF*/;

    uint8_t lrc = 0xFF;
    for( std::size_t i = 1 /*Skip SOF*/; i < m_buffer.size(); i++ ) {
        lrc ^= m_buffer[i];
    }
    m_buffer.push_back( lrc );

    m_finalize = true;
}

size_t Msg::getLength() {
    return m_buffer.size();
}

bool Msg::isFinalize() {
    return m_finalize;
}

std::vector<uint8_t>& Msg::getData() {
    return m_buffer;
}
