#pragma once

#include <znet/znet.h>

#include <cstdint>
#include <vector>
#include <string>

namespace znet {
namespace UserCode {

enum class UserIDStatus : uint8_t {
    Available = 0x00,
    Enabled = 0x01,
    Disabled = 0x02,
    Messaging = 0x03,
    PassageMode = 0x04,
    NotAvailable = 0xFE
};

struct UserCodeRange {
    znet_supported_users_t id;
    znet_supported_users_t lastId;
    UserIDStatus status;
    std::string code;
};

using UserCodeRanges = std::vector<UserCodeRange>;

enum class KeypadMode : uint8_t { Normal = 0x00, Vacation = 0x01, Privacy = 0x02, LockedOut = 0x03 };

enum class UserPollStatus : uint8_t { Idle = 0x00, Polling = 0x01, Finished = 0x02 };

}  // namespace UserCode
}  // namespace znet
