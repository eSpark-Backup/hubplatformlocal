#pragma once

#include "networkd/monitoring/nm_interface_change_listener.h"
#include "networkd/monitoring/nm_ip_version.h"

#include "service_interfaces/task_loop.h"
#include "service_interfaces/task_loop_timer.h"

#include "json_utils/json_fwd.h"

#include <chrono>

namespace HubPlatform {
namespace Network {

class InterfaceMonitor {
public:
    InterfaceMonitor( Services::TaskLoop& taskLoop,
                      InterfaceInfoListener& interfaceInfoListener,
                      std::string interfaceName,
                      bool ping,
                      std::chrono::seconds pingPeriod,
                      IPVersion ipVersion = IPVersion::IPv4 );

    virtual ~InterfaceMonitor() = default;

    virtual void start() = 0;
    virtual void stop() = 0;
    virtual bool isActive() = 0;

    virtual const Json& manualUpdate() = 0;
    virtual void setPingPeriod( std::chrono::seconds pingPeriod );
    virtual void setIPVersion( IPVersion ipVersion );

    virtual const Json& getInfo() = 0;
    virtual bool isInternetAvailable() = 0;

protected:
    Services::TaskLoop& m_taskLoop;
    InterfaceInfoListener& m_interfaceChangeListener;
    std::string m_interfaceName;
    bool m_ping;
    std::chrono::seconds m_pingPeriod;
    IPVersion m_ipVersion;
};

}  // namespace Network
}  // namespace HubPlatform
