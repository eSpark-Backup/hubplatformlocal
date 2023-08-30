#include "networkd/monitoring/nm_interface_monitor_factory.h"
#include "networkd/monitoring/nm_dummy_interface_monitor.h"
#include "networkd/monitoring/nm_openwrt_interface_monitor.h"

namespace HubPlatform {
namespace Network {

InterfaceMonitorFactory::InterfaceMonitorFactory( Services::TaskLoop& taskLoop )
: m_taskLoop( taskLoop ) {}

std::unique_ptr< InterfaceMonitor > InterfaceMonitorFactory::createInterfaceMonitor(
    InterfaceInfoListener& interfaceInfoListener,
    std::string interfaceName,
    bool ping,
    std::chrono::seconds pingPeriod ) const {
#if defined( __HUB_TYPE_LOCAL__ ) || defined( __HUB_TYPE_VHUB__ )
    return std::make_unique< DummyInterfaceMonitor >( m_taskLoop, interfaceInfoListener, std::move( interfaceName ) );
#else
    return std::make_unique< OpenwrtInterfaceMonitor >(
        m_taskLoop, interfaceInfoListener, std::move( interfaceName ), ping, std::move( pingPeriod ) );
#endif
}

}  // namespace Network
}  // namespace HubPlatform
