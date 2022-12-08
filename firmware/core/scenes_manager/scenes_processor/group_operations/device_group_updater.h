#pragma once

#include "json_utils/json_fwd.h"
#include "database_client/idbclient.h"
#include "external_controllers/ec_messenger.h"

namespace HubPlatform {
namespace ScenesManager {

class DeviceGroupUpdater {
public:
    using BroadcastCallback = std::function< void( MessageSubClasses messageSubClass, const JsonObject message ) >;
    static JsonObject getDeviceGroupUpdateBroadcastData( const JsonObject& updatedData, const JsonObject& savedData );
    static JsonObject getDeviceGroupUpdateData( const JsonObject& dbItem, const Json& params );
    static void removeDeviceFromGroupOperations( const std::string& deviceId,
                                                 IDbClient& dbClient,
                                                 const BroadcastCallback& sendBroadcast );
};

}  // namespace ScenesManager
}  // namespace HubPlatform
