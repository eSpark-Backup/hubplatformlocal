#include <znet/converters/color_switch_converter.hpp>
#include <znet/converters/resources.hpp>
#include <znet/znet_utils.hpp>

#include <cstring>

namespace znet {
namespace color_switch_converter {

namespace {

using namespace resources;
using RangedStringList = utils::RangedList<ColorComponentID, const char*>;
using StringList = std::initializer_list<const char* const>;

const StringList Components_00 = {
    cc_switch_color::Wwhite,  // 0
    cc_switch_color::Cwhite,  // 1
    cc_switch_color::Red,     // 2
    cc_switch_color::Green,   // 3
    cc_switch_color::Blue,    // 4
    cc_switch_color::Amber,   // 5
    cc_switch_color::Cyan,    // 6
    cc_switch_color::Purple,  // 7
    cc_switch_color::Indexed  // 8
};

const RangedStringList Components = { { ColorComponentID::Wwhite, Components_00 } };

}  // namespace

const char* componentIdToStr( ColorComponentID componentID ) {
    const auto result = utils::findInRangedList( Components, componentID );
    return result ? *result : nullptr;
}

ColorComponentIDOpt strToColorComponentId( const char* str ) {
    return utils::findIndexInRangedList( Components,
                                         [str]( const char* component ) { return strcasecmp( str, component ) == 0; } );
}

}  // namespace color_switch_converter
}  // namespace znet
