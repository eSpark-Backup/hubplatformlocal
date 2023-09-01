#pragma once

#include <znet/enums/color_component.hpp>

namespace znet {
namespace color_switch_converter {

const char* componentIdToStr( ColorComponentID componentID );
ColorComponentIDOpt strToColorComponentId( const char* str );

}  // namespace color_switch_converter
}  // namespace znet
