#pragma once

#include "networkd/scan/nm_scan_result.h"

#include "json_utils/json_fwd.h"

namespace HubPlatform {
namespace Network {

class Exception : public std::runtime_error {
    using std::runtime_error::runtime_error;
};

Json networkToJson( const NetworkItem& networkItem );

int convertWifiSignalDbmToPercent( double dbm );
NetworkMap scanWifiToMap( const std::string& interfaceName, const std::string& ssid = "" );

std::string getBssid( const Json& wifiScanResult, const std::string& ssid );
NetworkItem getScannedNetwork( const Json& wifiScanResult, const std::string& ssid );

}  // namespace Network
}  // namespace HubPlatform
