#pragma once

#include "database_client/idbclient.h"
#include "json_utils/json_utils.h"

#include <map>

namespace HubPlatform {
namespace CloudDevices {
namespace SubscriptionUtils {

Json readSubscriptionFromDb( IDbClient& dbClient, const std::string& abstractUUID );
Json readAllSubscriptionsFromDb( IDbClient& dbClient );
void removeSubscriptionFromDb( IDbClient& dbClient, const std::string& abstractUUID );
void storeSubscriptionToDb( IDbClient& dbClient, const Json& subscription );
void removeSubscriptionItemFromDb( IDbClient& dbClient, const std::string& abstractUUID, const std::string& id );

Json createSubscriptionMessage( const JsonObject& subscriptionsTable,
                                const std::map< std::string, Json >& itemValueMap );

}  // namespace SubscriptionUtils
}  // namespace CloudDevices
}  // namespace HubPlatform
