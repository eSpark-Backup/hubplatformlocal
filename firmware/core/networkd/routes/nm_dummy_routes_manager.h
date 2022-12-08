#pragma once

#include "networkd/routes/nm_routes_manager.h"
#include "networkd/resources/nm_resources.h"
#include "networkd/utils/nm_utils.h"

namespace HubPlatform {
namespace Network {

class DummyRoutesManager : public RoutesManager {
public:
    std::string checkAndFixRoutes( const std::vector< Json >& interfaces ) override {
        return Utils::getDefaultRouteDevice().empty() ? "" : Resources::EthernetDeviceName;
    };
};

}  // namespace Network
}  // namespace HubPlatform
