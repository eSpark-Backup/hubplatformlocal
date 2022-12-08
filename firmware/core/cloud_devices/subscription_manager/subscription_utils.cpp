#include "cloud_devices/subscription_manager/subscription_utils.h"

#include "cloud_devices/subscription_manager/subscription_exceptions.h"
#include "cloud_devices/resources/cloud_devices_resources.h"
#include "cored/model/resources/cored_resources.h"

namespace HubPlatform {
namespace CloudDevices {
namespace SubscriptionUtils {

Json readSubscriptionFromDb( IDbClient& dbClient, const std::string& abstractUUID ) {
    using namespace Resources::DB;
    auto query = JSON( SubscriptionFields::Id << abstractUUID );
    auto result = dbClient.findOne( TableNames::Subscriptions, query, nullptr );
    if( result.empty() )
        return {};
    return result;
}

Json readAllSubscriptionsFromDb( IDbClient& dbClient ) {
    auto result = dbClient.findAll( Resources::DB::TableNames::Subscriptions, JSON_EMPTY_OBJ, nullptr );
    if( result.empty() )
        return {};
    return result;
}

void removeSubscriptionFromDb( IDbClient& dbClient, const std::string& abstractUUID ) {
    using namespace Resources::DB;
    dbClient.remove( TableNames::Subscriptions, JSON( SubscriptionFields::Id << abstractUUID ) );
}

void removeSubscriptionItemFromDb( IDbClient& dbClient, const std::string& abstractUUID, const std::string& id ) {
    using namespace Resources::DB;

    Json subscription = readSubscriptionFromDb( dbClient, abstractUUID );

    JsonObject mappings;
    json_get_arr( subscription, SubscriptionFields::Mappings, mappings );
    JsonArrayBuilder editMappings;
    for( auto mapping : mappings ) {
        if( !( mapping.find( SubscriptionFields::ItemId ) != mapping.end() &&
               json_get_str( mapping, SubscriptionFields::ItemId ) == id ) ) {
            editMappings << mapping;
        }
    }

    subscription[ SubscriptionFields::Mappings ] = editMappings;

    dbClient.remove( TableNames::Subscriptions, JSON( SubscriptionFields::Id << abstractUUID ) );
    dbClient.update( Resources::DB::TableNames::Subscriptions, JSON_EMPTY_OBJ, subscription, true, false );
}

void storeSubscriptionToDb( IDbClient& dbClient, const Json& subscription ) {
    using namespace Resources::DB;
    auto validateField = []( const Json& json, const std::string& key ) {
        if( json_get_str( json, key ).empty() ) {
            throw SubscriptionError( "Unable to store invalid record to database: invalid field: " + key );
        }
    };

    validateField( subscription, SubscriptionFields::Id );
    validateField( subscription, SubscriptionFields::DeviceId );

    auto& mappings = subscription.at( SubscriptionFields::Mappings );
    for( auto& mapping : mappings ) {
        validateField( mapping, SubscriptionFields::Capability );
        validateField( mapping, SubscriptionFields::Variable );
        validateField( mapping, SubscriptionFields::ItemId );
    }

    dbClient.update( Resources::DB::TableNames::Subscriptions, JSON_EMPTY_OBJ, subscription, true, false );
}

Json createSubscriptionMessage( const JsonObject& subscriptionsTable,
                                const std::map< std::string, Json >& itemValueMap ) {
    using namespace Resources;
    using namespace Resources::DB;

    const int cloudApiVersion = 1;

    auto abstracts = JSON_EMPTY_ARR;
    for( auto& data : subscriptionsTable ) {
        Json abstract;

        auto gatewayName = json_get_str( data, SubscriptionFields::GatewayName );
        if( !gatewayName.empty() )
            abstract[ CloudRPCParams::GatewayName ] = gatewayName;

        abstract[ CloudRPCParams::AbstractUUID ] = data.at( SubscriptionFields::Id );
        abstract[ CloudRPCParams::DeviceId ] = data.at( SubscriptionFields::DeviceId );

        JsonArray mappings;

        const JsonArray& dbMappings = data.at( SubscriptionFields::Mappings );
        for( auto& dbMapping : dbMappings ) {
            Json mapping;
            mapping[ CloudRPCParams::Capability ] = dbMapping.at( SubscriptionFields::Capability );
            mapping[ CloudRPCParams::Variable ] = dbMapping.at( SubscriptionFields::Variable );

            std::string itemId = json_get_str( dbMapping, SubscriptionFields::ItemId );
            mapping[ CloudRPCParams::ItemId ] = itemId;

            auto& itemJson = itemValueMap.at( itemId );

            auto valueJson = itemJson.at( HubPlatform::CoreD::Model::Resources::ItemFields::ValueFieldName );
            std::string valueType =
                json_get_str( itemJson, HubPlatform::CoreD::Model::Resources::ItemFields::TypeFieldName );

            if( valueType == "int" || valueType == "float" || valueType == "string" ) {
                mapping[ CloudRPCParams::Value ] = valueJson;
            }
            else if( valueType == "bool" ) {
                mapping[ CloudRPCParams::Value ] = int( valueJson.get< bool >() );
            }
            else {
                mapping[ CloudRPCParams::Value ] = valueJson.dump();
            }

            mappings.push_back( std::move( mapping ) );
        }

        abstract[ CloudRPCParams::Mappings ] = std::move( mappings );
        abstracts.push_back( std::move( abstract ) );
    }

    Json params;
    params[ CloudRPCParams::Abstracts ] = std::move( abstracts );

    Json message;
    message[ API::General::Api ] = cloudApiVersion;
    message[ API::General::Method ] = CloudMethods::CloudControllerSubscribeAbstracts;
    message[ API::General::Params ] = std::move( params );

    return message;
}

}  // namespace SubscriptionUtils
}  // namespace CloudDevices
}  // namespace HubPlatform
