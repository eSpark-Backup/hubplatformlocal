#include "networkd/monitoring/nm_openwrt_interface_monitor.h"
#include "networkd/monitoring/nm_monitor_utils.h"
#include "networkd/monitoring/nm_ping.h"
#include "networkd/resources/nm_resources.h"

#include "json_utils/json_utils.h"

#include <thread>

namespace HubPlatform {
namespace Network {
namespace {
const auto UpdatePeriod = std::chrono::seconds( 1 );
}  // namespace

OpenwrtInterfaceMonitor::OpenwrtInterfaceMonitor( Services::TaskLoop& taskLoop,
                                                  InterfaceInfoListener& interfaceChangeListener,
                                                  std::string interfaceName,
                                                  bool ping,
                                                  std::chrono::seconds pingPeriod,
                                                  IPVersion ipVersion )
: InterfaceMonitor( taskLoop,
                    interfaceChangeListener,
                    std::move( interfaceName ),
                    ping,
                    std::move( pingPeriod ),
                    ipVersion )
, m_updateTimer( m_taskLoop, [&]() { update(); } )
, m_pingTime( std::chrono::steady_clock::now() ) {
    m_updateTimer.setPeriodicMode( Services::PostponedTask::PeriodicMode::FixedIdleDuration );
}

void OpenwrtInterfaceMonitor::start() {
    m_updateTimer.start( std::chrono::seconds( 0 ), UpdatePeriod );
}

void OpenwrtInterfaceMonitor::stop() {
    m_updateTimer.stop();
}

bool OpenwrtInterfaceMonitor::isActive() {
    return m_updateTimer.isActive();
}

void OpenwrtInterfaceMonitor::update( bool silent ) {
    using namespace Resources::InterfaceInfo;

    std::lock_guard< std::recursive_mutex > lock( m_interfaceInfoMutex );

    auto ifaceInfo = getDynamicNetworkIfaceConfig( m_interfaceName );
    if( m_ping && ifaceInfo[ Fields::Status ] == Statuses::Up ) {
        if( json_get_str( m_interfaceInfo, Fields::Status ) != Statuses::Up ||
            std::chrono::steady_clock::now() >= m_pingTime ) {
            m_pingTime = std::chrono::steady_clock::now() + m_pingPeriod;
            ifaceInfo[ Fields::InternetAvailable ] = ping( ifaceInfo[ Fields::Id ], m_ipVersion );
        }
        else {
            ifaceInfo[ Fields::InternetAvailable ] = m_interfaceInfo[ Fields::InternetAvailable ];
        }
    }
    else {
        ifaceInfo[ Fields::InternetAvailable ] = false;
    }

    if( m_interfaceInfo != ifaceInfo ) {
        m_interfaceInfo = std::move( ifaceInfo );
        if( !silent ) {
            m_interfaceChangeListener.onNewInterfaceInfo( m_interfaceInfo );
        }
    }
}

const Json& OpenwrtInterfaceMonitor::manualUpdate() {
    std::lock_guard< std::recursive_mutex > lock( m_interfaceInfoMutex );

    m_pingTime = std::chrono::steady_clock::now();
    update();
    return m_interfaceInfo;
}

void OpenwrtInterfaceMonitor::setPingPeriod( std::chrono::seconds pingPeriod ) {
    std::lock_guard< std::recursive_mutex > lock( m_interfaceInfoMutex );

    m_pingTime = std::chrono::steady_clock::now();
    InterfaceMonitor::setPingPeriod( std::move( pingPeriod ) );
}

/*
Examples:
{
    "_id": "wlan0",
    "hwaddr": "8C:F7:10:E4:ED:90",
    "ipv4": {
        ...
    },
    "status": "up",
    "internetAvailable", true
}
{
    "status": "down",
    "internetAvailable", false
}
*/
const Json& OpenwrtInterfaceMonitor::getInfo() {
    std::lock_guard< std::recursive_mutex > lock( m_interfaceInfoMutex );

    if( m_interfaceInfo.empty() ) {
        m_pingTime = std::chrono::steady_clock::now();
        update( true );
    }

    return m_interfaceInfo;
}

bool OpenwrtInterfaceMonitor::isInternetAvailable() {
    using namespace Resources::InterfaceInfo;
    return json_get_bool( getInfo(), Fields::InternetAvailable );
}

}  // namespace Network
}  // namespace HubPlatform
