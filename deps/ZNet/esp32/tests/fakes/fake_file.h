#pragma once

#include <znet/IFile.hpp>

#include <vector>
#include <cstdint>

namespace znet {
namespace ut {

class FakeFile : public IFile {
public:
    using Data = std::vector<uint8_t>;

    template <typename... Args>
    FakeFile( Args... args ) {
        addValuesToData( args... );
    }

    FakeFile( Data data );

    bool write( const void* src, size_t len ) override;

    bool read( void* dst, size_t len ) override;

    const Data& getData() const;

    size_t getReadPos() const;

    void setReadPos( size_t pos );

private:
    template <typename First, typename... Args>
    typename std::enable_if<std::is_pod<First>::value>::type addValuesToData( First first, Args... args ) {
        addValuesToDataImpl( &first, sizeof( first ) );
        addValuesToData( args... );
    }

    void addValuesToData() {}

    void addValuesToDataImpl( const void*, size_t size );

private:
    Data m_data;
    size_t m_pos = 0;
};

}  // namespace ut
}  // namespace znet
