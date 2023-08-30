#pragma once

#include <memory>

#include <cstdint>

namespace znet {
namespace IOWrapper {

class Source {
public:
    virtual ~Source() = default;

    virtual size_t read( size_t size, uint8_t* dst ) = 0;

    virtual void seek( size_t pos ) = 0;
};

class Destination {
public:
    virtual ~Destination() = default;

    virtual void write( size_t size, const uint8_t* src ) = 0;
};

using SourceUPtr = std::unique_ptr<Source>;
using DestinationUPtr = std::unique_ptr<Destination>;

}  // namespace IOWrapper
}  // namespace znet
