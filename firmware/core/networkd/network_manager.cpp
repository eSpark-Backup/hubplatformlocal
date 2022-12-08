#include "networkd/network_manager.h"
#include "networkd/resources/nm_resources.h"
#if defined( __HUB_TYPE_LOCAL__ ) || defined( __HUB_TYPE_VHUB__ )
#include "networkd/routes/nm_dummy_routes_manager.h"
#else
#include "networkd/routes/nm_openwrt_routes_manager.h"
#endif
#include "networkd/monitoring/nm_ping.h"
#include "provision_data_utils/provision_data_manager.h"
#include "database_client/db_tables_names.h"
#include "database_client/dbqueryparser.h"
#include "system_utils/system.h"
#include "json_utils/json_utils.h"
#include "jsonrpc/error_codes.h"

#include <future>

namespace HubPlatform {
namespace Network {
namespace {
const auto RoutesCheckPeriod = std::chrono::seconds( 10 );
}  // namespace

NetworkManager::NetworkManager( IDbClient& dbClient,
                                const ProvisionDataUtils::ProvisionData& provisionData,
                                NetworkConfigManager& configManager,
                                const NetworkConfigFileManager& configFileManager,
                                Services::TaskLoop& taskLoop,
                                NetworkChangeListener& networkChangeListener,
                                WifiScanResultListener& wifiScanResultListener,
                                const CoreDRemoteInterface::CoreDInterface& core )
: m_dbClient( dbClient )
, m_configManager( configManager )
, m_configFileManager( configFileManager )
, m_taskLoop( taskLoop )
, m_networkChangeListener( networkChangeListener )
, m_core( core )
, m_routesCheckTimer( m_taskLoop, [this]() { checkAndFixRoutes(); } )
, m_apInterfaceCheckTimer( m_taskLoop, [this]() { checkIfApInterfaceShouldBeUp(); } )
, m_fixNetworkTimer( m_taskLoop, [this]() { fixNetwork(); } )
, m_forceBroadcastTimer( m_taskLoop, [this]() { forceBroadcast(); } )
#if defined( __HUB_TYPE_LOCAL__ ) || defined( __HUB_TYPE_VHUB__ )
, m_routesManager( std::make_unique< DummyRoutesManager >() )
#else
, m_routesManager( std::make_unique< OpenwrtRoutesManager >() )
#endif
, m_wifiPassiveScanner( wifiScanResultListener )
, m_ethernetInterface( m_configManager, InterfaceMonitorFactory( taskLoop ), *this )
, m_clientWifiInterface( m_taskLoop,
                         m_dbClient,
                         provisionData,
                         m_configManager,
                         m_wifiPassiveScanner,
                         InterfaceMonitorFactory( taskLoop ),
                         *this )
, m_apWifiInterface( m_dbClient, provisionData, m_configManager, InterfaceMonitorFactory( taskLoop ), *this )
, m_gsmManager( dbClient,
                provisionData,
                m_configManager,
                m_configFileManager.getMobileConfiguration(),
                InterfaceMonitorFactory( taskLoop ),
                *this,
                taskLoop )
, m_wifiConnectionService( *this ) {
    updateAdditionalPingServers();
    if( getActiveInterface().empty() ) {
        setActiveInterface( Resources::EthernetDeviceName );
    }
    if( !m_dbClient.has( TempNetworkDynamicInfoDBName,
                         JSON( db::ID_FIELD_NAME << Resources::HubInternetAvailabillityId ) ) ) {
        m_dbClient.insert(
            TempNetworkDynamicInfoDBName,
            JSON( db::ID_FIELD_NAME << Resources::HubInternetAvailabillityId << "internetAvailable" << false ) );
    }

    onNewInterfaceInfo( m_ethernetInterface.getInfo() );
    onNewInterfaceInfo( m_clientWifiInterface.getInfo() );
    onNewInterfaceInfo( m_apWifiInterface.getInfo() );

    {
        auto transaction = m_configManager.startWirelessConfigurationTransaction();

        m_configManager.prepareEnvironment();

        m_ethernetInterface.startMonitoring();

        if( m_clientWifiInterface.hasSettings() && m_clientWifiInterface.getEnablePolicy() != EnablePolicy::Off ) {
            m_clientWifiInterface.enable( true );
            if( m_apWifiInterface.getEnablePolicy() != EnablePolicy::Off ) {
                m_apWifiInterface.enable( true );
            }
            else {
                m_apWifiInterface.disable( true );
                // If we have no ethernet connection, give some time for client Wi-Fi to start
                m_apInterfaceCheckTimer.start( std::chrono::seconds( 60 ) );
            }
        }
        else {
            m_clientWifiInterface.disable( true );

            if( m_apWifiInterface.getEnablePolicy() != EnablePolicy::Off || !isNonMobileInternetAvailable() ) {
                m_apWifiInterface.enable( true );
            }
            else {
                m_apWifiInterface.disable( true );
            }
        }

        manageConnectionService();

        /* openwrt network manager (netifd) has no time to get to a state where it can handle requests we make
         * using a 'wifi' command (to ask it to apply our settings to network device drivers),
         * so put a delay here to give it a time */
        std::this_thread::sleep_for( std::chrono::seconds( 3 ) );
    }

    m_taskLoop
        .createPostponedTask( [=]( Services::PostponedTask& ) {
            // give physical modems time to "get over it" between reboots
            getGSMManager().start();
            getGSMManager().restoreDynamicInterfaces();
        } )
        ->start( std::chrono::seconds( 15 ), false );

    // NOTE: sometimes on failed attempt to connect to WiFi if backup WiFi is also bad,
    //       we do not get event that internet availability changed, but rotes become broken.
    //       So just in case periodically check and fix routes
    m_routesCheckTimer.start( std::chrono::seconds( 0 ), RoutesCheckPeriod );

    onForceBroadcastSettingsChanged();
}

GSMManager& NetworkManager::getGSMManager() {
    return m_gsmManager;
}

const GSMManager& NetworkManager::getGSMManager() const {
    return m_gsmManager;
}

void NetworkManager::connectToWiFi( const std::string& ssid,
                                    const std::string& bssid,
                                    const std::string& encryption,
                                    const std::string& key,
                                    bool temporaryDisconnectAp ) {
    using namespace Resources::InterfaceInfo;

    if( m_clientWifiInterface.getEnablePolicy() == EnablePolicy::Off ) {
        LOGGER_WARN_RAW(
            cc::warn( "Client wifi (STA) interface is turned off, reject the request to establish a connection" ) );
        return;
    }

    if( m_apWifiInterface.getInfo()[ Fields::Status ] != Statuses::Up &&
        m_clientWifiInterface.getInfo()[ Fields::Status ] != Statuses::Up ) {
        m_apWifiInterface.enable();
        std::this_thread::sleep_for( std::chrono::seconds( 5 ) );
    }

    LOGGER_DEBUG_RAW( cc::debug( "Trying to detect wireless network parameters" ) );
    const auto scannedNetwork = m_wifiPassiveScanner.scanAndGetNetwork( Resources::ClientWifiDeviceName, ssid );
    if( scannedNetwork.isValid() ) {
        if( !bssid.empty() && bssid != scannedNetwork.m_bssid ) {
            LOGGER_WARN_RAW( cc::warn( "Wireless network bssid was changed from " ) + cc::note( bssid ) +
                             cc::warn( " to " ) + cc::note( scannedNetwork.m_bssid ) );
        }

        LOGGER_INFO_RAW( cc::info( "Will try to connect to a wireless network with the next parameters, ssid " ) +
                         cc::note( scannedNetwork.m_ssid ) + cc::info( ", bssid " ) +
                         cc::note( scannedNetwork.m_bssid ) + cc::info( ", channel " ) +
                         cc::num10( scannedNetwork.m_channel ) );
    }
    else {
        LOGGER_WARN_RAW( cc::warn( "There are no visible wireless networks with a specified ssid nearby" ) );
    }

    if( temporaryDisconnectAp ||
        ( m_apWifiInterface.getEnablePolicy() == EnablePolicy::Off && isNonMobileInternetAvailable() ) ) {
        m_apWifiInterface.disable();
    }

    m_clientWifiInterface.connect( scannedNetwork.m_channel, ssid, scannedNetwork.m_bssid, encryption, key );

    if( temporaryDisconnectAp ) {
        if( m_apWifiInterface.getEnablePolicy() != EnablePolicy::Off || !isNonMobileInternetAvailable() ) {
            m_apWifiInterface.enable();
        }
    }
    manageConnectionService();
}

bool NetworkManager::resetInterface( const std::string& interfaceId ) {
    if( interfaceId.empty() ) {  // no interface - reset all network settings
        resetWiFi();
        getGSMManager().resetGSMInterfaces();
    }
    else if( interfaceId == Resources::ClientWifiDeviceName ) {
        m_clientWifiInterface.reset();
        manageConnectionService();
    }
    else if( interfaceId == Resources::ApWifiDeviceName ) {
        m_apWifiInterface.reset();
        /* TODO: wifi ap goes up if we have no connection to a LAN, should we prevent this
         *       if we have added a hub to an account and have an access to the hub via JSON RPC API?
         *       (we may have an LTE connection) */
        if( m_apWifiInterface.getEnablePolicy() != EnablePolicy::Off || !isNonMobileInternetAvailable() ) {
            m_apWifiInterface.enable();
        }
        else {
            m_apWifiInterface.disable();
        }

        manageConnectionService();
    }
    else {
        try {
            getGSMManager().getGSMInterface( interfaceId ).reset();
        }
        // this may happen if interface was removed after we checked its existence in ha-uid.handleResetRequest
        catch( const InvalidGSMInterface& ex ) {
            Log::error( cc::error( "Failed to reset non-existing gsm interface, " ) + cc::note( ex.what() ) );
        }
        catch( ... ) {
            Log::error( cc::error( "Failed to reset non-existing gsm interface" ) );
        }
    }
    return true;
}

bool NetworkManager::resetWiFi() {
    auto transaction = m_configManager.startWirelessConfigurationTransaction();
    if( !m_clientWifiInterface.reset() )
        return false;

    if( !m_apWifiInterface.reset() )
        return false;

    if( m_apWifiInterface.getEnablePolicy() != EnablePolicy::Off || !isNonMobileInternetAvailable() ) {
        m_apWifiInterface.enable();
    }
    else {
        m_apWifiInterface.disable();
    }

    manageConnectionService();
    return true;
}

bool NetworkManager::startScan( const std::string& /*interfaceId*/ ) {
    using namespace Resources::InterfaceInfo;
    if( m_apWifiInterface.getInfo()[ Fields::Status ] != Statuses::Up &&
        m_clientWifiInterface.getInfo()[ Fields::Status ] != Statuses::Up ) {
        m_apWifiInterface.enable();

        // Wait until AP enabled
        m_taskLoop
            .createPostponedTask(
                [=]( Services::PostponedTask& ) { m_wifiPassiveScanner.startScan( Resources::ClientWifiDeviceName ); } )
            ->start( std::chrono::seconds( 5 ), false );
        return true;
    }
    // NOTE: currently we support scanning wifi networks only on one interface.
    m_wifiPassiveScanner.startScan( Resources::ClientWifiDeviceName );
    return true;
}

bool NetworkManager::stopScan( const std::string& /*interfaceId*/ ) {
    m_wifiPassiveScanner.stopScan( Resources::ClientWifiDeviceName );
    if( m_apWifiInterface.getEnablePolicy() == EnablePolicy::Off && isNonMobileInternetAvailable() ) {
        m_apWifiInterface.disable();
    }
    return true;
}

Errors::Error NetworkManager::setEnablePolicy( const std::string& interfaceId, const std::string& enablePolicy ) {
    auto policy = enablePolicyFromString( enablePolicy, EnablePolicy::Auto );

    // TODO: add correct support for Auto enable policy for wan interfaces

    if( interfaceId == Resources::EthernetDeviceName ) {
        if( !m_ethernetInterface.setEnablePolicy( policy ) )
            return Errors::NETWORK_OPERATION_PROHIBITED;
        return Errors::OK;
    }
    else if( interfaceId == Resources::ApWifiDeviceName ) {
        if( !m_apWifiInterface.setEnablePolicy( policy ) )
            return Errors::NETWORK_OPERATION_PROHIBITED;

        if( policy == EnablePolicy::Off && isNonMobileInternetAvailable() ) {
            m_apWifiInterface.disable();
        }
        else {
            m_apWifiInterface.enable();
        }

        manageConnectionService();
        return Errors::OK;
    }
    else if( interfaceId == Resources::ClientWifiDeviceName ) {
        if( !m_clientWifiInterface.setEnablePolicy( policy ) )
            return Errors::NETWORK_OPERATION_PROHIBITED;

        if( policy == EnablePolicy::Off ) {
            m_clientWifiInterface.disable();
        }
        else {
            if( m_clientWifiInterface.hasSettings() ) {
                m_clientWifiInterface.enable();
            }
        }

        manageConnectionService();
        return Errors::OK;
    }
    else {
        try {
            auto& interface = getGSMManager().getGSMInterface( interfaceId );
            if( !interface.setEnablePolicy( policy ) )
                return Errors::NETWORK_OPERATION_PROHIBITED;
            return Errors::OK;
        }
        catch( ... ) {
            return Errors::NETWORK_INVALID_INTERFACE;
        }
    }

    return Errors::NETWORK_INVALID_INTERFACE;
}

Errors::Error NetworkManager::setCustomSettings( const std::string& interfaceId, const Json& settings ) {
    bool result = false;

    forEachNetworkInterface( [&]( NetworkInterface& interface ) {
        if( interface.getName() == interfaceId ) {
            result = interface.setCustomSettings( settings );
        }
    } );

    if( result ) {
        if( interfaceId == std::string( Resources::ApWifiDeviceName ) &&
            m_apWifiInterface.getEnablePolicy() == EnablePolicy::On ) {
            // restart ap with new settings
            m_apWifiInterface.enable( true );
        }
    }

    return result ? Errors::OK : Errors::NETWORK_SETTINGS_SET_FAILED;
}

void NetworkManager::onForceBroadcastSettingsChanged() {
    auto periodSetting = m_core.findSetting( Resources::Settings::ModemSignalQualityPeriod );
    auto forceBroadcastPeriod = std::chrono::minutes( periodSetting ? periodSetting->getIntValue() : 0 );
    if( forceBroadcastPeriod.count() ) {
        LOGGER_INFO_RAW( cc::info( "Enabling periodic modem signal quality broadcasts each " ) +
                         cc::num10( forceBroadcastPeriod.count() ) + cc::info( " minute(s)" ) );
        m_forceBroadcastTimer.start( std::chrono::seconds( 0 ), forceBroadcastPeriod );
    }
    else {
        LOGGER_INFO_RAW( cc::info( "Disabling periodic modem signal quality broadcasts" ) );
        m_forceBroadcastTimer.stop();
    }
}

void NetworkManager::manageConnectionService() {
    if( !m_clientWifiInterface.hasSettings() && ( m_clientWifiInterface.getEnablePolicy() != EnablePolicy::Off ) &&
        ( ( m_apWifiInterface.getEnablePolicy() != EnablePolicy::Off ) || !isNonMobileInternetAvailable() ) ) {
        m_wifiConnectionService.start();
    }
    else {
        m_wifiConnectionService.stop();
    }
}

bool NetworkManager::isWifiInternetAvailable() const {
    return m_clientWifiInterface.isInternetAvailable();
}

bool NetworkManager::isInternetAvailable() const {
    auto internetAvailableJson = m_dbClient.findOne(
        TempNetworkDynamicInfoDBName, JSON( db::ID_FIELD_NAME << Resources::HubInternetAvailabillityId ), nullptr );
    return json_get_bool( internetAvailableJson, "internetAvailable" );
}

bool NetworkManager::isNonMobileInternetAvailable() const {
    return m_ethernetInterface.isInternetAvailable() || m_clientWifiInterface.isInternetAvailable();
}

NetworkChangeListener::NetworkInfo NetworkManager::getActiveNetwork( std::vector< Json > interfaces ) const {
    std::sort( interfaces.begin(), interfaces.end(), []( const Json& a, const Json& b ) {
        return json_get_int( a, "priority" ) < json_get_int( b, "priority" );
    } );

    NetworkChangeListener::NetworkInfo network;
    for( const auto& interface : interfaces ) {
        auto interfaceName = json_get_str( interface, "_id" );

        if( json_get_str( interface, "status" ) != "up" ) {
            continue;
        }
        if( !json_get( interface, std::vector< Json >{ "ipv4", "ip" }, network.ipv4 ) ) {
            Log::error( cc::error( "No network ip address found, interface: " ) + cc::note( interfaceName ) );
            continue;
        }
        if( !json_get( interface, "type", network.type ) ) {
            Log::error( cc::error( "No network type found" ) );
            continue;
        }
        if( !json_get( interface, "_id", network.interfaceId ) ) {
            Log::error( cc::error( "No network interface id found" ) );
            continue;
        }
        return network;
    }
    return {};
}

void NetworkManager::checkAndFixRoutes( bool notifyOnRoutesChange ) {
    // TODO: Research routing problem deeper. Try to fix routing before notifying
    //       about internet availability change, because it may fix internet
    auto device = m_routesManager->checkAndFixRoutes( getNetworkInterfacesVector( false ) );
    if( getActiveInterface() != device ) {
        // NOTE: maybe we should also notify when we fix routes even if default route was not changed at the end
        LOGGER_DEBUG_RAW( cc::debug( "Default route changed to " ) + cc::note( device.empty() ? "none" : device ) );
        setActiveInterface( device );
        if( notifyOnRoutesChange ) {
            m_networkChangeListener.onInternetAvailabilityChanged( false );
            if( !device.empty() )
                m_networkChangeListener.onInternetAvailabilityChanged( true );
        }

        notifyActiveInterfaceChanged( device );
    }
}

void NetworkManager::setActiveInterface( const std::string& activeInterface ) {
    auto query = JSON( db::ID_FIELD_NAME << Resources::HubActiveInterfaceId << "interfaceId" << activeInterface );
    m_dbClient.insert( TempNetworkDynamicInfoDBName, query );
}

std::string NetworkManager::getActiveInterface() {
    auto activeInterfaceJson = m_dbClient.findOne(
        TempNetworkDynamicInfoDBName, JSON( db::ID_FIELD_NAME << Resources::HubActiveInterfaceId ), nullptr );
    return json_get_str( activeInterfaceJson, "interfaceId" );
}

void NetworkManager::notifyActiveInterfaceChanged( const std::string& activeDevice ) {
    forEachNetworkInterface( [&activeDevice]( NetworkInterface& interface ) {
        if( interface.getName() == activeDevice ) {
            interface.onInterfaceActive( true );
        }
        else {
            interface.onInterfaceActive( false );
        }
    } );
}

void NetworkManager::forEachNetworkInterface( const std::function< void( NetworkInterface& ) >& callback ) {
    callback( m_ethernetInterface );
    callback( m_clientWifiInterface );
    callback( m_apWifiInterface );

    m_gsmManager.foreachGSMInterface( [&callback]( GSMInterface& gsmInterface ) { callback( gsmInterface ); } );
}

std::vector< Json > NetworkManager::getNetworkInterfacesVector( bool requireWifiApInterface ) const {
    auto interfaces = std::vector< Json >( { m_ethernetInterface.getInfo(), m_clientWifiInterface.getInfo() } );

    if( requireWifiApInterface ) {
        interfaces.push_back( m_apWifiInterface.getInfo() );
    }

    std::vector< Json > gsmDynamicInterfacesInfo;
    auto getGSMDynamicInterfacesInfo = [&gsmDynamicInterfacesInfo]( const GSMInterface& gsmInterface ) {
        gsmDynamicInterfacesInfo.push_back( gsmInterface.getInfo() );
    };

    getGSMManager().foreachGSMInterface( getGSMDynamicInterfacesInfo );
    interfaces.insert( interfaces.end(), gsmDynamicInterfacesInfo.begin(), gsmDynamicInterfacesInfo.end() );

    return interfaces;
}

void NetworkManager::onInterfaceInternetAvailabilityChanged( bool internetAvailable ) {
    // NOTE: It may take some time for UID to understand that it needs
    //       to reconnect because default route changed, so poke it here
    auto prevInternetAvailableJson = m_dbClient.findOne(
        TempNetworkDynamicInfoDBName, JSON( db::ID_FIELD_NAME << Resources::HubInternetAvailabillityId ), nullptr );
    bool prevInternetAvailable = json_get_bool( prevInternetAvailableJson, "internetAvailable" );

    checkAndFixRoutes( prevInternetAvailable || internetAvailable );

    if( prevInternetAvailable == internetAvailable ) {
        // This can only happen when this interface got internet
        // but we already have internet by other interface, nothing to do
        return;
    }

    bool overallInternetAvailable =
        m_ethernetInterface.isInternetAvailable() || m_clientWifiInterface.isInternetAvailable();

    onNonMobileInternetAvailabilityChanged( overallInternetAvailable );

    auto checkGSMDynamicInterfacesInternet = [&overallInternetAvailable]( const GSMInterface& gsmInterface ) {
        if( gsmInterface.isInternetAvailable() ) {
            overallInternetAvailable = true;
        }
    };

    getGSMManager().foreachGSMInterface( checkGSMDynamicInterfacesInternet );

    if( prevInternetAvailable == overallInternetAvailable ) {
        // This can only happen when this interface lost internet
        // but we already have internet by other interface, nothing to do
        return;
    }

    Log::info( cc::info( "Internet is now " ) + cc::note( overallInternetAvailable ? "available" : "not available" ) );
    m_dbClient.update( TempNetworkDynamicInfoDBName,
                       JSON( db::ID_FIELD_NAME << Resources::HubInternetAvailabillityId ),
                       JSON( "$set" << JSON( "internetAvailable" << overallInternetAvailable ) ) );

    m_networkChangeListener.onInternetAvailabilityChanged( overallInternetAvailable );
}

void NetworkManager::onNonMobileInternetAvailabilityChanged( bool internetAvailable ) {
    m_fixNetworkTimer.stop();
    if( !internetAvailable ) {
        m_fixNetworkTimer.start( std::chrono::seconds( 30 ), false );
    }
    m_apInterfaceCheckTimer.stop();
    m_apInterfaceCheckTimer.start( std::chrono::seconds( 60 ), false );
}

void NetworkManager::fixNetwork() {
    if( isNonMobileInternetAvailable() ) {
        return;
    }
    Log::info( cc::info( "Trying to fix network" ) );
    updateAdditionalPingServers();
    System::getCommandOutput( "/etc/init.d/firewall restart" );
}

void NetworkManager::forceBroadcast() {
    using namespace Resources::InterfaceInfo;
    m_gsmManager.foreachGSMInterface( [this]( GSMInterface& gsmInterface ) {
        auto info = gsmInterface.getInfo();
        if( !info[ Fields::SignalQuality ].empty() ) {
            auto notificationInterfaceInfo = JSON_EMPTY_OBJ;
            notificationInterfaceInfo[ Fields::Id ] = info[ Fields::Id ];
            notificationInterfaceInfo[ Fields::SignalQuality ] = info[ Fields::SignalQuality ];

            Json notification;
            notification[ "interfaces" ] = JSON_ARRAY( notificationInterfaceInfo );
            notification[ "syncNotification" ] = false;
            m_networkChangeListener.onNetworkInfoChanged( std::move( notification ) );
        }
        else {
            LOGGER_TRACE_RAW( cc::trace( "Modem signal quality not available for: " ) +
                              cc::note( gsmInterface.getDeviceName() ) );
        }
    } );
}

void NetworkManager::checkIfApInterfaceShouldBeUp() {
    if( m_apWifiInterface.getEnablePolicy() != EnablePolicy::Off || !isNonMobileInternetAvailable() ) {
        m_apWifiInterface.enable();
    }
    else {
        m_apWifiInterface.disable();
    }

    manageConnectionService();
}

void NetworkManager::updateAdditionalPingServers() {
    auto extractAddress = []( std::string url ) {
        auto address = std::move( url );
        if( !address.empty() ) {
            static const std::string protoMark = "://";
            auto protoMarkPos = address.find( protoMark );
            if( protoMarkPos != std::string::npos ) {
                address.erase( 0, protoMarkPos + protoMark.size() );
            }
        }
        if( !address.empty() ) {
            auto portMarkPos = address.find( ':' );
            if( portMarkPos != std::string::npos ) {
                address.erase( portMarkPos );
            }
        }
        if( !address.empty() ) {
            auto resourceMarkPos = address.find( '/' );
            if( resourceMarkPos != std::string::npos ) {
                address.erase( resourceMarkPos );
            }
        }
        return address;
    };

    auto& manager = ProvisionDataUtils::getManager();
    std::vector< std::string > servers;
    LOGGER_DEBUG( cc::debug( "Read provisioning data" ) );
    manager.readData();
    auto provisionServer = extractAddress( manager.getProvisionData().provisionServer );
    if( !provisionServer.empty() ) {
        servers.push_back( provisionServer );
    }
    auto cloudServer = extractAddress( manager.getProvisionData().cloudServer );
    if( !cloudServer.empty() ) {
        servers.push_back( cloudServer );
    }
    // TODO: move all ping resources to provision data.
    std::string nmaServer;
    auto joCacheCloud = m_dbClient.findOne( HaTempCacheDBName, JSON( "cache-cloud-info" << 1 ), nullptr );
    if( !joCacheCloud.empty() ) {
        nmaServer = extractAddress( json_get_str( joCacheCloud, "cache-nma-url" ) );
    }
    if( !nmaServer.empty() ) {
        servers.push_back( nmaServer );
    }
    setAdditionalServers( servers );
}

namespace {
Json generateAddRemoveJsonBroadcast( const std::string& deviceName, const std::string& status ) {
    using namespace Resources::InterfaceInfo;
    return JSON( db::ID_FIELD_NAME << deviceName << Fields::Status << status );
}
}  // namespace

void NetworkManager::onNewInterfaceInfo( Json info ) {
    using namespace Resources::InterfaceInfo;

    auto interfaces = m_dbClient.findAll( TempNetworkInterfaces, JSON_EMPTY_OBJ, nullptr );
    LOGGER_TRACE_RAW( cc::trace( "Old network interfaces: " ) + cc::j( interfaces ) );
    auto oldNetwork = getActiveNetwork( std::move( interfaces ) );
    LOGGER_TRACE_RAW( cc::debug( "Old IP address: " ) + cc::note( oldNetwork.ipv4 ) );

    if( Fields::Id != db::ID_FIELD_NAME ) {
        info[ db::ID_FIELD_NAME ] = info[ Fields::Id ];
    }

    const auto deviceName = json_get_str( info, db::ID_FIELD_NAME );
    const auto status = json_get_str( info, Fields::Status );
    const auto type = json_get_str( info, Fields::Type );

    /* handle special cases: interface added/removed */
    if( !status.empty() && ( status == "added" || status == "removed" ) ) {
        if( status == "added" ) {
            m_dbClient.insert( TempNetworkInterfaces, JSON( db::ID_FIELD_NAME << deviceName ) );
        }
        else {
            m_dbClient.remove( TempNetworkInterfaces, JSON( db::ID_FIELD_NAME << deviceName ) );
        }

        Json notification;
        notification[ "interfaces" ] = JSON_ARRAY( generateAddRemoveJsonBroadcast( deviceName, status ) );
        notification[ "syncNotification" ] = false;
        m_networkChangeListener.onNetworkInfoChanged( std::move( notification ) );
    }
    else {
        auto prevNotification =
            m_dbClient.findOne( TempNetworkInterfaces, JSON( db::ID_FIELD_NAME << deviceName ), nullptr );
        auto diff = jsonDiff( prevNotification, info );
        if( !diff.empty() ) {
            m_dbClient.update( TempNetworkInterfaces, JSON_EMPTY_OBJ, info, true );

            diff[ Fields::Id ] = json_get_str( info, Fields::Id );

            LOGGER_TRACE_RAW( cc::trace( "Interface " ) + cc::note( json_get_str( info, Fields::Id ) ) +
                              cc::trace( " changed: " ) + cc::j( diff ) );

            auto it = diff.find( Fields::Status );
            if( it != diff.end() && it.value().is_string() ) {
                auto status = it.value().get< std::string >();
                Log::info( cc::info( "Interface " ) + cc::note( json_get_str( info, Fields::Id ) ) +
                           cc::info( " went " ) + cc::note( status ) );
                m_networkChangeListener.onInterfaceStatusChanged( deviceName, type, status == Statuses::Up );
            }

            it = diff.find( Fields::InternetAvailable );
            if( it != diff.end() && it.value().is_boolean() ) {
                bool internetAvailable = it.value().get< bool >();
                Log::info( cc::info( "Interface " ) +
                           cc::note( json_get_str( info, Fields::Id ) + " " + ( internetAvailable ? "got" : "lost" ) ) +
                           cc::info( " internet" ) );

                onInterfaceInternetAvailabilityChanged( internetAvailable );
            }

            if( !prevNotification.empty() ) {
                Json notification;
                notification[ "interfaces" ] = JSON_ARRAY( std::move( diff ) );
                Json& interfaces = notification[ "interfaces" ];
                bool syncNotification = false;
                syncNotification |= json_contains( interfaces, Fields::Ipv4 );
                syncNotification |= json_contains( interfaces, Fields::Ipv6 );
                syncNotification |= json_contains( interfaces, Fields::Status );
                syncNotification |= json_contains( interfaces, Fields::Mask );
                notification[ "syncNotification" ] = syncNotification;

                m_networkChangeListener.onNetworkInfoChanged( std::move( notification ) );
            }
        }
    }
    interfaces = m_dbClient.findAll( TempNetworkInterfaces, JSON_EMPTY_OBJ, nullptr );
    LOGGER_TRACE_RAW( cc::trace( "New network interfaces: " ) + cc::j( interfaces ) );
    auto newNetwork = getActiveNetwork( std::move( interfaces ) );
    LOGGER_TRACE_RAW( cc::debug( "New IP address: " ) + cc::note( newNetwork.ipv4 ) );
    if( newNetwork.ipv4 != oldNetwork.ipv4 ) {
        LOGGER_DEBUG( cc::debug( "Update IP address: " ) + cc::note( newNetwork.ipv4 ) + cc::debug( " and type: " ) +
                      cc::note( newNetwork.type ) );
        m_networkChangeListener.onCurrentNetworkChanged( newNetwork );
    }
}

void NetworkManager::onNewInterfaceAdded( const std::string& interfaceName ) {
    onNewInterfaceInfo( generateAddRemoveJsonBroadcast( interfaceName, "added" ) );
}

void NetworkManager::onInterfaceRemoved( const std::string& interfaceName ) {
    onNewInterfaceInfo( generateAddRemoveJsonBroadcast( interfaceName, "removed" ) );
}

bool NetworkManager::scanWifi( std::string& jsonResult ) {
    // NOTE: this handler is asynchronous
    Json scanResult = m_wifiPassiveScanner.manualScan( Resources::ClientWifiDeviceName );
    if( scanResult.empty() ) {
        return false;
    }

    JsonArray& networks = scanResult[ "networks" ];
    if( networks.is_array() ) {
        for( auto& network : networks ) {
            network[ "open" ] = json_get_str( network, Resources::Scan::WifiSecurityKey ) == "open";
            json_remove( network, Resources::Scan::BssidKey );
            json_remove( network, Resources::Scan::ChannelKey );
            json_remove( network, Resources::Scan::WifiSecurityKey );
        }
        scanResult[ "status" ] = "success";
    }

    jsonResult = scanResult.dump();
    return true;
}

void NetworkManager::connect( std::string ssid, std::string bssid, std::string encryption, std::string key ) {
    // NOTE: this handler is asynchronous
    m_taskLoop.addTask( [=]() { connectToWiFi( ssid, bssid, encryption, key, true ); } );
}

void NetworkManager::temporaryShutDownAP() {
    // NOTE: this handler is asynchronous
    m_taskLoop.addTask( [=]() {
        m_apWifiInterface.disable();
        m_taskLoop
            .createPostponedTask( [=]( Services::PostponedTask& ) {
                if( m_apWifiInterface.getEnablePolicy() != EnablePolicy::Off || !isInternetAvailable() ) {
                    m_apWifiInterface.enable();
                }
                manageConnectionService();
            } )
            ->start( std::chrono::seconds( 20 ), false );
    } );
}

void NetworkManager::restart() {
    // NOTE: this handler is asynchronous
    m_taskLoop.addTask( [=]() { resetWiFi(); } );
}

}  // namespace Network
}  // namespace HubPlatform
