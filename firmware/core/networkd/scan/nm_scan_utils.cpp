#include "networkd/scan/nm_scan_utils.h"
#include "networkd/scan/nm_wifi_scan_result_parser.h"
#include "networkd/resources/nm_resources.h"

#include "system_utils/system.h"
#include "json_utils/json_utils.h"
#include "base_utils/utils.h"

#include <cmath>
#include <algorithm>

namespace HubPlatform {
namespace Network {

Json networkToJson( const NetworkItem& networkItem ) {
    Json item;
    item[ std::string( Resources::Scan::ChannelKey ) ] = networkItem.m_channel;
    item[ std::string( Resources::Scan::SsidKey ) ] = networkItem.m_ssid;
    item[ std::string( Resources::Scan::BssidKey ) ] = networkItem.m_bssid;
    item[ std::string( Resources::Scan::SignalLevelValueKey ) ] = networkItem.m_signalPercentLevel;
    item[ std::string( Resources::Scan::WifiSecurityKey ) ] = networkItem.m_security;
    return item;
}

NetworkItem jsonToNetwork( const Json& item ) {
    NetworkItem networkItem;

    networkItem.m_channel = json_get_int( item, Resources::Scan::ChannelKey );
    networkItem.m_ssid = json_get_str( item, Resources::Scan::SsidKey );
    networkItem.m_bssid = json_get_str( item, Resources::Scan::BssidKey );
    networkItem.m_signalPercentLevel = json_get_int( item, Resources::Scan::SignalLevelValueKey );
    networkItem.m_security = json_get_str( item, Resources::Scan::WifiSecurityKey );
    return networkItem;
}

int convertWifiSignalDbmToPercent( double dbm ) {
    if( dbm < -92. ) {
        return 1;
    }

    if( dbm > -21. ) {
        return 100;
    }

    return static_cast< int >( round( ( -0.0154 * dbm - 0.3794 ) * dbm + 98.182 ) );
}

NetworkMap scanWifiToMap( const std::string& interfaceName, const std::string& ssid ) {
    auto isSpace = []( unsigned char c ) -> bool { return std::isspace( c ); };

    if( std::all_of( interfaceName.begin(), interfaceName.end(), isSpace ) ) {
        return {};
    }

    std::string clarifyingSsid;

#if defined( __HUB_TYPE_G150__ ) || defined( __HUB_TYPE_G450__ ) || defined( __HUB_TYPE_G550__ )
    if( !ssid.empty() ) {
        clarifyingSsid = " " + ssid;
    }
    const std::string command = "iwpriv " + interfaceName + " get_site_survey" + clarifyingSsid;

    IwprivScanResultParser parser;
#else
    if( !ssid.empty() ) {
        clarifyingSsid = " ssid " + escapeSymbolsByFirst( ssid, "\\'\" #" );
    }
    const std::string command = "iw dev " + interfaceName + " scan" + clarifyingSsid;

    IwScanResultParser parser;
#endif

    std::string output;
    int result;
    std::tie( output, result ) = System::getCommandOutput( command.c_str(), true );
    if( result != 0 ) {
        if( std::count( output.begin(), output.end(), '\n' ) > 1 ) {
            // NOTE: sometimes scan fails with (almost) complete normal output,
            //       which is huge and does not contain any errors
            output = "";
        }
        throw Exception( output );
    }
    parser.parse( output );
    return parser.getScannedItems();
}

NetworkItem getScannedNetwork( const Json& wifiScanResult, const std::string& ssid ) {
    if( !wifiScanResult.is_object() ) {
        return {};
    }

    const auto& list = wifiScanResult[ std::string( "networks" ) ];

    if( !list.is_array() || ssid.empty() ) {
        return {};
    }

    int level = std::numeric_limits< int >::min();
    NetworkItem result;

    for( const Json& item : list ) {
        if( json_get_str( item, "ssid" ) != ssid ) {
            continue;
        }

        auto bssid = json_get_str( item, "bssid" );
        if( bssid.empty() ) {
            continue;
        }

        const auto currentLevel = json_get_int( item, "signalLevel" );
        if( currentLevel <= level ) {
            continue;
        }

        result = jsonToNetwork( item );
        level = currentLevel;
    }

    return result;
}

std::string getBssid( const Json& wifiScanResult, const std::string& ssid ) {
    if( !wifiScanResult.is_object() ) {
        return {};
    }

    const auto& list = wifiScanResult[ std::string( "networks" ) ];

    if( !list.is_array() || ssid.empty() ) {
        return {};
    }

    int level = std::numeric_limits< int >::min();
    std::string result;

    for( const Json& item : list ) {
        if( json_get_str( item, "ssid" ) != ssid ) {
            continue;
        }

        auto bssid = json_get_str( item, "bssid" );
        if( bssid.empty() ) {
            continue;
        }

        const auto currentLevel = json_get_int( item, "signalLevel" );
        if( currentLevel <= level ) {
            continue;
        }

        result = std::move( bssid );
        level = currentLevel;
    }

    return result;
}

}  // namespace Network
}  // namespace HubPlatform
