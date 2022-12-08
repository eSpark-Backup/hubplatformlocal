#include "fake_file.h"

#include <string.h>
#include <numeric>

namespace znet {
namespace ut {

FakeFile::FakeFile( Data data )
: m_data( std::move( data ) ) {}

bool FakeFile::write( const void* src, size_t len ) {
    addValuesToDataImpl( src, len );
    return true;
}

bool FakeFile::read( void* dst, size_t len ) {
    bool result = m_data.size() >= m_pos + len;
    if( !result ) {
        len = m_data.size() - m_pos;
    }
    memcpy( dst, m_data.data() + m_pos, len );
    m_pos += len;
    return result;
}

void FakeFile::addValuesToDataImpl( const void* src, size_t len ) {
    m_data.resize( m_data.size() + len );
    memcpy( m_data.data() + m_data.size() - len, src, len );
}

const FakeFile::Data& FakeFile::getData() const {
    return m_data;
}

size_t FakeFile::getReadPos() const {
    return m_pos;
}

void FakeFile::setReadPos( size_t pos ) {
    m_pos = std::min( pos, m_data.size() );
}

}  // namespace ut
}  // namespace znet
