#pragma once

#include "networkd/monitoring/nm_interface_monitor.h"

#include "mutex"

namespace HubPlatform {
namespace Network {

class OpenwrtInterfaceMonitor : public InterfaceMonitor {
public:
    OpenwrtInterfaceMonitor( Services::TaskLoop& taskLoop,
                             InterfaceInfoListener& interfaceChangeListener,
                             std::string interfaceName,
                             bool ping,
                             std::chrono::seconds pingPeriod,
                             IPVersion ipVersion = IPVersion::IPv4 );

    void start() override;
    void stop() override;
    bool isActive() override;

    const Json& manualUpdate() override;
    void setPingPeriod( std::chrono::seconds pingPeriod ) override;

    const Json& getInfo() override;
    bool isInternetAvailable() override;

private:
    void update( bool silent = false );

private:
    Services::TaskLoopTimer m_updateTimer;

    std::recursive_mutex m_interfaceInfoMutex;
    Json m_interfaceInfo;
    std::chrono::steady_clock::time_point m_pingTime;
};

}  // namespace Network
}  // namespace HubPlatform
