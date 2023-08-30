#include "znet/converters/user_code.hpp"

#include "znet/converters/resources.hpp"
#include "znet/enums/user_code.hpp"
#include "znet/znet_utils.hpp"

#include <cassert>
#include <cstring>

namespace znet {

namespace user_code_converter {

namespace {

using StringList = std::initializer_list<const char* const>;
using RangedStatusStringList = utils::RangedList<UserCode::UserIDStatus, const char*>;
using RangedKeypadModeStringList = utils::RangedList<UserCode::KeypadMode, const char*>;
using PollingStatusStringList = utils::RangedList<UserCode::UserPollStatus, const char*>;

const StringList StatusList_00 = {
    resources::cc_user_code::UserIdStatus::Available,   // 0x00
    resources::cc_user_code::UserIdStatus::Enabled,     // 0x01
    resources::cc_user_code::UserIdStatus::Disabled,    // 0x02
    resources::cc_user_code::UserIdStatus::Messaging,   // 0x03
    resources::cc_user_code::UserIdStatus::PassageMode  // 0x04
};

const StringList StatusList_FE = {
    resources::cc_user_code::UserIdStatus::NotAvailable,  // 0xFE
};

const RangedStatusStringList StatusList = {
    { UserCode::UserIDStatus::Available, StatusList_00 },
    { UserCode::UserIDStatus::NotAvailable, StatusList_FE },
};

const StringList KeypadModeList_00 = {
    resources::cc_user_code::KeypadMode::Normal,     // 0x00
    resources::cc_user_code::KeypadMode::Vacation,   // 0x01
    resources::cc_user_code::KeypadMode::Privacy,    // 0x02
    resources::cc_user_code::KeypadMode::LockedOut,  // 0x03
};

const RangedKeypadModeStringList KeypadModeList = {
    { UserCode::KeypadMode::Normal, KeypadModeList_00 },
};

const StringList PollingStatusList_00 = {
    resources::cc_user_code::UserPollingStatus::Idle,      // 0x00
    resources::cc_user_code::UserPollingStatus::Polling,   // 0x01
    resources::cc_user_code::UserPollingStatus::Finished,  // 0x02
};

const PollingStatusStringList PollingStatusList = {
    { UserCode::UserPollStatus::Idle, PollingStatusList_00 },
};

template <typename List, typename ID>
const char* getNameById( const List& list, ID id ) {
    const auto result = utils::findInRangedList( list, id );
    assert( result );
    return result ? *result : nullptr;
}

}  // namespace

const char* userIdStatusToString( UserCode::UserIDStatus status ) {
    return getNameById( StatusList, status );
}

user_code_converter::UserIdStatusOpt userIdStatusFromString( const char* statusName ) {
    return utils::findIndexInRangedList(
        StatusList, [statusName]( const char* currentStatus ) { return strcmp( statusName, currentStatus ) == 0; } );
}

const char* keypadModeToString( UserCode::KeypadMode mode ) {
    return getNameById( KeypadModeList, mode );
}

KeypadModeOpt keypadModeFromString( const char* modeName ) {
    return utils::findIndexInRangedList(
        KeypadModeList, [modeName]( const char* currentMode ) { return strcmp( modeName, currentMode ) == 0; } );
}

const char* userPollStatusToString( UserCode::UserPollStatus status ) {
    return getNameById( PollingStatusList, status );
}

}  // namespace user_code_converter
}  // namespace znet
