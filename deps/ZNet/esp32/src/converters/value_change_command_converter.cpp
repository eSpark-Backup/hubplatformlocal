#include <znet/converters/value_change_command_converter.hpp>
#include <znet/znet_utils.hpp>
#include "znet/converters/resources.hpp"

namespace znet {

namespace {
using RangedStringList = utils::RangedList<ValueChangeCommand, const char*>;
using StringList = std::initializer_list<const char* const>;

const StringList Commands_00 = { resources::value_change_command::Up,
                                 resources::value_change_command::Down,
                                 resources::value_change_command::Stop };

const RangedStringList CommandsList = { { ValueChangeCommand::Up, Commands_00 } };
}  // namespace

ValueChangeCommandOpt stringToValueChangeCommand( const char* str ) {
    return utils::findIndexInRangedList( CommandsList, [str]( const char* cmd ) { return strcmp( str, cmd ) == 0; } );
}

}  // namespace znet
