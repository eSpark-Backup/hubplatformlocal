#pragma once

#include <cstdint>

namespace znet {
namespace ut {

template <typename Type>
class TemporaryValue {
public:
    TemporaryValue( Type& wrapped )
    : m_saved( wrapped )
    , m_wrapped( wrapped ) {}

    TemporaryValue& operator=( const Type& value ) {
        m_wrapped = value;
        return *this;
    }

    operator Type() const {
        return m_wrapped;
    }

    ~TemporaryValue() {
        m_wrapped = m_saved;
    }

private:
    const Type m_saved;
    Type& m_wrapped;
};

using TemporaryUint8 = TemporaryValue<uint8_t>;

class ZNetStateAccessor {
public:
    ZNetStateAccessor();

    void setSelfNodeId( uint8_t nodeId );

    void setSisNodeId( uint8_t nodeId );

    void setZwaveLibraryType( uint8_t type );

    void setZwaveProtocolVersion( uint8_t version, uint8_t subversion );

private:
    TemporaryUint8 m_selfNodeId;
    TemporaryUint8 m_sisNodeId;

    TemporaryUint8 m_zwaveLibraryType;
    TemporaryUint8 m_zwaveProtocolVersion;
    TemporaryUint8 m_zwaveProtocolSubVersion;
};

}  // namespace ut
}  // namespace znet
