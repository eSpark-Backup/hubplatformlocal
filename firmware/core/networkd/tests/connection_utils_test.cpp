#include "networkd/monitoring/nm_monitor_utils.h"
#include "networkd/scan/nm_scan_utils.h"
#include "networkd/scan/nm_wifi_scan_result_parser.h"
#include "networkd/connection_service/nm_connection_defs.h"
#include "json_utils/json_utils.h"

#include <gtest/gtest.h>

/*

TESTPLAN

DONE    1. Convert wifi signal dbm to percent
DONE    2. Parse OpenWrt virtual interface status
DONE    3. Parse G150 virtual interface status
DONE    4. Parse iwpriv wifi scan result
DONE    5. Parse iw wifi scan result
DONE    6. Parse IP CIDR

*/

namespace HubPlatform {
namespace Network {
namespace ConnectionUtils {

namespace UT {

namespace {
constexpr const auto SuccessfullOpenwrtVirtualInterfaceStatusWithIpv4AndIpv6 = std::make_pair(
    R"({
"up": true,
"pending": false,
"available": true,
"autostart": true,
"dynamic": false,
"uptime": 272,
"l3_device": "eth2",
"proto": "dhcp",
"device": "eth1",
"updated": [
        "addresses",
        "routes",
        "data"
],
"metric": 0,
"dns_metric": 0,
"delegation": true,
"ipv4-address": [
        {
                "address": "192.168.11.17",
                "mask": 23
        }
],
"ipv6-address": [
        {
                "address": "2001:db80::a00:27ff:fe67:cd9c",
                "mask": 64,
                "preferred": 1681,
                "valid": 7081
        }
],
"ipv6-prefix": [
        {
                "address": "2001:db80:0:100::",
                "mask": 56,
                "preferred": 86282,
                "valid": 86282,
                "class": "wan6",
                "assigned": {
                        "lan": {
                                "address": "2001:db80:0:110::",
                                "mask": 60
                        }
                }
        }
],
"ipv6-prefix-assignment": [
        {
                "address": "fd9c:331f:8d7::",
                "mask": 60,
                "local-address": {
                        "address": "fd9c:331f:8d7::1",
                        "mask": 60
                }
        }
],
"route": [
        {
                "target": "0.0.0.0",
                "mask": 0,
                "nexthop": "192.168.10.1",
                "source": "192.168.11.17\/32"
        }
],
"dns-server": [
        "192.168.10.10",
        "1.1.1.1",
        "2001:4860:4860::8888",
        "8.8.8.8"
],
"dns-search": [ "ezlo.lan" ]
}
)",
    R"({
"_id":"eth1",
"ipv4":{
   "dns":[
      "192.168.10.10",
      "1.1.1.1",
      "8.8.8.8"
   ],
   "gateway":"192.168.10.1",
   "ip":"192.168.11.17",
   "mask":"255.255.254.0",
   "mode":"dhcp"
},
"status":"up",
"ipv6":{
   "addresses":[
      {
         "ip":"2001:db80::a00:27ff:fe67:cd9c",
         "prefixlen":64
      }
   ],
   "dns":[ "2001:4860:4860::8888" ],
   "mode":"dhcp"
}
})" );

constexpr const auto SuccessfullOpenWrtInterfaceStatus3GProto = std::make_pair(
    R"({
        "up": true,
        "pending": false,
        "available": true,
        "autostart": true,
        "dynamic": false,
        "uptime": 193,
        "l3_device": "mobile0",
        "proto": "3g",
        "updated": [
                "addresses",
                "routes"
        ],
        "metric": 0,
        "dns_metric": 0,
        "delegation": true,
        "ipv4-address": [
                {
                        "address": "10.217.174.113",
                        "mask": 32,
                        "ptpaddress": "10.64.64.64"
                }
        ],
        "ipv6-address": [

        ],
        "ipv6-prefix": [

        ],
        "ipv6-prefix-assignment": [

        ],
        "route": [
                {
                        "target": "0.0.0.0",
                        "mask": 0,
                        "nexthop": "10.64.64.64",
                        "source": "0.0.0.0\/0"
                }
        ],
        "dns-server": [
                "10.74.32.5",
                "10.74.32.6"
        ],
        "dns-search": [

        ],
        "neighbors": [

        ],
        "inactive": {
                "ipv4-address": [

                ],
                "ipv6-address": [

                ],
                "route": [

                ],
                "dns-server": [

                ],
                "dns-search": [

                ],
                "neighbors": [

                ]
        },
        "data": {

        }
}
)",
    R"({
 "_id": "mobile0",
 "ipv4": {
  "dns": [
   "10.74.32.5",
   "10.74.32.6"
  ],
  "gateway": "10.64.64.64",
  "ip": "10.217.174.113",
  "mask": "255.255.255.255",
  "mode": "3g"
 },
 "status": "up"
})" );

constexpr const auto NotReadyOpenWrtInterfaceStatus3GProto = std::make_pair(
    R"({
        "up": false,
        "pending": false,
        "available": true,
        "autostart": false,
        "dynamic": false,
        "proto": "3g",
        "data": {

        }
})",
    R"({
 "status": "down"
})" );

constexpr const auto SuccessfullG150VirtualInterfaceStatusWithIpv4 = std::make_pair(
    R"({
"up": true,
"pending": false,
"available": true,
"autostart": true,
"uptime": 61552,
"l3_device": "eth0.2",
"proto": "dhcp",
"device": "eth0.2",
"metric": 0,
"delegation": true,
"ipv4-address": [
        {
                "address": "192.168.11.131",
                "mask": 23
        }
],
"ipv6-address": [

],
"ipv6-prefix": [

],
"ipv6-prefix-assignment": [

],
"route": [
        {
                "target": "0.0.0.0",
                "mask": 0,
                "nexthop": "192.168.10.1",
                "source": "0.0.0.0\/0"
        }
],
"dns-server": [
        "192.168.10.10",
        "1.1.1.1",
        "8.8.8.8"
],
"dns-search": [
        "ezlo.lan"
],
"inactive": {
        "ipv4-address": [

        ],
        "ipv6-address": [

        ],
        "route": [

        ],
        "dns-server": [

        ],
        "dns-search": [

        ]
},
"data": {

}
}
)",
    R"({
"_id":"eth0.2",
"ipv4":{
   "dns":[
      "192.168.10.10",
      "1.1.1.1",
      "8.8.8.8"
   ],
   "gateway":"192.168.10.1",
   "ip":"192.168.11.131",
   "mask":"255.255.254.0",
   "mode":"dhcp"
},
"status":"up"
})" );

constexpr const auto SuccessfullOpenwrtVirtualInterfaceStatusWithIpv4 = std::make_pair(
    R"({
"up": true,
"pending": false,
"available": true,
"autostart": true,
"dynamic": false,
"uptime": 272,
"l3_device": "eth2",
"proto": "dhcp",
"device": "eth1",
"updated": [
        "addresses",
        "routes",
        "data"
],
"metric": 0,
"dns_metric": 0,
"delegation": true,
"ipv4-address": [
        {
                "address": "192.168.11.17",
                "mask": 23
        }
],
"ipv6-address": [],
"ipv6-prefix": [],
"ipv6-prefix-assignment": [],
"route": [
        {
                "target": "0.0.0.0",
                "mask": 0,
                "nexthop": "192.168.10.1",
                "source": "192.168.11.17\/32"
        }
],
"dns-server": [ "192.168.10.10" ],
"dns-search": [ "ezlo.lan" ]
}
)",
    R"({
"_id":"eth1",
"ipv4":{
   "dns":[ "192.168.10.10" ],
   "gateway":"192.168.10.1",
   "ip":"192.168.11.17",
   "mask":"255.255.254.0",
   "mode":"dhcp"
},
"status":"up"
})" );

constexpr const auto NotReadyOpenWrtInterfaceStatus = std::make_pair(
    R"({
"up": false,
"pending": false,
"available": true,
"autostart": true,
"dynamic": false,
"proto": "dhcp",
"device": "eth0",
"data": {}
}
)",
    R"({
"_id":"eth0",
"status":"down"
})" );

constexpr auto IwprivSuccessfulScan = std::make_pair(
    R"(
ra0       get_site_survey:
Ch  SSID                             BSSID               Security               Siganl(%)W-Mode  ExtCH  NT WPS DPID
1   eZLO_House                       78:8a:20:d4:44:8f   WPA2PSK/AES            44       11b/g/n NONE   In  NO
1   Atom_70003587                    3c:71:bf:7e:e3:09   NONE                   91       11b/g/n ABOVE  In  NO
1   eZLO_House2                      98:8b:c1:d4:18:1e   WEP                    61       11b/g/n NONE   In  NO
1   mios_55100234                    78:94:b4:f7:81:40   WPA2PSK/TKIPAES        100      11b/g/n ABOVE  In  NO
3   eZLO_House3                      78:8a:b1:d4:12:8e   WPAPSK/AES             60       11b/g/n NONE   In  NO
5   0xC2AF5C5F28E38384295F2FC2AF     50:d4:f7:04:e2:ae   WPA1PSKWPA2PSK/TKIPAES 50       11b/g/n NONE   In YES
11                                   50:d4:f7:04:e2:af   WPAPSK/AES             70       11b/g/n NONE   In  NO
)",
    R"({
"networks":[
  {
     "channel": 1,
     "ssid":"eZLO_House",
     "bssid":"78:8a:20:d4:44:8f",
     "security":"psk2",
     "signalLevel":44
  },
  {
	 "channel": 1,
     "ssid":"Atom_70003587",
     "bssid":"3c:71:bf:7e:e3:09",
     "security":"none",
     "signalLevel":91
  },
  {
     "channel": 1,
     "ssid":"eZLO_House2",
     "bssid":"98:8b:c1:d4:18:1e",
     "security":"wep",
     "signalLevel":61
  },
  {
	 "channel": 1,
     "ssid":"mios_55100234",
     "bssid":"78:94:b4:f7:81:40",
     "security":"psk2",
     "signalLevel":100
  },
  {
     "channel": 3,
     "ssid":"eZLO_House3",
     "bssid":"78:8a:b1:d4:12:8e",
     "security":"psk",
     "signalLevel":60
  },
  {
     "channel": 5,
     "bssid":"50:d4:f7:04:e2:ae",
     "security":"psk2",
     "signalLevel":50,
     "ssid":"¯\\_(ツ)_/¯"
  }
]})" );

constexpr auto IwprivEmptyScan = std::make_pair(
    R"(
 ra0       get_site_survey:
 Ch  SSID                             BSSID               Security               Siganl(%)W-Mode  ExtCH  NT WPS DPID
)",
    R"(
{ "networks":[] })" );

constexpr auto IwSuccessfulScan = std::make_pair( R"(BSS ac:ec:de:32:a0:01(on wlan0)
	TSF: 1825120051584 usec (21d, 02:58:40)
	freq: 2447
	beacon interval: 100 TUs
	capability: ESS (0x0431)
	signal: -90.00 dBm
	last seen: 11500 ms ago
	Information elements from Probe Response frame:
	SSID: eZLO_Smart_House
	HT capabilities:
		Capabilities: 0x11ee
			HT20/HT40
			SM Power Save disabled
			RX HT20 SGI
			RX HT40 SGI
			TX STBC
			RX STBC 1-stream
			Max AMSDU length: 3839 bytes
			DSSS/CCK HT40
		Maximum RX AMPDU length 65535 bytes (exponent: 0x003)
		Minimum RX AMPDU time spacing: 8 usec (0x06)
		HT TX/RX MCS rate indexes supported: 0-23
	HT operation:
		 * primary channel: 8
		 * secondary channel offset: below
		 * STA channel width: any
	RSN:	 * Version: 1
		 * Group cipher: CCMP
		 * Pairwise ciphers: CCMP
		 * Authentication suites: PSK
		 * Capabilities: 1-PTKSA-RC 1-GTKSA-RC (0x0000)
 BSS 70:4f:57:6a:65:90(on wlan0)
	TSF: 148463567691 usec (1d, 17:14:23)
	freq: 2422
	beacon interval: 100 TUs
	capability: ESS (0x0411)
	signal: -80.00 dBm
	last seen: 910 ms ago
	Information elements from Probe Response frame:
	SSID: Max_Wifi
	HT capabilities:
		Capabilities: 0x106e
			HT20/HT40
			SM Power Save disabled
			RX HT20 SGI
			RX HT40 SGI
			No RX STBC
			Max AMSDU length: 3839 bytes
			DSSS/CCK HT40
		Maximum RX AMPDU length 65535 bytes (exponent: 0x003)
		Minimum RX AMPDU time spacing: 4 usec (0x05)
		HT RX MCS rate indexes supported: 0-15, 32
		HT TX MCS rate indexes are undefined
	HT operation:
		 * primary channel: 3
		 * secondary channel offset: above
		 * STA channel width: any
	RSN:	 * Version: 1
		 * Group cipher: CCMP
		 * Pairwise ciphers: CCMP
		 * Authentication suites: PSK
		 * Capabilities: 1-PTKSA-RC 1-GTKSA-RC (0x0000)
 BSS fc:ec:da:38:1b:36(on wlan0)
	TSF: 1825140045320 usec (21d, 02:59:00)
	freq: 2437
	beacon interval: 100 TUs
	capability: ESS (0x0431)
	signal: -76.00 dBm
	last seen: 580 ms ago
	Information elements from Probe Response frame:
	SSID: eZLO_Smart_House
	HT capabilities:
		Capabilities: 0x1ac
			HT20
			SM Power Save disabled
			RX HT20 SGI
			TX STBC
			RX STBC 1-stream
			Max AMSDU length: 3839 bytes
			No DSSS/CCK HT40
		Maximum RX AMPDU length 65535 bytes (exponent: 0x003)
		Minimum RX AMPDU time spacing: 8 usec (0x06)
		HT TX/RX MCS rate indexes supported: 0-23
	HT operation:
		 * primary channel: 6
		 * secondary channel offset: no secondary
		 * STA channel width: 20 MHz
	RSN:	 * Version: 1
		 * Group cipher: CCMP
		 * Pairwise ciphers: CCMP
		 * Authentication suites: PSK
		 * Capabilities: 1-PTKSA-RC 1-GTKSA-RC (0x0000)
 BSS 3c:98:72:f9:c7:b0(on wlan0)
	TSF: 255730483537 usec (2d, 23:02:10)
	freq: 2437
	beacon interval: 100 TUs
	capability: ESS (0x0c11)
	signal: -87.00 dBm
	last seen: 11680 ms ago
	Information elements from Probe Response frame:
	SSID: mt7620-C7B0
	HT capabilities:
		Capabilities: 0x106e
			HT20/HT40
			SM Power Save disabled
			RX HT20 SGI
			RX HT40 SGI
			No RX STBC
			Max AMSDU length: 3839 bytes
			DSSS/CCK HT40
		Maximum RX AMPDU length 65535 bytes (exponent: 0x003)
		Minimum RX AMPDU time spacing: 4 usec (0x05)
		HT RX MCS rate indexes supported: 0-15, 32
		HT TX MCS rate indexes are undefined
	HT operation:
		 * primary channel: 6
		 * secondary channel offset: below
		 * STA channel width: any
	RSN:	 * Version: 1
		 * Group cipher: TKIP
		 * Pairwise ciphers: TKIP CCMP
		 * Authentication suites: PSK
		 * Capabilities: 1-PTKSA-RC 1-GTKSA-RC (0x0000)
 BSS fc:ec:da:32:a0:04(on wlan0)
	TSF: 1825120051584 usec (21d, 02:58:40)
	freq: 2447
	beacon interval: 100 TUs
	capability: ESS (0x0431)
	signal: -67.00 dBm
	last seen: 11500 ms ago
	Information elements from Probe Response frame:
	SSID: eZLO_Smart_House
	HT capabilities:
		Capabilities: 0x11ee
			HT20/HT40
			SM Power Save disabled
			RX HT20 SGI
			RX HT40 SGI
			TX STBC
			RX STBC 1-stream
			Max AMSDU length: 3839 bytes
			DSSS/CCK HT40
		Maximum RX AMPDU length 65535 bytes (exponent: 0x003)
		Minimum RX AMPDU time spacing: 8 usec (0x06)
		HT TX/RX MCS rate indexes supported: 0-23
	HT operation:
		 * primary channel: 8
		 * secondary channel offset: below
		 * STA channel width: any
	RSN:	 * Version: 1
		 * Group cipher: CCMP
		 * Pairwise ciphers: CCMP
		 * Authentication suites: PSK
		 * Capabilities: 1-PTKSA-RC 1-GTKSA-RC (0x0000)
 BSS 3c:98:72:f9:c6:70(on wlan0)
	last seen: 233308.187s [boottime]
	TSF: 0 usec (0d, 00:00:00)
	freq: 2462
	beacon interval: 100 TUs
	capability: ESS (0x0421)
	signal: -20.00 dBm
	last seen: 0 ms ago
	SSID: test_none
	HT capabilities:
		Capabilities: 0x1ec
			HT20
			SM Power Save disabled
			RX HT20 SGI
			RX HT40 SGI
			TX STBC
			RX STBC 1-stream
			Max AMSDU length: 3839 bytes
			No DSSS/CCK HT40
		Maximum RX AMPDU length 65535 bytes (exponent: 0x003)
		Minimum RX AMPDU time spacing: 4 usec (0x05)
		HT RX MCS rate indexes supported: 0-15
		HT TX MCS rate indexes are undefined
	HT operation:
		 * primary channel: 11
		 * secondary channel offset: no secondary
		 * STA channel width: 20 MHz
 BSS 3c:98:72:f9:c6:70(on wlan0)
	last seen: 233621.173s [boottime]
	TSF: 0 usec (0d, 00:00:00)
	freq: 2462
	beacon interval: 100 TUs
	capability: ESS (0x0421)
	signal: -22.00 dBm
	last seen: 0 ms ago
	SSID: test_wep_open
	HT capabilities:
		Capabilities: 0x1ec
			HT20
			SM Power Save disabled
			RX HT20 SGI
			RX HT40 SGI
			TX STBC
			RX STBC 1-stream
			Max AMSDU length: 3839 bytes
			No DSSS/CCK HT40
		Maximum RX AMPDU length 65535 bytes (exponent: 0x003)
		Minimum RX AMPDU time spacing: 4 usec (0x05)
		HT RX MCS rate indexes supported: 0-15
		HT TX MCS rate indexes are undefined
	HT operation:
		 * primary channel: 11
		 * secondary channel offset: no secondary
		 * STA channel width: 20 MHz
 BSS 3c:98:72:f9:c6:70(on wlan0)
	last seen: 236276.318s [boottime]
	TSF: 0 usec (0d, 00:00:00)
	freq: 2462
	beacon interval: 100 TUs
	capability: ESS (0x0431)
	signal: -22.00 dBm
	last seen: 0 ms ago
	SSID: test_wep_shared
BSS 48:5b:39:12:96:ab(on wlan0)
	last seen: 550.252s [boottime]
	TSF: 0 usec (0d, 00:00:00)
	freq: 2437
	beacon interval: 100 TUs
	capability: ESS (0x0411)
	signal: -72.00 dBm
	last seen: 0 ms ago
	SSID: Wireless_Killa_Asus
	RSN:     * Version: 1
		 * Group cipher: TKIP
		 * Pairwise ciphers: CCMP TKIP
		 * Authentication suites: PSK
		 * Capabilities: 1-PTKSA-RC 1-GTKSA-RC (0x0000)
BSS 00:e0:20:20:8c:c7(on wlan0)
	last seen: 550.252s [boottime]
	TSF: 0 usec (0d, 00:00:00)
	freq: 2437
	beacon interval: 100 TUs
	capability: ESS (0x0411)
	signal: -72.00 dBm
	last seen: 0 ms ago
	SSID: Wireless_Killa_Asus
	HT capabilities:
		Capabilities: 0x2c
			HT20
			SM Power Save disabled
			RX HT20 SGI
			No RX STBC
			Max AMSDU length: 3839 bytes
			No DSSS/CCK HT40
		Maximum RX AMPDU length 65535 bytes (exponent: 0x003)
		Minimum RX AMPDU time spacing: 4 usec (0x05)
		HT RX MCS rate indexes supported: 0-15
		HT TX MCS rate indexes are undefined
	HT operation:
		 * primary channel: 6
		 * secondary channel offset: no secondary
		 * STA channel width: 20 MHz
	RSN:     * Version: 1
		 * Group cipher: CCMP
		 * Pairwise ciphers: CCMP
		 * Authentication suites: PSK
		 * Capabilities: 1-PTKSA-RC 1-GTKSA-RC (0x0000)
 BSS 3c:98:72:f9:c6:70(on wlan0)
	last seen: 3096.466s [boottime]
	TSF: 0 usec (0d, 00:00:00)
	freq: 2462
	beacon interval: 100 TUs
	capability: ESS (0x0431)
	signal: -21.00 dBm
	last seen: 0 ms ago
	SSID: test_psk
	HT capabilities:
		Capabilities: 0x1ec
			HT20
			SM Power Save disabled
			RX HT20 SGI
			RX HT40 SGI
			TX STBC
			RX STBC 1-stream
			Max AMSDU length: 3839 bytes
			No DSSS/CCK HT40
		Maximum RX AMPDU length 65535 bytes (exponent: 0x003)
		Minimum RX AMPDU time spacing: 4 usec (0x05)
		HT RX MCS rate indexes supported: 0-15
		HT TX MCS rate indexes are undefined
	HT operation:
		 * primary channel: 11
		 * secondary channel offset: no secondary
		 * STA channel width: 20 MHz
 BSS 3c:98:72:f9:c6:70(on wlan0)
	last seen: 233511.514s [boottime]
	TSF: 0 usec (0d, 00:00:00)
	freq: 2462
	beacon interval: 100 TUs
	capability: ESS (0x0431)
	signal: -24.00 dBm
	last seen: 0 ms ago
	SSID: test_psk2
	HT capabilities:
		Capabilities: 0x1ec
			HT20
			SM Power Save disabled
			RX HT20 SGI
			RX HT40 SGI
			TX STBC
			RX STBC 1-stream
			Max AMSDU length: 3839 bytes
			No DSSS/CCK HT40
		Maximum RX AMPDU length 65535 bytes (exponent: 0x003)
		Minimum RX AMPDU time spacing: 4 usec (0x05)
		HT RX MCS rate indexes supported: 0-15
		HT TX MCS rate indexes are undefined
	HT operation:
		 * primary channel: 11
		 * secondary channel offset: no secondary
		 * STA channel width: 20 MHz
	RSN:	 * Version: 1
		 * Group cipher: CCMP
		 * Pairwise ciphers: CCMP
		 * Authentication suites: PSK
		 * Capabilities: 1-PTKSA-RC 1-GTKSA-RC (0x0000)
 BSS 3c:98:72:f9:c6:70(on wlan0)
	last seen: 236680.479s [boottime]
	TSF: 0 usec (0d, 00:00:00)
	freq: 2462
	beacon interval: 100 TUs
	capability: ESS (0x0431)
	signal: -23.00 dBm
	last seen: 10 ms ago
	SSID: test_psk_psk2
	HT capabilities:
		Capabilities: 0x1ec
			HT20
			SM Power Save disabled
			RX HT20 SGI
			RX HT40 SGI
			TX STBC
			RX STBC 1-stream
			Max AMSDU length: 3839 bytes
			No DSSS/CCK HT40
		Maximum RX AMPDU length 65535 bytes (exponent: 0x003)
		Minimum RX AMPDU time spacing: 4 usec (0x05)
		HT RX MCS rate indexes supported: 0-15
		HT TX MCS rate indexes are undefined
	HT operation:
		 * primary channel: 11
		 * secondary channel offset: no secondary
		 * STA channel width: 20 MHz
	RSN:	 * Version: 1
		 * Group cipher: CCMP
		 * Pairwise ciphers: CCMP
		 * Authentication suites: PSK
		 * Capabilities: 1-PTKSA-RC 1-GTKSA-RC (0x0000)
 BSS 18:cc:23:00:1a:9b(on wlan0)
	last seen: 246656.947s [boottime]
	TSF: 0 usec (0d, 00:00:00)
	freq: 2447
	beacon interval: 200 TUs
	capability: ESS (0x0431)
	signal: -62.00 dBm
	last seen: 0 ms ago
	SSID: different_order
	RSN:	 * Version: 1
		 * Group cipher: CCMP
		 * Pairwise ciphers: CCMP
		 * Authentication suites: PSK
		 * Capabilities: 1-PTKSA-RC 1-GTKSA-RC (0x0000)
	HT capabilities:
		Capabilities: 0x116e
			HT20/HT40
			SM Power Save disabled
			RX HT20 SGI
			RX HT40 SGI
			RX STBC 1-stream
			Max AMSDU length: 3839 bytes
			DSSS/CCK HT40
		Maximum RX AMPDU length 65535 bytes (exponent: 0x003)
		Minimum RX AMPDU time spacing: 4 usec (0x05)
		HT RX MCS rate indexes supported: 0-7, 32
		HT TX MCS rate indexes are undefined
	HT operation:
		 * primary channel: 8
		 * secondary channel offset: below
		 * STA channel width: any
 BSS 98:de:d0:af:c5:90(on wlan0)
	last seen: 247647.419s [boottime]
	TSF: 0 usec (0d, 00:00:00)
	freq: 2442
	beacon interval: 200 TUs
	capability: ESS (0x0431)
	signal: -24.00 dBm
	last seen: 10 ms ago
	SSID: Igaar_eZLO
	RSN:	 * Version: 1
		 * Group cipher: CCMP
		 * Pairwise ciphers: CCMP
		 * Authentication suites: PSK
		 * Capabilities: 1-PTKSA-RC 1-GTKSA-RC (0x0000)
	HT capabilities:
		Capabilities: 0x11ef
			RX LDPC
			HT20/HT40
			SM Power Save disabled
			RX HT20 SGI
			RX HT40 SGI
			TX STBC
			RX STBC 1-stream
			Max AMSDU length: 3839 bytes
			DSSS/CCK HT40
		Maximum RX AMPDU length 65535 bytes (exponent: 0x003)
		Minimum RX AMPDU time spacing: 8 usec (0x06)
		HT TX/RX MCS rate indexes supported: 0-15
	HT operation:
		 * primary channel: 7
		 * secondary channel offset: above
		 * STA channel width: any
 BSS a0:f3:c1:85:ea:2d(on wlan0)
	last seen: 7266.646s [boottime]
	TSF: 0 usec (0d, 00:00:00)
	freq: 2412
	beacon interval: 100 TUs
	capability: ESS (0x0431)
	signal: -75.00 dBm
	last seen: 0 ms ago
	SSID: Name with spaces
	RSN:     * Version: 1
		 * Group cipher: CCMP
		 * Pairwise ciphers: CCMP
		 * Authentication suites: PSK
		 * Capabilities: 1-PTKSA-RC 1-GTKSA-RC (0x0000)
	HT capabilities:
		Capabilities: 0x11ef
			RX LDPC
			HT20/HT40
			SM Power Save disabled
			RX HT20 SGI
			RX HT40 SGI
			TX STBC
			RX STBC 1-stream
			Max AMSDU length: 3839 bytes
			DSSS/CCK HT40
		Maximum RX AMPDU length 65535 bytes (exponent: 0x003)
		Minimum RX AMPDU time spacing: No restriction (0x00)
		HT RX MCS rate indexes supported: 0-15
		HT TX MCS rate indexes are undefined
	HT operation:
		 * primary channel: 1
		 * secondary channel offset: above
		 * STA channel width: any
BSS 50:d4:f7:04:e2:ae(on wlan0)
        last seen: 11819.064s [boottime]
        TSF: 0 usec (0d, 00:00:00)
        freq: 2432
        beacon interval: 100 TUs
        capability: ESS (0x0431)
        signal: -79.00 dBm
        last seen: 0 ms ago
        SSID: \xc2\xaf\x5c_(\xe3\x83\x84)_/\xc2\xaf
        HT capabilities:
                Capabilities: 0x11ee
                        HT20/HT40
                        SM Power Save disabled
                        RX HT20 SGI
                        RX HT40 SGI
                        TX STBC
                        RX STBC 1-stream
                        Max AMSDU length: 3839 bytes
                        DSSS/CCK HT40
                Maximum RX AMPDU length 65535 bytes (exponent: 0x003)
                Minimum RX AMPDU time spacing: 8 usec (0x06)
                HT TX/RX MCS rate indexes supported: 0-23
        HT operation:
                 * primary channel: 5
                 * secondary channel offset: no secondary
                 * STA channel width: any
        RSN:     * Version: 1
                 * Group cipher: TKIP
                 * Pairwise ciphers: CCMP TKIP
                 * Authentication suites: PSK
                 * Capabilities: 1-PTKSA-RC 1-GTKSA-RC (0x0000)
)",
                                                  R"({
"networks":[
   {
      "channel": 3,
      "ssid":"Max_Wifi",
      "bssid":"70:4f:57:6a:65:90",
      "security":"psk2",
      "signalLevel":30
   },
   {
      "channel": 6,
      "ssid":"mt7620-C7B0",
      "bssid":"3c:98:72:f9:c7:b0",
      "security":"psk2",
      "signalLevel":15
   },
   {
      "channel": 8,
      "ssid":"eZLO_Smart_House",
      "bssid":"fc:ec:da:32:a0:04",
      "security":"psk2",
      "signalLevel":54
   },
   {
      "channel": 11,
      "bssid":"3c:98:72:f9:c6:70",
      "security":"none",
      "signalLevel":100,
      "ssid":"test_none"
   },
   {
      "channel": 11,
      "bssid":"3c:98:72:f9:c6:70",
      "security":"psk",
      "signalLevel":99,
      "ssid":"test_psk"
   },
   {
      "channel": 11,
      "bssid":"3c:98:72:f9:c6:70",
      "security":"psk2",
      "signalLevel":98,
      "ssid":"test_psk2"
   },
   {
      "channel": 11,
      "bssid":"3c:98:72:f9:c6:70",
      "security":"psk2",
      "signalLevel":99,
      "ssid":"test_psk_psk2"
   },
   {
      "channel": 11,
      "bssid":"3c:98:72:f9:c6:70",
      "security":"none",
      "signalLevel":99,
      "ssid":"test_wep_open"
   },
   {
      "channel": 6,
      "bssid":"00:e0:20:20:8c:c7",
      "security":"psk2",
      "signalLevel":46,
      "ssid":"Wireless_Killa_Asus"
   },
   {
      "channel": 8,
      "bssid":"18:cc:23:00:1a:9b",
      "security":"psk2",
      "signalLevel":63,
      "ssid":"different_order"
   },
   {
      "channel": 7,
      "bssid":"98:de:d0:af:c5:90",
      "security":"psk2",
      "signalLevel":98,
      "ssid":"Igaar_eZLO"
   },
   {
      "channel": 1,
      "bssid":"a0:f3:c1:85:ea:2d",
      "security":"psk2",
      "signalLevel":40,
      "ssid":"Name with spaces"
   },
   {
      "channel": 5,
      "bssid":"50:d4:f7:04:e2:ae",
      "security":"psk2",
      "signalLevel":32,
      "ssid":"¯\\_(ツ)_/¯"
   }
]})" );

Json getSortedWifiScanResult( Json json ) {
    const auto networksIt = json.find( "networks" );
    if( networksIt != json.end() ) {
        std::sort( networksIt.value().begin(), networksIt.value().end(), []( const Json& a, const Json& b ) {
            return json_get_str( a, "ssid" ) < json_get_str( b, "ssid" );
        } );
    }

    return json;
}

}  // namespace

class ConnectionUtilsTestSuite : public ::testing::Test {
public:
    ConnectionUtilsTestSuite() {
        // Do not print errors and warnings on erroneous test cases
        // TODO: implement fake log and check error messages too.
        Log::setLocalLogLevel( Log::FATAL );
    }

    Json networkMapToJson( const NetworkMap& networkMap ) {
        Json result;
        auto& jsonList = result[ std::string( WifiListKey ) ] = Json::array();
        for( auto& elem : networkMap ) {
            jsonList.push_back( networkToJson( elem.second ) );
        }

        return result;
    }

    Json parseIwWifiScanResult( const std::string& scanResult ) {
        IwScanResultParser parser;
        parser.parse( scanResult );
        return networkMapToJson( parser.getScannedItems() );
    }

    Json parseIwprivWifiScanResult( const std::string& scanResult ) {
        IwprivScanResultParser parser;
        parser.parse( scanResult );
        return networkMapToJson( parser.getScannedItems() );
    }
};

TEST_F( ConnectionUtilsTestSuite, convertWifiSignalDbmToPercent ) {
    EXPECT_EQ( convertWifiSignalDbmToPercent( -20.5 ), 100 );
    EXPECT_EQ( convertWifiSignalDbmToPercent( -21. ), 99 );
    EXPECT_EQ( convertWifiSignalDbmToPercent( -23. ), 99 );
    EXPECT_EQ( convertWifiSignalDbmToPercent( -30. ), 96 );
    EXPECT_EQ( convertWifiSignalDbmToPercent( -40. ), 89 );
    EXPECT_EQ( convertWifiSignalDbmToPercent( -50. ), 79 );
    EXPECT_EQ( convertWifiSignalDbmToPercent( -60. ), 66 );
    EXPECT_EQ( convertWifiSignalDbmToPercent( -70. ), 49 );
    EXPECT_EQ( convertWifiSignalDbmToPercent( -80. ), 30 );
    EXPECT_EQ( convertWifiSignalDbmToPercent( -90. ), 8 );
    EXPECT_EQ( convertWifiSignalDbmToPercent( -92. ), 3 );
    EXPECT_EQ( convertWifiSignalDbmToPercent( -92.2 ), 1 );

    // check is monotonic
    {
        int prevPercent = 1;
        for( double dbm = -100; dbm < -10.; dbm += 0.1 ) {
            const auto percent = convertWifiSignalDbmToPercent( dbm );
            EXPECT_LE( prevPercent, percent );
            prevPercent = percent;
        }

        EXPECT_EQ( prevPercent, 100 );
    }
}

TEST_F( ConnectionUtilsTestSuite, parseOpenWrtVirtualInterfaceStatus ) {
    EXPECT_EQ( parseVirtualInterfaceStatus( SuccessfullOpenwrtVirtualInterfaceStatusWithIpv4AndIpv6.first ).dump( 4 ),
               Json::parse( SuccessfullOpenwrtVirtualInterfaceStatusWithIpv4AndIpv6.second ).dump( 4 ) );

    EXPECT_EQ( parseVirtualInterfaceStatus( SuccessfullOpenwrtVirtualInterfaceStatusWithIpv4.first ).dump( 4 ),
               Json::parse( SuccessfullOpenwrtVirtualInterfaceStatusWithIpv4.second ).dump( 4 ) );

    EXPECT_EQ( parseVirtualInterfaceStatus( "Interface wan not found" ).dump( 4 ),
               JSON( "status"
                     << "down" )
                   .dump( 4 ) );

    EXPECT_EQ( parseVirtualInterfaceStatus( NotReadyOpenWrtInterfaceStatus.first ).dump( 4 ),
               Json::parse( NotReadyOpenWrtInterfaceStatus.second ).dump( 4 ) );

    EXPECT_EQ( parseVirtualInterfaceStatus( SuccessfullOpenWrtInterfaceStatus3GProto.first ).dump( 4 ),
               Json::parse( SuccessfullOpenWrtInterfaceStatus3GProto.second ).dump( 4 ) );

    EXPECT_EQ( parseVirtualInterfaceStatus( NotReadyOpenWrtInterfaceStatus3GProto.first ).dump( 4 ),
               Json::parse( NotReadyOpenWrtInterfaceStatus3GProto.second ).dump( 4 ) );
}

TEST_F( ConnectionUtilsTestSuite, parseG150VirtualInterfaceStatus ) {
    EXPECT_EQ( parseVirtualInterfaceStatus( SuccessfullG150VirtualInterfaceStatusWithIpv4.first ).dump( 4 ),
               Json::parse( SuccessfullG150VirtualInterfaceStatusWithIpv4.second ).dump( 4 ) );
}

TEST_F( ConnectionUtilsTestSuite, parseIwprivWifiScanResult ) {
    EXPECT_EQ( getSortedWifiScanResult( parseIwprivWifiScanResult( IwprivSuccessfulScan.first ).dump( 4 ) ),
               getSortedWifiScanResult( Json::parse( IwprivSuccessfulScan.second ) ).dump( 4 ) );

    EXPECT_EQ( parseIwprivWifiScanResult( IwprivEmptyScan.first ).dump( 4 ),
               Json::parse( IwprivEmptyScan.second ).dump( 4 ) );

    EXPECT_EQ( parseIwprivWifiScanResult( "" ).dump( 4 ), Json::parse( IwprivEmptyScan.second ).dump( 4 ) );
}

TEST_F( ConnectionUtilsTestSuite, parseIwWifiScanResult ) {
    EXPECT_EQ( getSortedWifiScanResult( parseIwWifiScanResult( IwSuccessfulScan.first ) ).dump( 4 ),
               getSortedWifiScanResult( Json::parse( IwSuccessfulScan.second ) ).dump( 4 ) );

    EXPECT_EQ( parseIwWifiScanResult( "" ), Json::parse( R"({ "networks":[] })" ) );

    EXPECT_EQ( parseIwWifiScanResult( "BSS: abc" ), Json::parse( R"({ "networks":[] })" ) );
}

TEST_F( ConnectionUtilsTestSuite, parseIpCidr ) {
    auto ipToStr = []( uint32_t ip ) -> std::string {
        return std::to_string( ( ip >> 24 ) & 0xFF ) + "." + std::to_string( ( ip >> 16 ) & 0xFF ) + "." +
               std::to_string( ( ip >> 8 ) & 0xFF ) + "." + std::to_string( ( ip >> 0 ) & 0xFF );
    };

    auto toStr = [&]( std::pair< uint32_t, uint32_t > ipWithMask ) -> std::string {
        return ipToStr( ipWithMask.first ) + "/" + ipToStr( ipWithMask.second );
    };

    EXPECT_EQ( toStr( parseIpCidr( "192.168.11.41" ) ), "192.168.11.41/255.255.255.255" );
    EXPECT_EQ( toStr( parseIpCidr( "192.168.11.41/32" ) ), "192.168.11.41/255.255.255.255" );
    EXPECT_EQ( toStr( parseIpCidr( "192.168.11.41/31" ) ), "192.168.11.41/255.255.255.254" );
    EXPECT_EQ( toStr( parseIpCidr( "192.168.11.41/24" ) ), "192.168.11.41/255.255.255.0" );
    EXPECT_EQ( toStr( parseIpCidr( "192.168.11.41/23" ) ), "192.168.11.41/255.255.254.0" );
    EXPECT_EQ( toStr( parseIpCidr( "192.168.11.41/16" ) ), "192.168.11.41/255.255.0.0" );
    EXPECT_EQ( toStr( parseIpCidr( "192.168.11.41/8" ) ), "192.168.11.41/255.0.0.0" );
    EXPECT_EQ( toStr( parseIpCidr( "192.168.11.41/0" ) ), "192.168.11.41/0.0.0.0" );

    EXPECT_EQ( toStr( parseIpCidr( "0.0.0.0/0" ) ), "0.0.0.0/0.0.0.0" );
    EXPECT_EQ( toStr( parseIpCidr( "001.02.3.4/16" ) ), "1.2.3.4/255.255.0.0" );
    EXPECT_EQ( toStr( parseIpCidr( "255.254.253.252/16" ) ), "255.254.253.252/255.255.0.0" );
}

}  // namespace UT

}  // namespace ConnectionUtils
}  // namespace Network
}  // namespace HubPlatform
