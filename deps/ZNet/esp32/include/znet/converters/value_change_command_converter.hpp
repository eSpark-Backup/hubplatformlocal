#pragma once

#include <znet/enums/value_change_command.hpp>
#include <znet/optional.hpp>

namespace znet {

using ValueChangeCommandOpt = utils::optional<ValueChangeCommand>;

ValueChangeCommandOpt stringToValueChangeCommand( const char* str );

}  // namespace znet
