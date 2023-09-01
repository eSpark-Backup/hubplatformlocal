#include <znet/converters/thermostat_setpoint_converter.hpp>
#include <znet/converters/resources.hpp>
#include <znet/znet_utils.hpp>

#include <cassert>

namespace znet {
namespace thermostat_setpoint_converter {

using namespace resources;

const char* scaleToString( uint8_t scale ) noexcept {
    if( scale == 0 ) {
        return scales::Celsius;
    }
    if( scale == 1 ) {
        return scales::Fahrenheit;
    }

    assert( !"Unsupported TermostatSetpointScale" );
    return nullptr;
}

ScaleResult scaleFromString( const char* scale ) noexcept {
    if( strcmp( scale, scales::Celsius ) == 0 ) {
        return { 0, true };
    }

    if( strcmp( scale, scales::Fahrenheit ) == 0 ) {
        return { 1, true };
    }

    return { 0, false };
}

}  // namespace thermostat_setpoint_converter
}  // namespace znet
