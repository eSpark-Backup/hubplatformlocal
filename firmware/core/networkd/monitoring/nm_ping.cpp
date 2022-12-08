#include "networkd/monitoring/nm_ping.h"

#include "log_utils/ezlo_log.h"

#include <chrono>
#include <mutex>

namespace HubPlatform {
namespace Network {

namespace {

// TODO: read from config
const std::vector< std::string > DefaultPublicServers = { "1.1.1.1", "8.8.8.8" };
const std::vector< std::string > DefaultIPv6PublicServers = { "2606:4700:4700::1111", "2001:4860:4860::8888" };

std::mutex g_additionalServersMutex;
std::vector< std::string > g_additionalServers = {};

constexpr const std::chrono::seconds PingTimeout( 3 );

std::string createPingCommand( std::chrono::seconds pingTimeout,
                               const std::string& interfaceName,
                               const std::string& server,
                               IPVersion ipVersion ) {
    std::string result = ( ipVersion == IPVersion::IPv4 ) ? "ping " : "ping6 ";
    result += "-c 1 -W " + std::to_string( pingTimeout.count() ) + " ";

    if( !interfaceName.empty() ) {
        result += "-I " + interfaceName + " ";
    }

    result += server;

    return result;
}
}  // namespace

void setAdditionalServers( std::vector< std::string > servers ) {
    std::lock_guard< std::mutex > lock( g_additionalServersMutex );
    g_additionalServers = std::move( servers );
}

std::vector< std::string > getAdditionalServers() {
    std::lock_guard< std::mutex > lock( g_additionalServersMutex );
    return g_additionalServers;
}

bool ping( const std::string& interfaceName, IPVersion ipVersion ) {
    auto servers = ipVersion == ( IPVersion::IPv4 ) ? DefaultPublicServers : DefaultIPv6PublicServers;
    auto additionalServers = getAdditionalServers();
    servers.insert( servers.end(), additionalServers.begin(), additionalServers.end() );

    // TODO: run pings in parallel?
    auto it = std::find_if( servers.begin(), servers.end(), [&]( const std::string& server ) {
        auto pingCommand = createPingCommand( PingTimeout, interfaceName, server, ipVersion );
        auto redirectSuffix = " >/dev/null 2>&1";

        bool ok = system( ( pingCommand + redirectSuffix ).c_str() ) == 0;
        LOGGER_TRACE_RAW( "Ping " + server + " on " + interfaceName + ( ok ? " success" : " fail" ) );
        return ok;
    } );

    return it != servers.end();
}

}  // namespace Network
}  // namespace HubPlatform
