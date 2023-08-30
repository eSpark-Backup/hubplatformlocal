#include "networkd/monitoring/nm_interface_monitor.h"

namespace HubPlatform {
namespace Network {

InterfaceMonitor::InterfaceMonitor( Services::TaskLoop& taskLoop,
                                    InterfaceInfoListener& interfaceInfoListener,
                                    std::string interfaceName,
                                    bool ping,
                                    std::chrono::seconds pingPeriod,
                                    IPVersion ipVersion )
: m_taskLoop( taskLoop )
, m_interfaceChangeListener( interfaceInfoListener )
, m_interfaceName( std::move( interfaceName ) )
, m_ping( ping )
, m_pingPeriod( std::move( pingPeriod ) )
, m_ipVersion( ipVersion ) {}

void InterfaceMonitor::setPingPeriod( std::chrono::seconds pingPeriod ) {
    m_pingPeriod = std::move( pingPeriod );
}

void InterfaceMonitor::setIPVersion( IPVersion ipVersion ) {
    m_ipVersion = ipVersion;
}

}  // namespace Network
}  // namespace HubPlatform
