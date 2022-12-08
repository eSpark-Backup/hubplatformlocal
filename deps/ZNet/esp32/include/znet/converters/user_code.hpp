#pragma once

#include <znet/optional.hpp>

#include <stdint.h>
#include <utility>

#include <znet/enums/user_code.hpp>

namespace znet {
namespace user_code_converter {

using UserIdStatusOpt = utils::optional<UserCode::UserIDStatus>;
using KeypadModeOpt = utils::optional<UserCode::KeypadMode>;

const char* userIdStatusToString( UserCode::UserIDStatus status );
UserIdStatusOpt userIdStatusFromString( const char* statusName );

const char* keypadModeToString( UserCode::KeypadMode mode );
KeypadModeOpt keypadModeFromString( const char* modeName );

const char* userPollStatusToString( UserCode::UserPollStatus status );

}  // namespace user_code_converter
}  // namespace znet
