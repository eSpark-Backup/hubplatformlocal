#include "networkd/routes/nm_openwrt_routes_manager.h"
#include "networkd/resources/nm_resources.h"
#include "networkd/utils/nm_utils.h"

#include "system_utils/system.h"
#include "json_utils/json_utils.h"

#include <algorithm>
#include <limits>

namespace HubPlatform {
namespace Network {

std::string OpenwrtRoutesManager::checkAndFixRoutes( const std::vector< Json >& interfaces ) {
    using namespace Resources::InterfaceInfo;

    std::string currentDevice = Utils::getDefaultRouteDevice();

    /* sort interfaces by their priorities in a descending order
     * if an interface has no priority specified, it's implied as a lowest one */
    auto compareObj = []( const Json& left, const Json& right ) {
        auto leftPriority = json_get_int( left, Fields::Priority, std::numeric_limits< int >::max() );
        auto rightPriority = json_get_int( right, Fields::Priority, std::numeric_limits< int >::max() );

        return leftPriority < rightPriority;  // higher priority, lower value
    };

    auto sortedInterfaces = interfaces;
    std::sort( sortedInterfaces.begin(), sortedInterfaces.end(), compareObj );

    for( auto& interface : sortedInterfaces ) {
        if( json_get_str( interface, Fields::Status ) != Statuses::Up ) {
            continue;
        }

        if( !json_get_bool( interface, Fields::InternetAvailable ) ) {
            continue;
        }

        auto device = json_get_str( interface, Fields::Id );
        if( device.empty() ) {
            continue;
        }

        auto& ipv4 = interface[ Fields::Ipv4 ];
        if( ipv4.empty() ) {
            continue;
        }

        auto gateway = json_get_str( ipv4, Fields::Gateway );
        if( gateway.empty() ) {
            continue;
        }

        if( device == currentDevice ) {
            if( json_get_bool( interface, Fields::InternetAvailable ) ) {
                // Do not try to change interface if current one has internet
                break;
            }
            continue;
        }

        std::string action;
        if( currentDevice.empty() ) {
            LOGGER_DEBUG_RAW( cc::debug( "Adding missing default route through " ) + cc::note( device ) );
            action = "add";
        }
        else {
            // NOTE: prefer ethernet (1st interface) over wifi (2nd interface)
            LOGGER_DEBUG_RAW( cc::debug( "Replacing default route to " ) + cc::note( device ) );
            action = "replace";
        }
        int result = 0;
        std::tie( std::ignore, result ) =
            System::getCommandOutput( ( "ip route " + action + " default via " + gateway + " dev " + device ).c_str() );
        if( result == 0 ) {
            return device;
        }
        else {
            LOGGER_DEBUG_RAW( cc::debug( "Failed to change a network route, err: " ) +
                              cc::note( std::to_string( result ) ) + cc::debug( ", gateway: " ) + cc::note( gateway ) +
                              cc::debug( ", device: " ) + cc::note( device ) );
        }
    }
    if( currentDevice.empty() ) {
        LOGGER_DEBUG_RAW( cc::debug( "No interface is suitable for default route" ) );
    }

    for( auto& interface : sortedInterfaces ) {
        bool shouldRemove = false;
        bool shouldAdd = false;
        auto device = json_get_str( interface, Fields::Id );
        int metric = ( json_get_int( interface, Fields::Priority, 0 ) + 1 ) * 100;

        auto& ipv4 = interface[ Fields::Ipv4 ];
        if( ipv4.empty() ) {
            shouldRemove = true;
        }

        std::string gateway;
        if( !shouldRemove ) {
            gateway = json_get_str( ipv4, Fields::Gateway );
            if( gateway.empty() ) {
                shouldRemove = true;
            }
        }

        std::string correctRoute;
        if( !shouldRemove ) {
            correctRoute =
                System::getAwkOutput( "ip route | awk '$1==\"default\" && $3==\"" + gateway + "\" && $5==\"" + device +
                                      "\" && $7==\"" + std::to_string( metric ) + "\"'" );
            if( correctRoute.empty() ) {
                shouldAdd = true;
            }
        }

        std::string routes = System::getAwkOutput( "ip route | awk '$1==\"default\" && $5==\"" + device +
                                                   "\" && NF>5 && $6!=\"proto\"'" );

        if( !routes.empty() ) {
            std::stringstream ss( routes );
            std::string route;
            while( std::getline( ss, route ) ) {
                if( route == correctRoute ) {
                    continue;
                }
                LOGGER_DEBUG_RAW( cc::debug( "Deleting incorrect additional default route through " ) +
                                  cc::note( device ) );
                int result = 0;
                std::tie( std::ignore, result ) = System::getCommandOutput( ( "ip route del " + route ).c_str() );
                if( result != 0 ) {
                    LOGGER_DEBUG_RAW( cc::debug( "Failed" ) );
                }
            }
        }
        if( shouldAdd ) {
            LOGGER_DEBUG_RAW( cc::debug( "Adding additional default route through " ) + cc::note( device ) );
            int result = 0;
            std::tie( std::ignore, result ) = System::getCommandOutput(
                ( "ip route add default via " + gateway + " dev " + device + " metric " + std::to_string( metric ) )
                    .c_str() );
            if( result != 0 ) {
                LOGGER_DEBUG_RAW( cc::debug( "Failed to add additional default route for device: " ) +
                                  cc::note( device ) + cc::debug( ", gateway: " ) + cc::note( gateway ) +
                                  cc::debug( ", reason: " ) + cc::note( std::to_string( result ) ) );
            }
        }
    }

    return currentDevice;
}

}  // namespace Network
}  // namespace HubPlatform
