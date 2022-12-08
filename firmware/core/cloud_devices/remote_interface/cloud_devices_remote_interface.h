#pragma once

#include "cloud_devices/remote_interface/cloud_devices_base_remote_interface.h"
#include "service_interfaces/service.h"

#include <memory>

namespace HubPlatform {
namespace Services {
class Service;
}
namespace CloudDevices {

class RemoteInterface final : public BaseRemoteInterface {
public:
    RemoteInterface( Services::Service& hostService );

    JsonObject createCloudSubscription( const JsonObject& params ) override;
    JsonObject updateCloudSubscription( const JsonObject& params ) override;
    JsonObject deleteCloudSubscription( const JsonObject& params ) override;
    JsonObject getCloudSubscriptionsList() override;

private:
    Services::Service& m_hostService;
};

}  // namespace CloudDevices
}  // namespace HubPlatform
