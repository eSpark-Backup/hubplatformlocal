#pragma once

#include "networkd/monitoring/nm_ip_version.h"
#include "json_utils/json_fwd.h"

namespace HubPlatform {
namespace Network {

void setAdditionalServers( std::vector< std::string > servers );
std::vector< std::string > getAdditionalServers();
bool ping( const std::string& interfaceName, IPVersion ipVersion );

}  // namespace Network
}  // namespace HubPlatform
