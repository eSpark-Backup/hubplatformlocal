#pragma once

#include <znet/enums/meter_type.hpp>

namespace znet {
namespace meter {

using Scale = uint8_t;
using ScaleOpt = utils::optional<Scale>;

MeterTypeOpt stringToType( const char* name );

const char* typeToString( MeterType type );

const char* scaleToString( MeterType type, uint8_t scale );

ScaleOpt stringToScale( MeterType type, const char* scaleName );

bool isScaleSupported( MeterType type, uint8_t scale );

}  // namespace meter
}  // namespace znet
