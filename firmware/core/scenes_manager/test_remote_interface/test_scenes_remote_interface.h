#pragma once

#include "json_utils/json_fwd.h"
#include "jsonrpc/message.h"
#include "service_interfaces/service.h"

namespace HubPlatform {
namespace ScenesManager {

class TestRemoteInterface final {
public:
    TestRemoteInterface( Services::Service& hostService );
    JsonObject setTestClock( std::chrono::seconds startUnixTime,
                             std::chrono::hours timeZoneOffset = std::chrono::hours( 0 ) );

private:
    Services::Service& m_hostService;
};

}  // namespace ScenesManager
}  // namespace HubPlatform
