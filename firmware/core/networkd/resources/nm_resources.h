#pragma once

#include <array>

namespace HubPlatform {
namespace Network {
namespace Resources {

extern const std::array< std::pair< const char*, const char* >, 4 > BackupTableFieldPairs;

constexpr const char HubInternetAvailabillityId[] = "hub";
constexpr const char HubActiveInterfaceId[] = "activeInterface";

#ifdef __HUB_TYPE_G150__
extern const char CombinedInterfaceName[];
#endif

extern const char ApInterfaceName[];
extern const char ClientInterfaceName[];
extern const char MobileBaseInterfaceName[];
extern const char MobileBaseDeviceName[];
extern const char ApIp[];
extern const char ApNetMask[];
extern const int ApStartLeasingAddress;
extern const int ApLimitLeasingAddress;

extern const char EthernetInterfaceName[];
extern const char EthernetDeviceName[];
extern const char ClientWifiDeviceName[];
extern const char ApWifiDeviceName[];

extern const char DefaultApWifiChannel[];
extern const char DefaultWifiRegion[];
extern const char DefaultApEncryption[];

extern const char EthernetType[];
extern const char WifiType[];
extern const char ApType[];
extern const char ClientType[];
extern const char MobileType[];

extern const char WanNetwork[];
extern const char LanNetwork[];

extern int EthernetPriority;
extern int WifiClientPriority;
extern int WifiApPriority;
extern int MobileBasePriority;

namespace Settings {
extern const char ModemSignalQualityPeriod[];
}

namespace Scan {
extern const char ChannelKey[];
extern const char BssidKey[];
extern const char SsidKey[];
extern const char SignalLevelValueKey[];
extern const char WifiSecurityKey[];
extern const int TryAttempts;
}  // namespace Scan

namespace InterfaceInfo {

namespace Fields {
extern const char Id[];
extern const char Status[];
extern const char Ipv4[];
extern const char Ipv6[];
extern const char Mask[];
extern const char Ip[];
extern const char Mode[];
extern const char Gateway[];
extern const char Dns[];
extern const char Addresses[];
extern const char PrefixLen[];
extern const char HwAddress[];
extern const char InternetAvailable[];
extern const char Type[];
extern const char Priority[];
extern const char Network[];
extern const char EnablePolicy[];
extern const char IMEI[];
extern const char SIMInfo[];
extern const char SignalQuality[];
extern const char AccessTechnology[];
extern const char FrequencyBand[];
extern const char DebugInformation[];
}  // namespace Fields

namespace Statuses {
extern const char Unknown[];
extern const char Down[];
extern const char Up[];
}  // namespace Statuses

}  // namespace InterfaceInfo

namespace GSM {
extern const char GSMModemBaudRate[];

extern const char EmbeddedCellularModemPath[];
extern const int EmbeddedCellularModemNumber;

#if( defined( __HUB_TYPE_H2__ ) || defined( __HUB_TYPE_H2_SECURE__ ) )
extern const int ExternalCellularModemsUSBPortPowerKeyPin;
#endif

extern const int ExternalCellularModemsUSBBusNumber;

}  // namespace GSM

extern const int PingPeriod;

}  // namespace Resources
}  // namespace Network
}  // namespace HubPlatform
