#pragma once

#include <stdint.h>

namespace znet {
namespace cc_converter {

const char* idToString( uint8_t id );

uint8_t idFromString( const char* id );

}  // namespace cc_converter
}  // namespace znet
