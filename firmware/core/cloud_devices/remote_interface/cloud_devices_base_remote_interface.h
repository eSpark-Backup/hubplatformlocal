#pragma once

#include "json_utils/json_fwd.h"
#include "jsonrpc/message.h"

namespace HubPlatform {
namespace CloudDevices {

class BaseRemoteInterface {
public:
    virtual ~BaseRemoteInterface() = default;

    virtual JsonObject createCloudSubscription( const JsonObject& params ) = 0;
    virtual JsonObject deleteCloudSubscription( const JsonObject& params ) = 0;
    virtual JsonObject updateCloudSubscription( const JsonObject& params ) = 0;
    virtual JsonObject getCloudSubscriptionsList() = 0;
};

}  // namespace CloudDevices
}  // namespace HubPlatform
