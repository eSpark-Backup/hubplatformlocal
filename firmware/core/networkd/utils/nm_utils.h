#pragma once

#include <string>
#include <vector>
#include <memory>

#include <sys/socket.h>

namespace HubPlatform {
namespace Network {
namespace Utils {

struct InterfaceInfo {
    enum class Type { IPV4 = 0, IPV6 };

    Type m_type;
    std::string m_name;
    sockaddr m_ipAddress;
    sockaddr m_mask;
};

std::string sockadrToString( const sockaddr& sa );
std::vector< InterfaceInfo > getSystemInterfaces();
std::string getDefaultRouteDevice();
std::string purgeCRSymbol( const std::string& atCommand );

}  // namespace Utils
}  // namespace Network
}  // namespace HubPlatform
