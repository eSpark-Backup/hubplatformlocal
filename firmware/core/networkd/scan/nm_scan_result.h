#pragma once

#include <string>
#include <map>

namespace HubPlatform {
namespace Network {

struct NetworkItem {
    int m_channel = -1;
    std::string m_ssid;
    std::string m_bssid;
    std::string m_security;
    int m_signalPercentLevel = 0;

    bool isValid() const {
        return !m_ssid.empty() && !m_bssid.empty() && m_channel != -1;
    }
};

using NetworkMap = std::map< std::string /*ssid*/, NetworkItem >;

}  // namespace Network
}  // namespace HubPlatform
