#pragma once

#include "networkd/monitoring/nm_interface_monitor.h"
#include "networkd/resources/nm_resources.h"

#include "service_interfaces/task_loop.h"

namespace HubPlatform {
namespace Network {

class InterfaceMonitorFactory {
public:
    InterfaceMonitorFactory( Services::TaskLoop& taskLoop );

    std::unique_ptr< InterfaceMonitor > createInterfaceMonitor(
        InterfaceInfoListener& interfaceInfoListener,
        std::string interfaceName,
        bool ping,
        std::chrono::seconds pingPeriod = std::chrono::seconds( Resources::PingPeriod ) ) const;

private:
    Services::TaskLoop& m_taskLoop;
};

}  // namespace Network
}  // namespace HubPlatform
