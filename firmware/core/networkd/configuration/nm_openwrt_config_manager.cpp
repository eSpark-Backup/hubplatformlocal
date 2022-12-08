#include "networkd/configuration/nm_openwrt_config_manager.h"
#include "networkd/configuration/nm_chat_script.h"
#include "networkd/resources/nm_resources.h"

#include "uci_utils/uci_dhcp_file.h"
#include "json_utils/json_utils.h"
#include "base_utils/utils.h"
#include "database_client/db_tables_names.h"
#include "database_client/dbqueryparser.h"
#include "log_utils/ezlo_log.h"
#include "tiny/tiny_consolecolors.h"

#include <fstream>
#include <utility>
#include <thread>
#include <chrono>

namespace HubPlatform {
namespace Network {

namespace {

const char FirewallConfigFile[] = "/etc/config/firewall";

bool setIfOptionExists( JsonObject& json,
                        const std::string& field,
                        const UciUtils::UciSection::SingleOptions& options,
                        const std::string& option ) {
    auto it = options.find( option );
    if( it == options.end() ) {
        json[ field ] = "";
        return false;
    }

    json[ field ] = it->second;
    return true;
}

bool fixNetworkConfig( UciUtils::UciNetworkFile& networkFile ) {
    const UciUtils::UciSection clientWifiSection{ "interface",
                                                  Resources::ClientInterfaceName,
                                                  { { "proto", "dhcp" }
#if defined( __HUB_TYPE_G150__ )
                                                    ,
                                                    { "ifname", Resources::ClientWifiDeviceName }
#endif
                                                  } };

    bool changed = false;
    if( networkFile.ensureSection( clientWifiSection ) ) {
        changed = true;
    }

    const UciUtils::UciSection apWifiSection{
        "interface",
        Resources::ApInterfaceName,
        { { "proto", "static" }, { "netmask", Resources::ApNetMask }, { "ipaddr", Resources::ApIp } }
    };
    if( networkFile.ensureSection( apWifiSection ) ) {
        changed = true;
    }

    int i = 0;
    while( true ) {
        if( !networkFile.findSection( "interface", Resources::MobileBaseInterfaceName + std::to_string( i ) )
                 .isValid() ) {
            break;
        }

        networkFile.removeSection( "interface", Resources::MobileBaseInterfaceName + std::to_string( i++ ) );
        changed = true;
    }

    if( !changed ) {
        return true;
    }

    if( !networkFile.save() ) {
        return false;
    }

    Log::info( cc::info( "Fix network config" ) );
    return networkFile.apply();
}

bool fixDhcpConfig() {
    UciUtils::UciDhcpFile dhcpFile;

    if( !dhcpFile.load() ) {
        return false;
    }

    const UciUtils::UciSection apWifiSection{ "dhcp",
                                              Resources::ApInterfaceName,
                                              { { "interface", Resources::ApInterfaceName },
                                                { "start", std::to_string( Resources::ApStartLeasingAddress ) },
                                                { "limit", std::to_string( Resources::ApLimitLeasingAddress ) },
                                                { "leasetime", "12h" } },
                                              {} };

    if( !dhcpFile.ensureSection( apWifiSection ) ) {
        return true;
    }

    if( !dhcpFile.save() ) {
        return false;
    }

    Log::info( cc::info( "Fix dhcp config" ) );
    return dhcpFile.apply();
}

bool fixFirewallConfig() {
    UciUtils::UciSettings firewallSettings;

    if( !UciUtils::readSettings( FirewallConfigFile, firewallSettings ) ) {
        Log::warn( cc::warn( std::string( FirewallConfigFile ) + " is broken!" ) );
        return false;
    }

    const auto hasZone = [&firewallSettings]( const char* InterfaceName ) {
        return std::any_of(
            firewallSettings.begin(), firewallSettings.end(), [InterfaceName]( const UciUtils::UciSection& section ) {
                if( section.m_type != "zone" ) {
                    return false;
                }

                const auto nameIt = section.m_singleOptions.find( "name" );
                if( nameIt != section.m_singleOptions.end() && nameIt->second == InterfaceName ) {
                    return true;
                }

                const auto networksRange = section.m_listedOptions.equal_range( "network" );
                return std::any_of( networksRange.first,
                                    networksRange.second,
                                    [InterfaceName]( const std::pair< std::string, std::string >& keyToValue ) {
                                        return keyToValue.second == InterfaceName;
                                    } );
            } );
    };

    const auto hasApWifiZone = hasZone( Resources::ApInterfaceName );
    const auto hasClientWifiZone = hasZone( Resources::ClientInterfaceName );

    if( hasApWifiZone && hasClientWifiZone ) {
        return false;
    }

    Log::info( cc::info( "Fix firewall config" ) );

    std::ofstream stream( FirewallConfigFile, std::ios::out | std::ios::app );

    stream << "\n";

    if( !hasApWifiZone ) {
        const UciUtils::UciSection section{ "zone",
                                            "",
                                            { { "name", Resources::ApInterfaceName },
                                              { "input", "ACCEPT" },
                                              { "output", "ACCEPT" },
                                              { "forward", "REJECT" } },
                                            { { "network", Resources::ApInterfaceName } } };
        stream << "\n#Auto generated zone settings for firmware access point\n" << section;
    }

    if( !hasClientWifiZone ) {
        const UciUtils::UciSection section{ "zone",
                                            "",
                                            { { "name", Resources::ClientInterfaceName },
                                              { "input", "ACCEPT" },
                                              { "output", "ACCEPT" },
                                              { "forward", "REJECT" },
                                              { "masq", "1" },
                                              { "mtu_fix", "1" } },
                                            { { "network", Resources::ClientInterfaceName } } };
        stream << "\n#Auto generated zone settings for firmware client\n" << section;
    }

    return true;
}

void enableWifiScanning() {
#if defined( __HUB_TYPE_G150__ ) || defined( __HUB_TYPE_G450__ ) || defined( __HUB_TYPE_G550__ )
    system( "iwpriv ra0 set SiteSurvey=0" );
#endif
}

void reloadFirewallConfiguration() {
    Log::info( cc::info( "Reload firewall configuration" ) );
    system( "/etc/init.d/firewall reload" );
}

/* mt7620(G150) wireless driver has different names for encryption types */
const std::map< std::string, std::string > OpenWRTxMt7620EncryptionMap = {
    { "none", "WPANONE" },      { "wep", "OPEN" },      { "psk", "WPAPSK" },     { "psk2", "WPA2PSK" },
    { "wep+shared", "SHARED" }, { "wep+open", "OPEN" }, { "unknown", "unknown" }
};

const std::string& getOpenWRTEncryption( const std::string& mt7620Encryption ) {
    for( const auto& pair : OpenWRTxMt7620EncryptionMap ) { /* map is too small to do something else... */
        if( pair.second == mt7620Encryption ) {
            return pair.first;
        }
    }

    return OpenWRTxMt7620EncryptionMap.at( "unknown" );
}

const std::string& getMt7620Encryption( const std::string& openWRTEncryption ) {
    if( OpenWRTxMt7620EncryptionMap.find( openWRTEncryption ) == OpenWRTxMt7620EncryptionMap.end() ) {
        return OpenWRTxMt7620EncryptionMap.at( "unknown" );
    }

    return OpenWRTxMt7620EncryptionMap.at( openWRTEncryption );
}

void waitForWiFiDriver() {
    auto wifiReady = []() { return system( "ifconfig -a | grep -q wlan*" ) == 0; };

    if( wifiReady() )
        return;

    Log::info( cc::info( "Waiting for WiFi driver ..." ) );
    if( waitForCondition( wifiReady, std::chrono::seconds( 15 ), std::chrono::seconds( 2 ) ) )
        Log::info( cc::info( "WiFi driver loaded" ) );
    else
        Log::warn( cc::warn( "WiFi driver not loaded" ) );
}

void initializeNetwork( IDbClient& dbClient ) {
    auto networkdStatus =
        dbClient.findOne( TempNetworkInfoDBName, JSON( db::ID_FIELD_NAME << "networkdStatus" ), nullptr );
    if( networkdStatus.empty() ) {
        /* TODO: for some reasons netifd doesn't accept further 'reload' commands without being restarted... */
        system( "/etc/init.d/network restart" );

        /* give netifd time to restart */
        std::this_thread::sleep_for( std::chrono::seconds( 10 ) );

        auto query = JSON( db::ID_FIELD_NAME << "networkdStatus"
                                             << "status"
                                             << "initialized" );
        dbClient.update( TempNetworkInfoDBName, JSON_EMPTY_OBJ, query, true );
    }
}

}  // namespace

class WirelessConfigurationTransactionImpl : public NetworkConfigManager::WirelessConfigurationTransaction {
public:
    explicit WirelessConfigurationTransactionImpl( UciUtils::UciWirelessFile& wirelessFile )
    : m_wirelessFile( wirelessFile )
    , m_modified( false )
    , m_hard( false ) {}

    void setModified( bool hard ) {
        m_modified = true;
        m_hard |= hard;
        fflush( stdout );
    }

    ~WirelessConfigurationTransactionImpl() override {
        if( m_modified ) {
            m_wirelessFile.apply( m_hard );
        }
    }

private:
    UciUtils::UciWirelessFile& m_wirelessFile;
    bool m_modified;
    bool m_hard;
};

OpenwrtNetworkConfigManager::OpenwrtNetworkConfigManager( const ProvisionDataUtils::ProvisionData& provisionData,
                                                          IDbClient& dbClient )
: m_provisionData( provisionData )
, m_dbClient( dbClient ) {
    int attempts = 0;
    while( !m_wirelessFile.load() && ++attempts <= 30 ) {
        Log::info( cc::info( "Waiting for wireless config" ) );
        std::this_thread::sleep_for( std::chrono::seconds( 2 ) );
    }

    m_networkFile.load();
}

bool OpenwrtNetworkConfigManager::fixWirelessConfig() {
    bool changed = false;

    auto& device = m_wirelessFile.findSection( "wifi-device", "" );
    if( !device.isValid() ) {
        return false;
    }

    if( device.m_singleOptions.count( "disabled" ) && device.m_singleOptions[ "disabled" ] == "1" ) {
        device.m_singleOptions[ "disabled" ] = "0";
        changed = true;
    }

    if( device.m_singleOptions.count( "htmode" ) && device.m_singleOptions[ "htmode" ] == "VHT80" ) {
        device.m_singleOptions[ "htmode" ] = "HT20";
        changed = true;
    }

    if( 0 == device.m_singleOptions.count( "channel" ) ) {
        device.m_singleOptions[ "channel" ] = Resources::DefaultApWifiChannel;
#ifdef __HUB_TYPE_G150__
        device.m_singleOptions[ "autoch" ] = ( device.m_singleOptions[ "channel" ] == "0" ) ? "2" : "0";
#endif
        changed = true;
    }

    size_t clientNum = 0;
    size_t apNum = 0;
    auto checkInterfaces = [&clientNum, &apNum]( UciUtils::UciSection& section ) {
        if( section.m_type == "wifi-iface" ) {
            if( section.m_name == Resources::ClientInterfaceName )
                ++clientNum;
            else if( section.m_name == Resources::ApInterfaceName )
                ++apNum;
            else
                return false;
        }
        return true;
    };

    if( false == m_wirelessFile.process( checkInterfaces ) || clientNum > 1 || apNum > 1 ) {
        Log::warn( cc::warn( "Wireless config contains interfaces other than client and AP" ) );
        while( m_wirelessFile.removeSection( "wifi-iface", "" ) ) {
        };
        changed = true;
    }

    if( !changed ) {
        return true;
    }

    if( !m_wirelessFile.save() ) {
        return false;
    }

    Log::info( cc::info( "Fix wireless config" ) );
    startWirelessConfigurationTransaction()->setModified();

    return true;
}

void OpenwrtNetworkConfigManager::prepareEnvironment() {
    // TODO: 1. Add required settings to openwrt image
    //       2. Investigate, should we fix config if they are incomplete?
    //          May be only have to check correctness of config.
    fixNetworkConfig( m_networkFile );
    fixDhcpConfig();
    fixWirelessConfig();

    if( fixFirewallConfig() ) {
        reloadFirewallConfiguration();
    }

#if defined( __HUB_TYPE_H2__ ) || defined( __HUB_TYPE_H2_SECURE__ )
    waitForWiFiDriver();
#endif

    initializeNetwork( m_dbClient );

    enableWifiScanning();
}

Json OpenwrtNetworkConfigManager::getSystemWirelessConfig( const std::string& interfaceName ) const {
    JsonObject wifi;
    const auto& deviceSection = m_wirelessFile.findSection( "wifi-device", "" );
    if( deviceSection.isValid() ) {
        // NOTE: channel is common for all wireless interfaces
        const auto& options = deviceSection.m_singleOptions;
        setIfOptionExists( wifi, "channel", options, "channel" );
    }

    std::string wifiInterfaceName = interfaceName;
#if defined( __HUB_TYPE_G150__ )
    wifiInterfaceName = "apcli_wifi";
#endif

    const auto& section = m_wirelessFile.findSection( "wifi-iface", wifiInterfaceName );
    if( section.isValid() ) {
        const auto& options = section.m_singleOptions;

        JsonObject network;

        setIfOptionExists( network, "ssid", options, "ssid" );
        setIfOptionExists( network, "mode", options, "mode" );

        if( network[ "mode" ] == "sta" )
            setIfOptionExists( network, "bssid", options, "bssid" );

        setIfOptionExists( network, "encryption", options, "encryption" );
        setIfOptionExists( network, "key", options, "key" );

#ifdef __HUB_TYPE_G150__
        if( interfaceName == std::string( Resources::ApInterfaceName ) ) {
            network[ "mode" ] = "ap";
        }
        else if( interfaceName == std::string( Resources::ClientInterfaceName ) ) {
            setIfOptionExists( network, "bssid", options, "bssid" );
            setIfOptionExists( network, "ssid", options, "ApCliSsid" );
            setIfOptionExists( network, "key", options, "ApCliWPAPSK" );
            network[ "mode" ] = "sta";

            setIfOptionExists( network, "encryption", options, "ApCliAuthMode" );
            network[ "encryption" ] = getOpenWRTEncryption( json_get_str( network, "encryption" ) );
        }
#endif

        wifi[ "network" ] = std::move( network );
        wifi[ "region" ] = Resources::DefaultWifiRegion;  // TODO: Read from config
    }

    return wifi;
}

bool OpenwrtNetworkConfigManager::setSystemWirelessConfig( const char* interfaceName, const Json& config ) {
    auto ssid = json_get_str( config[ "network" ], "ssid" );
    Log::info( cc::info( "Configuring " ) + cc::note( interfaceName ) + cc::info( " interface for " ) +
               cc::note( ssid ) );
    LOGGER_TRACE_RAW( cc::j( config ) );

    auto device = m_wirelessFile.findSection( "wifi-device", "" );
    if( !device.isValid() ) {
        return false;
    }

    std::string wifiInterfaceName;
    UciUtils::UciSection::SingleOptions wifiIfaceOptions;
#ifdef __HUB_TYPE_G150__
    wifiInterfaceName = Resources::CombinedInterfaceName;

    const auto& existingSection = m_wirelessFile.findSection( "wifi-iface", wifiInterfaceName );
    if( existingSection.isValid() ) {
        wifiIfaceOptions = existingSection.m_singleOptions;
    }

    wifiIfaceOptions[ "device" ] = device.m_name;
    wifiIfaceOptions[ "ifname" ] = Resources::ApWifiDeviceName;
    wifiIfaceOptions[ "network" ] = Resources::ApInterfaceName;

    if( interfaceName == std::string( Resources::ApInterfaceName ) ) {
        if( wifiIfaceOptions.count( "ApCliSsid" ) == 0 ) {
            device.m_singleOptions[ "channel" ] = json_get_str( config, "channel" );
        }
        wifiIfaceOptions[ "ssid" ] = m_provisionData.defaultWifiSsid;
        // wifiIfaceOptions[ "ssid" ] =
        //    "Atom" + m_provisionData.defaultWifiSsid.substr( m_provisionData.defaultWifiSsid.find( '_' ) );
        wifiIfaceOptions[ "key" ] = m_provisionData.defaultWifiPassword;
        wifiIfaceOptions[ "encryption" ] = Resources::DefaultApEncryption;
        wifiIfaceOptions[ "mode" ] = json_get_str( config, "mode" );
    }
    else {
        device.m_singleOptions[ "channel" ] = json_get_str( config, "channel" );
        wifiIfaceOptions[ "ApCliEnable" ] = "1";
        wifiIfaceOptions[ "ApCliSsid" ] = ssid;
        wifiIfaceOptions[ "ApCliAuthMode" ] = getMt7620Encryption( json_get_str( config[ "network" ], "encryption" ) );
        wifiIfaceOptions[ "ApCliEncrypType" ] = "AES";
        wifiIfaceOptions[ "ApCliWPAPSK" ] = json_get_str( config[ "network" ], "key" );
        if( wifiIfaceOptions.count( "ssid" ) == 0 )
            wifiIfaceOptions[ "ssid" ] = "";
        if( wifiIfaceOptions.count( "key" ) == 0 )
            wifiIfaceOptions[ "key" ] = "";
        if( wifiIfaceOptions.count( "encryption" ) == 0 )
            wifiIfaceOptions[ "encryption" ] = "";
        if( wifiIfaceOptions.count( "mode" ) == 0 )
            wifiIfaceOptions[ "mode" ] = "ap";
    }
    bool hasClientConfiguration = wifiIfaceOptions.count( "ApCliSsid" ) != 0;

    device.m_singleOptions[ "autoch" ] = ( device.m_singleOptions[ "channel" ] == "0" ) ? "2" : "0";
#else
    if( std::string( interfaceName ) == std::string( Resources::ClientInterfaceName ) ||
        !m_wirelessFile.findSection( "wifi-iface", Resources::ClientInterfaceName ).isValid() ) {
        device.m_singleOptions[ "channel" ] = json_get_str( config, "channel" );
    }

    int channel = atoi( device.m_singleOptions[ "channel" ].c_str() );
    device.m_singleOptions[ "hwmode" ] = ( channel > 0 && channel < 36 ) ? "11g" : "11a";

    wifiInterfaceName = interfaceName;
    wifiIfaceOptions = { { "device", device.m_name },
                         { "ssid", ssid },
                         { "key", json_get_str( config[ "network" ], "key" ) },
                         { "encryption", json_get_str( config[ "network" ], "encryption" ) },
                         { "network", interfaceName },
                         { "mode", json_get_str( config, "mode" ) } };

    auto bssid = json_get_str( config[ "network" ], "bssid" );
    if( !bssid.empty() ) {
        wifiIfaceOptions.insert( { "bssid", bssid } );
    }
#endif

    bool modified = false;
    if( m_wirelessFile.ensureSection( std::move( device ) ) )
        modified = true;

    UciUtils::UciSection section{ "wifi-iface", wifiInterfaceName, std::move( wifiIfaceOptions ) };

    if( m_wirelessFile.ensureSection( std::move( section ) ) )
        modified = true;

    if( !modified )
        return true;

    if( !m_wirelessFile.save() ) {
        return false;
    }

#if defined( __HUB_TYPE_G150__ )
    startWirelessConfigurationTransaction()->setModified( hasClientConfiguration );
#else
    startWirelessConfigurationTransaction()->setModified();
#endif

    return true;
}

bool OpenwrtNetworkConfigManager::resetSystemWirelessConfig( const char* interfaceName, bool hard ) {
    Log::info( cc::info( "Resetting " ) + cc::note( interfaceName ) + cc::info( " interface" ) );

#if defined( __HUB_TYPE_G150__ )
    std::string wifiInterfaceName = Resources::CombinedInterfaceName;

    auto& section = m_wirelessFile.findSection( "wifi-iface", wifiInterfaceName );
    if( !section.isValid() ) {
        return true;
    }

    auto& wifiIfaceOptions = section.m_singleOptions;

    if( interfaceName == std::string( Resources::ApInterfaceName ) ) {
        wifiIfaceOptions[ "ssid" ] = "";
        wifiIfaceOptions[ "key" ] = "";
        wifiIfaceOptions[ "encryption" ] = "";
        wifiIfaceOptions[ "mode" ] = "ap";
    }
    else {
        wifiIfaceOptions.erase( "ApCliEnable" );
        wifiIfaceOptions.erase( "ApCliSsid" );
        wifiIfaceOptions.erase( "ApCliAuthMode" );
        wifiIfaceOptions.erase( "ApCliEncrypType" );
        wifiIfaceOptions.erase( "ApCliWPAPSK" );
    }

    bool hasClientConfiguration = wifiIfaceOptions.count( "ApCliSsid" ) != 0;
    if( wifiIfaceOptions[ "ssid" ].empty() && !hasClientConfiguration ) {
        if( !m_wirelessFile.removeSection( "wifi-iface", Resources::CombinedInterfaceName ) ) {
            return true;
        }
    }
    else {
        m_wirelessFile.ensureSection( std::move( section ) );
    }
#else
    if( !m_wirelessFile.removeSection( "wifi-iface", interfaceName ) ) {
        return true;
    }
#endif

    if( !m_wirelessFile.save() ) {
        return false;
    }

#if defined( __HUB_TYPE_G150__ )
    startWirelessConfigurationTransaction()->setModified( hasClientConfiguration );
#else
    startWirelessConfigurationTransaction()->setModified();
#endif

    return true;
}

Json OpenwrtNetworkConfigManager::getSystemNetworkConfig( const std::string& interfaceName ) const {
    JsonObject interface;
    const auto& section = m_networkFile.findSection( "interface", interfaceName );
    if( section.isValid() ) {
        const auto& options = section.m_singleOptions;

        JsonObject ipv4;
        if( setIfOptionExists( ipv4, "mode", options, "proto" ) ) {
            if( ipv4[ "mode" ] == "static" ) {
                setIfOptionExists( ipv4, "ip", options, "ipaddr" );
                setIfOptionExists( ipv4, "mask", options, "netmask" );
            }
            else if( ipv4[ "mode" ] == "3g" ) {
                setIfOptionExists( interface, "apn", options, "apn" );
                setIfOptionExists( interface, "device", options, "device" );
                setIfOptionExists( interface, "dialnumber", options, "dialnumber" );
                setIfOptionExists( interface, "pppname", options, "pppname" );
            }
        }

        interface[ "ipv4" ] = std::move( ipv4 );

        // TODO: add IPv6 part
    }
    return interface;
}

bool OpenwrtNetworkConfigManager::isSystemNetworkConfig( const char* interfaceName ) {
    return m_networkFile.findSection( "interface", interfaceName ).isValid();
}

bool OpenwrtNetworkConfigManager::setSystemNetworkConfig( const char* interfaceName, const Json& config, bool reload ) {
    UciUtils::UciSection::SingleOptions singleOptions;

    try {
        for( auto it = config.begin(); it != config.end(); ++it ) {
            singleOptions.emplace( std::make_pair( it.key(), ( *it ).get< std::string >() ) );
        }
    }
    catch( const std::exception& ex ) {
        Log::error( cc::error( std::string( "OpenwrtNetworkConfigManager::setSystemNetworkConfig: invalid json - " ) +
                               ex.what() ) );
    }

    UciUtils::UciSection section{ "interface", interfaceName, std::move( singleOptions ) };

    m_networkFile.ensureSection( std::move( section ) );
    if( !m_networkFile.save() ) {
        return false;
    }

    if( reload ) {
        return m_networkFile.apply();
    }

    return true;
}

bool OpenwrtNetworkConfigManager::removeSystemNetworkConfig( const char* interfaceName, bool reload ) {
    if( !m_networkFile.removeSection( "interface", interfaceName ) ) {
        Log::warn( cc::warn( "Attempt to remove an inexistent uci section" ) );
        return true;
    }

    if( !m_networkFile.save() ) {
        return false;
    }

    if( reload ) {
        return m_networkFile.apply();
    }

    return true;
}

std::shared_ptr< NetworkConfigManager::WirelessConfigurationTransaction >
OpenwrtNetworkConfigManager::startWirelessConfigurationTransaction() {
    if( auto transaction = m_wirelessConfigurationTransaction.lock() ) {
        return transaction;
    }
    auto transaction = std::make_shared< WirelessConfigurationTransactionImpl >( m_wirelessFile );
    m_wirelessConfigurationTransaction = transaction;
    return transaction;
}

EthernetConfiguration OpenwrtNetworkConfigManager::getEthernetConfiguration( const std::string& interfaceName ) const {
    EthernetConfiguration configuration;

    configuration.m_mode = getModeConfiguration( interfaceName );

    return configuration;
}

APWifiConfiguration OpenwrtNetworkConfigManager::getAPWifiConfiguration( const std::string& interfaceName ) const {
    APWifiConfiguration configuration;

    configuration.m_mode = getModeConfiguration( interfaceName );

    const auto wirelessConfig = getSystemWirelessConfig( interfaceName );

    auto channel = json_get_str( wirelessConfig, "channel" );
    configuration.m_channel = channel.empty() ? 0 : std::stoi( channel );

    if( json_contains( wirelessConfig, "network" ) ) {
        configuration.m_ssid = json_get_str( wirelessConfig[ "network" ], "ssid" );
        configuration.m_encryption = json_get_str( wirelessConfig[ "network" ], "encryption" );
        configuration.m_key = json_get_str( wirelessConfig[ "network" ], "key" );
    }

    return configuration;
}

ClientWifiConfiguration OpenwrtNetworkConfigManager::getClientWifiConfiguration(
    const std::string& interfaceName ) const {
    ClientWifiConfiguration configuration;

    configuration.m_mode = getModeConfiguration( interfaceName );

    const auto wirelessConfig = getSystemWirelessConfig( interfaceName );

    auto channel = json_get_str( wirelessConfig, "channel" );
    configuration.m_channel = channel.empty() ? 0 : std::stoi( channel );

    if( json_contains( wirelessConfig, "network" ) ) {
        configuration.m_ssid = json_get_str( wirelessConfig[ "network" ], "ssid" );
        configuration.m_bssid = json_get_str( wirelessConfig[ "network" ], "bssid" );
        configuration.m_encryption = json_get_str( wirelessConfig[ "network" ], "encryption" );
        configuration.m_key = json_get_str( wirelessConfig[ "network" ], "key" );
    }

    return configuration;
}

MobileConfiguration OpenwrtNetworkConfigManager::getMobileConfiguration( const std::string& interfaceName ) const {
    MobileConfiguration configuration;

    const auto config = getSystemNetworkConfig( interfaceName );
    if( json_contains( config, "ipv4" ) ) {
        configuration.m_mode = json_get_str( config[ "ipv4" ], "mode" );
        configuration.m_apn = json_get_str( config, "apn" );
        configuration.m_device = json_get_str( config, "device" );
        configuration.m_dialNumber = json_get_str( config, "dialnumber" );
        configuration.m_pppName = json_get_str( config, "pppname" );
    }

    return configuration;
}

// TODO: not implemented
bool OpenwrtNetworkConfigManager::configureEthernetInterface( const std::string& interfaceName,
                                                              const EthernetConfiguration& configuration ) {
    return true;
}

bool OpenwrtNetworkConfigManager::configureAPWifiInterface( const std::string& interfaceName,
                                                            const APWifiConfiguration& configuration ) {
    m_apWifiConfiguration = configuration;

    if( configuration.isEmtpy() ) {
        return resetSystemWirelessConfig( interfaceName.c_str() );
    }
    Json config = JSON( "channel" << std::to_string( configuration.m_channel ) << "region"
                                  << Resources::DefaultWifiRegion << "mode" << Resources::ApType << "network"
                                  << JSON( "ssid" << configuration.m_ssid << "encryption" << configuration.m_encryption
                                                  << "key" << configuration.m_key ) );

    return setSystemWirelessConfig( interfaceName.c_str(), config );
}

bool OpenwrtNetworkConfigManager::configureClientWifiInterface( const std::string& interfaceName,
                                                                const ClientWifiConfiguration& configuration ) {
    bool result;

    if( configuration.isEmtpy() ) {
        result = resetSystemWirelessConfig( interfaceName.c_str() );

#if defined( __HUB_TYPE_H2__ ) || defined( __HUB_TYPE_H2_SECURE__ ) || defined( __HUB_TYPE_G150__ )
        /* for systems which share one wifi radio between several interfaces we need to restore an ap interface
         * after a client interface was stopped */
        // TODO: take in account systems with multiple wifi APs
        if( isWirelessConfigured( Resources::ApInterfaceName ) ) {
            result &= configureAPWifiInterface( Resources::ApInterfaceName, m_apWifiConfiguration );
        }
#endif
        return result;
    }

    Json config =
        JSON( "channel" << std::to_string( configuration.m_channel ) << "region" << Resources::DefaultWifiRegion
                        << "mode" << Resources::ClientType << "network"
                        << JSON( "ssid" << configuration.m_ssid << "encryption" << configuration.m_encryption << "key"
                                        << configuration.m_key << "bssid" << configuration.m_bssid ) );

    result = setSystemWirelessConfig( interfaceName.c_str(), config );

    return result;
}

bool OpenwrtNetworkConfigManager::configureMobileInterface( const std::string& interfaceName,
                                                            const MobileConfiguration& configuration ) {
    if( configuration.isEmtpy() ) {
        const auto ipv6InterfaceName = "ipv6" + interfaceName;

        if( isSystemNetworkConfig( ipv6InterfaceName.c_str() ) ) {
            removeSystemNetworkConfig( ipv6InterfaceName.c_str() );
        }

        if( isSystemNetworkConfig( interfaceName.c_str() ) ) {
            removeSystemNetworkConfig( interfaceName.c_str() );
        }

        return setSystemNetworkConfig( interfaceName.c_str(),
                                       JSON( "proto"
                                             << "3g" ),
                                       true );
    }

    const std::chrono::seconds delay( 10 );  // to avoid rare annoying messages during a ppp link establishment
    const std::chrono::seconds timeToIpv4Connect( 10 );
    const std::chrono::seconds timeToIpv6Connect( 20 );

    auto config = JSON( "proto"
                        << "3g"
                        << "pppname" << configuration.m_pppName << "device" << configuration.m_device << "apn"
                        << configuration.m_apn << "dialnumber" << configuration.m_dialNumber << "delay"
                        << std::to_string( delay.count() ) );
    ChatScript3G chatScript;
    Json chatScriptConfig;
    bool result = true;

    if( configuration.m_ipv6 ) {
        json_add_str( chatScriptConfig, ChatOption::PDPType, "IPV6" );
        json_add_str( config, "ipv6", "1" );

        const auto ipv6Config = JSON( "ifname"
                                      << "@" + interfaceName << "proto"
                                      << "dhcpv6" );

        LOGGER_DEBUG_RAW( cc::debug( "IPv6 config for mobile interface " ) + cc::note( interfaceName ) +
                          cc::debug( ": " ) + cc::note( ipv6Config.dump( 1 ) ) );

        result = setSystemNetworkConfig( ( "ipv6" + interfaceName ).c_str(), ipv6Config, false );
    }

    chatScript.saveScript( chatScriptConfig );

    /* delegate responsibility of a ppp link/linux_interface creation to
     * a netifd/pppd (using 3g/ppp protocol handlers) bundle */
    result &= setSystemNetworkConfig( interfaceName.c_str(), config, true );

    /* let netifd/pppd up a new interface we're going to monitor calmly */
    const auto timeToConnect = ( ( configuration.m_ipv6 ) ? timeToIpv6Connect : timeToIpv4Connect ) + delay;
    std::this_thread::sleep_for( timeToConnect );

    return result;
}

bool OpenwrtNetworkConfigManager::isWirelessConfigured( const std::string& interfaceName ) const {
#if defined( __HUB_TYPE_G150__ )
    auto wirelessSection = m_wirelessFile.findSection( "wifi-iface", Resources::CombinedInterfaceName );
    if( wirelessSection.isValid() ) {
        if( interfaceName == Resources::ClientInterfaceName ) {
            return wirelessSection.m_singleOptions[ "ApCliEnable" ] == "1";
        }
        else if( interfaceName == Resources::ApInterfaceName ) {
            return !wirelessSection.m_singleOptions[ "ssid" ].empty();
        }
    }

    return false;
#else
    return m_wirelessFile.findSection( "wifi-iface", interfaceName ).isValid();
#endif
}

ModeConfiguration OpenwrtNetworkConfigManager::getModeConfiguration( const std::string& interfaceName ) const {
    ModeConfiguration configuration;

    const auto config = getSystemNetworkConfig( interfaceName );
    if( json_contains( config, "ipv4" ) ) {
        configuration.m_mode = json_get_str( config[ "ipv4" ], "mode" );
        configuration.m_ip = json_get_str( config[ "ipv4" ], "ip" );
        configuration.m_netMask = json_get_str( config[ "ipv4" ], "mask" );
    }

    return configuration;
}

}  // namespace Network
}  // namespace HubPlatform
