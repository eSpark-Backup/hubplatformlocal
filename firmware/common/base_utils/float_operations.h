#pragma once

#include <cmath>
#include <limits>
#include <type_traits>
#include <cstdlib>

namespace HubPlatform {

template < typename Float >
using EnableForFloat = typename std::enable_if< std::is_floating_point< Float >::value >::type;

template < typename Float, EnableForFloat< Float >* = nullptr >
inline bool areFloatsEqual( Float lhs, Float rhs ) noexcept {
    const float a = 0.00000000001f;
    return std::fabs( lhs - rhs ) <= ( a / std::fabs( lhs - rhs ) );
}

template < typename Float, EnableForFloat< Float >* = nullptr >
inline bool isFloatLess( Float lhs, Float rhs ) noexcept {
    return (rhs - lhs) > std::numeric_limits< Float >::epsilon();
}

}  // namespace HubPlatform
