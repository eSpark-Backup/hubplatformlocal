#include <znet/converters/signed_configuration_parameter_converter.hpp>

#include <cassert>

#include <znet/enums/configuration_format.hpp>

namespace znet {
namespace signed_configuration_parameter_converter {

int64_t toUiValue( uint32_t value, uint8_t size, ConfigurationFormat format ) noexcept {
    if( format != ConfigurationFormat::SignedInteger ) {
        return static_cast<int64_t>( value );
    }

    switch( size ) {
    case 1:
        return (int8_t)value;

    case 2:
        return (int16_t)value;

    case 4:
        return (int32_t)value;

    default:
        assert( 0 && "Unsupported configuration value size" );
        return static_cast<int64_t>( value );
    }
}

uint32_t toZNetValue( int64_t value, uint8_t size, ConfigurationFormat format ) noexcept {
    if( format != ConfigurationFormat::SignedInteger ) {
        return static_cast<uint32_t>( value );
    }

    switch( size ) {
    case 1:
        return (uint8_t)value;

    case 2:
        return (uint16_t)value;

    case 4:
        return (uint32_t)value;

    default:
        assert( 0 && "Unsupported configuration value size" );
        return static_cast<uint32_t>( value );
    }
}

}  // namespace signed_configuration_parameter_converter
}  // namespace znet
