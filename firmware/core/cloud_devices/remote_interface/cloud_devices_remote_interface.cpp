#include "cloud_devices/remote_interface/cloud_devices_remote_interface.h"
#include "cloud_devices/resources/cloud_devices_resources.h"
#include "service_interfaces/groups_names.h"
#include "service_interfaces/service.h"
#include "database_client/dbqueryparser.h"
#include "database_client/db_tables_names.h"

namespace HubPlatform {
namespace CloudDevices {

RemoteInterface::RemoteInterface( Services::Service& hostService )
: m_hostService( hostService ) {}

JsonObject RemoteInterface::createCloudSubscription( const JsonObject& params ) {
    return m_hostService.sendRequest( GroupsNames::CLOUD_DEVICES,
                                      params,
                                      MessageSubClasses::HUB_CLOUD_SUBSCRIPTION_CREATE,
                                      std::chrono::seconds( 60 ) );
}

JsonObject RemoteInterface::updateCloudSubscription( const JsonObject& params ) {
    return m_hostService.sendRequest( GroupsNames::CLOUD_DEVICES,
                                      params,
                                      MessageSubClasses::HUB_CLOUD_SUBSCRIPTION_UPDATE,
                                      std::chrono::seconds( 60 ) );
}

JsonObject RemoteInterface::deleteCloudSubscription( const JsonObject& params ) {
    return m_hostService.sendRequest( GroupsNames::CLOUD_DEVICES,
                                      params,
                                      MessageSubClasses::HUB_CLOUD_SUBSCRIPTION_DELETE,
                                      std::chrono::seconds( 60 ) );
}

JsonObject RemoteInterface::getCloudSubscriptionsList() {
    auto abstracts = JSON_EMPTY_ARR;
    auto subscriptions =
        m_hostService.getDBClient().findAll( Resources::DB::TableNames::Subscriptions, JSON_EMPTY_OBJ, nullptr );
    if( subscriptions.empty() )
        return JSON( Resources::CloudRPCParams::Abstracts << abstracts );

    for( auto& subscription : subscriptions ) {
        std::string uuid = json_get_str( subscription, Resources::DB::SubscriptionFields::Id );
        json_remove( subscription, Resources::DB::SubscriptionFields::Id );
        json_add_str( subscription, Resources::RPCParams::AbstractUUID, uuid );
        abstracts.push_back( subscription );
    }
    JsonObject result;
    result[ Resources::CloudRPCParams::Abstracts ] = std::move( abstracts );
    return result;
}

}  // namespace CloudDevices
}  // namespace HubPlatform
