#pragma once

#include "networkd/routes/nm_routes_manager.h"

namespace HubPlatform {
namespace Network {

class OpenwrtRoutesManager : public RoutesManager {
public:
    std::string checkAndFixRoutes( const std::vector< Json >& interfaces ) override;
};

}  // namespace Network
}  // namespace HubPlatform
