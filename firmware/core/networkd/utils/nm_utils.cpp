#include "networkd/utils/nm_utils.h"

#include "system_utils/system.h"
#include "log_utils/ezlo_log.h"
#include "tiny/include/tiny/tiny_consolecolors.h"

#include <memory>
#include <functional>

#include <string.h>
#include <sys/types.h>
#include <ifaddrs.h>
#include <arpa/inet.h>

namespace HubPlatform {
namespace Network {
namespace Utils {

// TODO: extend to get interfaces mac addresses
std::vector< InterfaceInfo > getSystemInterfaces() {
    std::vector< InterfaceInfo > interfaces;
    ifaddrs* ifaddr = nullptr;

    if( getifaddrs( &ifaddr ) != 0 ) {
        LOGGER_ERROR_RAW( cc::error( "Failed to get system network interfaces info" ) );
        return {};
    }

    std::unique_ptr< ifaddrs, std::function< void( ifaddrs* ) > > ifAddresses(
        ifaddr, []( ifaddrs* ifaddr ) { freeifaddrs( ifaddr ); } );

    for( auto* ifa = ifAddresses.get(); ifa; ifa = ifa->ifa_next ) {
        if( !ifa->ifa_addr || !ifa->ifa_netmask ||
            ( ifa->ifa_addr->sa_family != AF_INET && ifa->ifa_addr->sa_family != AF_INET6 ) ) {
            continue;
        }

        InterfaceInfo interfaceInfo;

        interfaceInfo.m_type =
            ( ifa->ifa_addr->sa_family == AF_INET ) ? InterfaceInfo::Type::IPV4 : InterfaceInfo::Type::IPV6;
        interfaceInfo.m_name = std::string( ifa->ifa_name );
        memcpy( &interfaceInfo.m_ipAddress, ifa->ifa_addr, sizeof( sockaddr ) );
        memcpy( &interfaceInfo.m_mask, ifa->ifa_netmask, sizeof( sockaddr ) );

        interfaces.push_back( std::move( interfaceInfo ) );
    }

    return interfaces;
}

std::string sockadrToString( const sockaddr& sa ) {
    char address[ 64 ] = { '\0' };

    switch( sa.sa_family ) {
    case AF_INET:
        inet_ntop( AF_INET, &reinterpret_cast< const sockaddr_in& >( sa ).sin_addr, address, sizeof( address ) );
        break;

    case AF_INET6:
        inet_ntop( AF_INET6, &reinterpret_cast< const sockaddr_in6& >( sa ).sin6_addr, address, sizeof( address ) );
        break;

    default:;
    }

    if( address[ 0 ] == '\0' ) {
        LOGGER_ERROR_RAW( cc::error( "Failed to convert sockaddr to string" ) );
    }

    return std::string( address );
}

std::string getDefaultRouteDevice() {
    return System::getAwkOutput( "ip route | awk '$1==\"default\" && (NF<=5 || $6==\"proto\") {print $5}'" );
}

// AT commands should be terminated by a CR symbol
std::string purgeCRSymbol( const std::string& atCommand ) {
    if( atCommand.back() == '\r' ) {
        return atCommand.substr( 0, atCommand.size() - 1 );
    }

    return atCommand;
}

}  // namespace Utils
}  // namespace Network
}  // namespace HubPlatform
