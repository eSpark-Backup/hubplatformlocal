#pragma once

#include "networkd/interfaces/nm_network_interface.h"
#include "networkd/interfaces/nm_ethernet_interface.h"
#include "networkd/interfaces/nm_client_wifi_interface.h"
#include "networkd/interfaces/nm_ap_wifi_interface.h"
#include "networkd/scan/nm_wifi_passive_scanner.h"
#include "networkd/routes/nm_routes_manager.h"
#include "networkd/connection_service/nm_wifi_connection_service.h"
#include "networkd/monitoring/nm_interface_change_listener.h"
#include "networkd/network_change_listener.h"
#include "networkd/gsm/nm_gsm_manager.h"
#include "networkd/configuration/nm_config_file_manager.h"

#include "database_client/idbclient.h"
#include "json_utils/json_fwd.h"
#include "cored/remote_interface/cored_interface.h"

#include <functional>

namespace HubPlatform {
namespace CoreDRemoteInterface {
class CoreDRemoteInterface;
}
namespace Network {

class NetworkManager final : public InterfaceInfoListener, public WifiConnectionService::ConnectionDelegate {
public:
    NetworkManager( IDbClient& dbClient,
                    const ProvisionDataUtils::ProvisionData& provisionData,
                    NetworkConfigManager& configManager,
                    const NetworkConfigFileManager& configFileManager,
                    Services::TaskLoop& taskLoop,
                    NetworkChangeListener& networkChangeListener,
                    WifiScanResultListener& wifiScanResultListener,
                    const CoreDRemoteInterface::CoreDInterface& core );

    GSMManager& getGSMManager();
    const GSMManager& getGSMManager() const;

    void connectToWiFi( const std::string& ssid,
                        const std::string& bssid,
                        const std::string& encryption,
                        const std::string& key,
                        bool temporaryDisconnectAp = false );

    bool resetInterface( const std::string& interfaceId );
    bool resetWiFi();

    bool startScan( const std::string& interfaceId /*, scanResultListener*/ );
    bool stopScan( const std::string& interfaceId );

    Errors::Error setEnablePolicy( const std::string& interfaceId, const std::string& enablePolicy );
    Errors::Error setCustomSettings( const std::string& interfaceId, const Json& settings );

    void onForceBroadcastSettingsChanged();

    void forEachNetworkInterface( const std::function< void( NetworkInterface& ) >& callback );

private:
    void onInterfaceInternetAvailabilityChanged( bool internetAvailable );
    void onNonMobileInternetAvailabilityChanged( bool internetAvailable );
    void checkIfApInterfaceShouldBeUp();
    void onNewInterfaceInfo( Json info ) override;
    void onNewInterfaceAdded( const std::string& interfaceName ) override;
    void onInterfaceRemoved( const std::string& interfaceName ) override;

private:
    bool scanWifi( std::string& jsonResult ) override;
    void connect( std::string ssid, std::string bssid, std::string encryption, std::string key ) override;
    void temporaryShutDownAP() override;
    void restart() override;

    void checkAndFixRoutes( bool notifyOnRoutesChange = true );
    void setActiveInterface( const std::string& activeInterface );
    std::string getActiveInterface();
    void notifyActiveInterfaceChanged( const std::string& activeInterface );
    void fixNetwork();
    void forceBroadcast();
    void updateAdditionalPingServers();
    std::vector< Json > getNetworkInterfacesVector( bool requireWifiApInterface = true ) const;

    void manageConnectionService();

    bool isWifiInternetAvailable() const override;
    bool isInternetAvailable() const;
    bool isNonMobileInternetAvailable() const;

    NetworkChangeListener::NetworkInfo getActiveNetwork( std::vector< Json > interfaces ) const;

private:
    IDbClient& m_dbClient;
    NetworkConfigManager& m_configManager;
    const NetworkConfigFileManager& m_configFileManager;
    Services::TaskLoop& m_taskLoop;
    NetworkChangeListener& m_networkChangeListener;
    const CoreDRemoteInterface::CoreDInterface& m_core;
    Services::TaskLoopTimer m_routesCheckTimer;
    Services::TaskLoopTimer m_apInterfaceCheckTimer;
    Services::TaskLoopTimer m_fixNetworkTimer;
    Services::TaskLoopTimer m_forceBroadcastTimer;
    std::unique_ptr< RoutesManager > m_routesManager;
    WifiPassiveScanner m_wifiPassiveScanner;

    EthernetInterface m_ethernetInterface;
    ClientWifiInterface m_clientWifiInterface;
    ApWifiInterface m_apWifiInterface;
    GSMManager m_gsmManager;

    WifiConnectionService m_wifiConnectionService;
};

}  // namespace Network
}  // namespace HubPlatform
