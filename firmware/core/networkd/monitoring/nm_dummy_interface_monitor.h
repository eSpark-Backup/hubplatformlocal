#pragma once

#include "networkd/monitoring/nm_interface_monitor.h"
#include "networkd/monitoring/nm_interface_change_listener.h"
#include "networkd/utils/nm_utils.h"

#include "service_interfaces/task_loop.h"
#include "service_interfaces/task_loop_timer.h"
#include "system_utils/system.h"

#include "json_utils/json_utils.h"

#include "tiny/include/tiny/tiny_consolecolors.h"
#include "json_utils/json_utils.h"

namespace HubPlatform {
namespace Network {

class DummyInterfaceMonitor : public InterfaceMonitor {
public:
    DummyInterfaceMonitor( Services::TaskLoop& taskLoop,
                           InterfaceInfoListener& interfaceInfoListener,
                           std::string interfaceName )
    : InterfaceMonitor( taskLoop, interfaceInfoListener, std::move( interfaceName ), false, std::chrono::seconds( 0 ) )
    , m_info( JSON( "_id" << m_interfaceName << "status"
                          << "down"
                          << "internetAvailable" << false ) )
    , m_active( false ) {}

    void start() override {
        m_active = true;
        m_interfaceChangeListener.onNewInterfaceInfo( getInfo() );
    };

    void stop() override {
        m_active = false;
    };

    bool isActive() override {
        return m_active;
    };

    const Json& manualUpdate() override {
        return getInfo();
    };

    const Json& getInfo() override {
        using namespace Utils;

        if( m_interfaceName == Resources::EthernetInterfaceName ) {
            static Json info = m_info;

            auto routeDevice = Utils::getDefaultRouteDevice();
            auto interfaces = Utils::getSystemInterfaces();

            if( !routeDevice.empty() ) {
                info[ "status" ] = "up";
                info[ "internetAvailable" ] = true;

                for( const auto& interface : interfaces ) {
                    if( interface.m_name == routeDevice && interface.m_type == InterfaceInfo::Type::IPV4 ) {
                        info[ "ipv4" ][ "ip" ] = Utils::sockadrToString( interface.m_ipAddress );
                        info[ "ipv4" ][ "mask" ] = Utils::sockadrToString( interface.m_mask );
                        std::string gateway = "error";
                        try {
                            gateway = System::getAwkOutput( "route -n | grep 'UG[ \t]' | awk '{print $2}'" );
                        }
                        catch( const System::PopenFailed& ex ) {
                            LOGGER_ERROR( cc::error( "Failed to parse routes: " ) + ex.what() );
                        }
                        info[ "ipv4" ][ "gateway" ] = gateway;
                        info[ "ipv4" ][ "mode" ] = "dhcp";
                    }
                }
            }
            else {
                info[ "status" ] = "down";
                info[ "internetAvailable" ] = false;
                info[ "ipv4" ] = JSON_EMPTY_OBJ;
                info[ "ipv4" ][ "mode" ] = "dhcp";
            }

            if( m_info != info ) {
                m_info = info;
                m_interfaceChangeListener.onNewInterfaceInfo( m_info );
            }
        }

        return m_info;
    };

    bool isInternetAvailable() override {
        return true;
    };

private:
    Json m_info;
    bool m_active;
};

}  // namespace Network
}  // namespace HubPlatform
