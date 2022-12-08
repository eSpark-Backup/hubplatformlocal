#include "networkd/interfaces/nm_client_wifi_interface.h"
#include "networkd/resources/nm_resources.h"

#include "json_utils/json_utils.h"
#include "jsonrpc/error_codes.h"
#include "jsonrpc/jsonrpc.h"

namespace HubPlatform {
namespace Network {

namespace {
const std::string CurrentSettingsId = "current_settings";
const std::string BackupSettingsId = "backup_settings";
const auto RescanPeriod = std::chrono::minutes( 1 );
}  // namespace

ClientWifiInterface::ClientWifiInterface( Services::TaskLoop& taskLoop,
                                          IDbClient& dbClient,
                                          const ProvisionDataUtils::ProvisionData& provisionData,
                                          NetworkConfigManager& configManager,
                                          WifiPassiveScanner& wifiPassiveScanner,
                                          const InterfaceMonitorFactory& interfaceMonitorFactory,
                                          InterfaceInfoListener& interfaceInfoListener )
: m_dbClient( dbClient )
, m_provisionData( provisionData )
, m_configManager( configManager )
, m_wifiPassiveScanner( wifiPassiveScanner )
, m_rescanTimer( taskLoop, [&]() { findNewChannelAndBssid(); } )
, m_interfaceInfoListener( interfaceInfoListener )
, m_settings( dbClient, Resources::ClientWifiDeviceName, EnablePolicy::Auto )
, m_monitor( interfaceMonitorFactory.createInterfaceMonitor( *this, Resources::ClientInterfaceName, true ) )
, m_trafficMonitor( Resources::ClientWifiDeviceName )
, m_connecting( false )
, m_enabled( false ) {}

void ClientWifiInterface::startMonitoring() {
    m_monitor->start();
}

void ClientWifiInterface::stopMonitoring() {
    m_monitor->stop();
    m_rescanTimer.stop();
}

bool ClientWifiInterface::hasSettings() const {
    auto network = getWirelessNetworkSettings( CurrentSettingsId );
    return !json_get_str( network, "ssid" ).empty();
}

Json ClientWifiInterface::getInfo() const {
    return getInfo( m_monitor->getInfo() );
}

Json ClientWifiInterface::getInfo( Json dynamicInfo ) const {
    using namespace Resources::InterfaceInfo;

    Json interfaceInfo = std::move( dynamicInfo );

    auto isUp = json_get_str( interfaceInfo, "status" ) == Statuses::Up;

    if( interfaceInfo.find( Fields::Ipv4 ) == interfaceInfo.end() ) {
        auto network = getNetworkConfig();
        interfaceInfo[ Fields::Ipv4 ] = network[ Fields::Ipv4 ];
    }

    interfaceInfo[ "wifi" ] = isUp ? getWirelessConfig() : JSON_EMPTY_OBJ;
    interfaceInfo[ "type" ] = Resources::WifiType;
    interfaceInfo[ Fields::Priority ] = Resources::WifiClientPriority;
    interfaceInfo[ Fields::Network ] = Resources::WanNetwork;
    interfaceInfo[ Fields::EnablePolicy ] = toString( getEnablePolicy() );

    // TODO: duplicate mac address from other interface

    // NOTE: Override interface id, originally it is not predictable, which is confusing
    interfaceInfo[ "_id" ] = Resources::ClientWifiDeviceName;

    return interfaceInfo;
}

const char* ClientWifiInterface::getName() const {
    return Resources::ClientWifiDeviceName;
}

bool ClientWifiInterface::isInternetAvailable() const {
    return m_monitor->isInternetAvailable();
}

void ClientWifiInterface::connect( int channel,
                                   const std::string& ssid,
                                   const std::string& bssid,
                                   const std::string& encryption,
                                   const std::string& key ) {
    using namespace Resources::InterfaceInfo;
    m_connecting = true;
    Json interfaceInfo;

    if( channel < 0 ) {
        Log::error( cc::error( "Can not connect to network because of invalid channel" ) );
        interfaceInfo = m_monitor->getInfo();
        setError( interfaceInfo, Errors::NETWORK_COULD_NOT_CONNECT );
        m_connecting = false;
        onNewInterfaceInfo( interfaceInfo );
        interfaceInfo.erase( Errors::ERROR_FIELD );
        onNewInterfaceInfo( std::move( interfaceInfo ) );
        return;
    }

    if( bssid.empty() ) {
        interfaceInfo = m_monitor->getInfo();
        setError( interfaceInfo, Errors::NETWORK_COULD_NOT_CONNECT );
        Log::error( cc::error( "Can not connect to network because of missing bssid" ) );
        m_connecting = false;
        onNewInterfaceInfo( interfaceInfo );
        interfaceInfo.erase( Errors::ERROR_FIELD );
        onNewInterfaceInfo( std::move( interfaceInfo ) );
        return;
    }

    stopMonitoring();
    backupWirelessNetworkSettings();
    setWirelessNetworkSettings( CurrentSettingsId,
                                JSON( "ssid" << ssid << "bssid" << bssid << "encryption" << encryption << "key" << key
                                             << "channel" << std::to_string( channel ) ) );

    if( !configure() ) {
        Log::error( cc::error( "Failed to configure" ) + cc::note( Resources::ClientWifiDeviceName ) );
        interfaceInfo = m_monitor->getInfo();
        setError( interfaceInfo, Errors::NETWORK_COULD_NOT_CONNECT );
        m_connecting = false;
        onNewInterfaceInfo( interfaceInfo );
        interfaceInfo.erase( Errors::ERROR_FIELD );
        onNewInterfaceInfo( std::move( interfaceInfo ) );
        return;
    }

    bool connected = false;
    bool hasInternet = false;
    auto connectionTimeout = std::chrono::steady_clock::now() + std::chrono::seconds( 25 );
    while( std::chrono::steady_clock::now() < connectionTimeout ) {
        interfaceInfo = m_monitor->manualUpdate();
        if( json_get_str( interfaceInfo, Fields::Status ) == Statuses::Up ) {
            if( !connected ) {
                Log::info( cc::info( "Connected to " ) + cc::note( ssid ) + cc::info( " network" ) );
            }
            connected = true;
        }
        hasInternet = json_get_bool( interfaceInfo, Fields::InternetAvailable );
        if( hasInternet ) {
            break;
        }
        std::this_thread::sleep_for( std::chrono::milliseconds( 500 ) );
    }

    bool ok = true;
    if( !connected ) {
        Log::error( cc::error( "Can not connect to network" ) );
        ok = false;
    }
    else if( !hasInternet ) {
        Log::error( cc::error( "Network have no internet access" ) );
        ok = false;
    }

    m_connecting = false;
    onNewInterfaceInfo( m_monitor->getInfo() );

    if( !ok ) {
        m_connecting = true;
        restoreWirelessNetworkSettings();
        configure();
        if( hasSettings() ) {
            connectionTimeout = std::chrono::steady_clock::now() + std::chrono::seconds( 25 );
            while( std::chrono::steady_clock::now() < connectionTimeout ) {
                const auto& info = m_monitor->manualUpdate();
                if( json_get_str( info, Fields::Status ) == Statuses::Up ) {
                    break;
                }
                std::this_thread::sleep_for( std::chrono::milliseconds( 500 ) );
            }
        }
        m_connecting = false;
        onNewInterfaceInfo( m_monitor->getInfo() );
    }

    if( hasSettings() ) {
        m_enabled = true;
    }

    startMonitoring();
}

bool ClientWifiInterface::reset() {
    stopMonitoring();
    setWirelessNetworkSettings( CurrentSettingsId, {} );
    setWirelessNetworkSettings( BackupSettingsId, {} );
    m_settings.reset();
    configure();
    startMonitoring();
    return true;
}

void ClientWifiInterface::backupWirelessNetworkSettings() {
    auto network = getWirelessNetworkSettings( CurrentSettingsId );
    setWirelessNetworkSettings( CurrentSettingsId, {} );
    setWirelessNetworkSettings( BackupSettingsId, std::move( network ) );
}

void ClientWifiInterface::restoreWirelessNetworkSettings() {
    auto network = getWirelessNetworkSettings( BackupSettingsId );
    setWirelessNetworkSettings( BackupSettingsId, {} );
    setWirelessNetworkSettings( CurrentSettingsId, std::move( network ) );
}

Json ClientWifiInterface::getWirelessNetworkSettings( const std::string& id ) const {
    static const Json EmptyNetwork = JSON( "ssid"
                                           << ""
                                           << "bssid"
                                           << ""
                                           << "encryption"
                                           << ""
                                           << "key"
                                           << "" );

    const auto query = JSON( "_id" << id );
    auto network = m_dbClient.findOne( WifiSettingsDBName, query, nullptr );

    if( !network.empty() ) {
        json_remove( network, "_id" );
        if( !json_get_str( network, "key" ).empty() ) {
            network[ "key" ] = m_provisionData.decryptSensitiveData( network[ "key" ] );
            if( json_get_str( network, "key" ).empty() ) {
                network = EmptyNetwork;
            }
        }
    }
    else {
        network = EmptyNetwork;
    }
    return network;
}

void ClientWifiInterface::setWirelessNetworkSettings( const std::string& id, Json network ) {
    if( network.empty() ) {
        const auto query = JSON( "_id" << id );
        m_dbClient.remove( WifiSettingsDBName, query );
    }
    else {
        network[ "_id" ] = id;
        if( !json_get_str( network, "key" ).empty() ) {
            network[ "key" ] = m_provisionData.encryptSensitiveData( network[ "key" ] );
        }
        m_dbClient.insert( WifiSettingsDBName, network );
    }
}

bool ClientWifiInterface::enable( bool force ) {
    if( m_enabled && !force )
        return true;

    Log::info( cc::info( "Enabling " ) + cc::note( Resources::ClientWifiDeviceName ) + cc::info( " interface" ) );
    m_enabled = true;
    if( !configure() )
        return false;

    startMonitoring();
    return true;
}

bool ClientWifiInterface::disable( bool force ) {
    if( !m_enabled && !force )
        return true;

    Log::info( cc::info( "Disabling " ) + cc::note( Resources::ClientWifiDeviceName ) + cc::info( " interface" ) );
    m_enabled = false;
    return m_configManager.configureClientWifiInterface( Resources::ClientInterfaceName );
}

bool ClientWifiInterface::configure() {
    const Json config = getSavedWirelessConfig();
    if( json_get_str( config[ "network" ], "ssid" ).empty() ) {
        return m_configManager.configureClientWifiInterface( Resources::ClientInterfaceName );
    }
    return m_configManager.configureClientWifiInterface( Resources::ClientInterfaceName,
                                                         makeSystemNetworkConfiguration( config ) );
}

Json ClientWifiInterface::getWirelessConfig() const {
    auto wifi = getUIWirelessConfiguration();
    if( wifi.empty() ) {
        wifi = getSavedWirelessConfig();
    }
    return wifi;
}

Json ClientWifiInterface::getSavedWirelessConfig() const {
    JsonObject wifi;
    wifi[ "network" ] = getWirelessNetworkSettings( CurrentSettingsId );
    wifi[ "mode" ] = Resources::ClientType;
    if( wifi[ "network" ].count( "channel" ) != 0 ) {
        wifi[ "channel" ] = wifi[ "network" ][ "channel" ];
        json_remove( wifi[ "network" ], "channel" );
    }
    wifi[ "region" ] = Resources::DefaultWifiRegion;

    return wifi;
}

Json ClientWifiInterface::getNetworkConfig() const {
    auto network = getUINetworkConfiguration();
    if( network.empty() ) {
        network = getDefaultNetworkConfig();
    }
    return network;
}

Json ClientWifiInterface::getDefaultNetworkConfig() const {
    JsonObject ipv4;
    ipv4[ "mode" ] = "dhcp";

    JsonObject interface;
    interface[ "ipv4" ] = std::move( ipv4 );

    return interface;
}

void ClientWifiInterface::onNewInterfaceInfo( Json dynamicInfo ) {
    using namespace Resources::InterfaceInfo;
    if( m_connecting ) {
        return;
    }

    bool hasError = ::hasError( dynamicInfo );
    bool connected = json_get_str( dynamicInfo, Fields::Status ) == Statuses::Up;
    bool shouldWork = hasSettings() && ( getEnablePolicy() != EnablePolicy::Off );
    if( !connected ) {
        if( shouldWork && !hasError ) {
            setError( dynamicInfo, Errors::NETWORK_COULD_NOT_CONNECT );
        }
    }
    else if( !json_get_bool( dynamicInfo, Fields::InternetAvailable ) ) {
        if( shouldWork && !hasError ) {
            setError( dynamicInfo, Errors::NETWORK_HAS_NO_INTERNET );
        }
    }

    m_interfaceInfoListener.onNewInterfaceInfo( getInfo( std::move( dynamicInfo ) ) );

    if( connected ) {
        m_rescanTimer.stop();
    }
    else {
        if( shouldWork ) {
            m_rescanTimer.start( RescanPeriod );
        }
    }
}

void ClientWifiInterface::findNewChannelAndBssid() {
    if( !hasSettings() || ( getEnablePolicy() == EnablePolicy::Off ) ) {
        m_rescanTimer.stop();
        return;
    }

    auto currentSettings = getWirelessNetworkSettings( CurrentSettingsId );
    auto ssid = json_get_str( currentSettings, "ssid" );

    LOGGER_DEBUG_RAW( cc::debug( "Trying to find new " ) + cc::note( ssid ) + cc::debug( " network channel/bssid" ) );
    auto scannedNetwork = m_wifiPassiveScanner.scanAndGetNetwork( Resources::ClientWifiDeviceName, ssid );
    if( scannedNetwork.m_channel == -1 ) {
        LOGGER_DEBUG_RAW( cc::debug( "Network not found" ) );
        return;
    }

    auto newSettings = currentSettings;
    newSettings[ "channel" ] = std::to_string( scannedNetwork.m_channel );
    newSettings[ "bssid" ] = scannedNetwork.m_bssid;

    if( newSettings == currentSettings ) {
        Log::warn( cc::warn( "Looks like " ) + cc::note( ssid ) + cc::warn( " network channel/bssid did not change" ) );
        return;
    }

    Log::info( cc::debug( "Found changed " ) + cc::note( ssid ) + cc::debug( " network" ) );
    LOGGER_DEBUG_RAW( cc::debug( "Network info changes: " ) + cc::j( jsonDiff( currentSettings, newSettings ) ) );

    setWirelessNetworkSettings( CurrentSettingsId, newSettings );

    if( !configure() ) {
        Log::error( cc::error( "Failed to configure" ) + cc::note( Resources::ClientWifiDeviceName ) );
        return;
    }
}

EnablePolicy ClientWifiInterface::getEnablePolicy() const {
    return m_settings.getEnablePolicy();
}

bool ClientWifiInterface::setEnablePolicy( EnablePolicy policy ) {
    if( m_settings.setEnablePolicy( policy ) )
        onNewInterfaceInfo( getInfo() );
    return true;
}

ClientWifiConfiguration ClientWifiInterface::makeSystemNetworkConfiguration( const Json& config ) const {
    ClientWifiConfiguration configuration;

    auto channel = json_get_str( config, "channel" );
    configuration.m_channel = channel.empty() ? 0 : std::stoi( channel );

    if( json_contains( config, "network" ) ) {
        configuration.m_ssid = json_get_str( config[ "network" ], "ssid" );
        configuration.m_encryption = json_get_str( config[ "network" ], "encryption" );
        configuration.m_key = json_get_str( config[ "network" ], "key" );
        configuration.m_bssid = json_get_str( config[ "network" ], "bssid" );
    }

    return configuration;
}

Json ClientWifiInterface::getUINetworkConfiguration() const {
    using namespace Resources::InterfaceInfo;

    auto configuration = m_configManager.getClientWifiConfiguration( Resources::ClientInterfaceName );
    return JSON( Fields::Ipv4 << configuration.m_mode.toJson() );
}

Json ClientWifiInterface::getUIWirelessConfiguration() const {
    using namespace Resources::InterfaceInfo;

    auto configuration = m_configManager.getClientWifiConfiguration( Resources::ClientInterfaceName );
    Json config;
    Json& networkConfig = config[ Fields::Network ];

    if( configuration.isEmtpy() ) {
        return JSON_EMPTY_OBJ;
    }

    config[ "region" ] = Resources::DefaultWifiRegion;
    config[ "channel" ] = std::to_string( configuration.m_channel );
    config[ "mode" ] = Resources::ClientType;

    networkConfig[ "ssid" ] = configuration.m_ssid;
    networkConfig[ "bssid" ] = configuration.m_bssid;
    networkConfig[ "key" ] = configuration.m_key;
    networkConfig[ "encryption" ] = configuration.m_encryption;

    return config;
}

InterfaceTrafficMonitor::TrafficStatistic ClientWifiInterface::getTrafficStatistics() const {
    return m_trafficMonitor.getTrafficStatistics();
}

}  // namespace Network
}  // namespace HubPlatform
