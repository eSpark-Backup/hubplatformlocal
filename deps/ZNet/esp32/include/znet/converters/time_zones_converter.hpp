#pragma once

#include <stdint.h>
#include <utility>

namespace znet {
namespace time_zones_converter {

const std::pair<const char*, const char*> getTimeZone( int16_t hoursTzo );

}  // namespace time_zones_converter
}  // namespace znet
