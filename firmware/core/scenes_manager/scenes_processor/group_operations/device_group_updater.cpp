#include "device_group_updater.h"
#include "group_operations_utils.h"
#include "scenes_manager/scenes_engine/model/model_resources.h"
#include "scenes_manager/scenes_engine/helpers/json_helper.h"
#include "scenes_manager/scenes_processor/group_operations/device_group.h"
#include "json_utils/json_path.h"
#include "jsonrpc/jsonrpc.h"
#include <algorithm>

namespace HubPlatform {
namespace ScenesManager {
using namespace SceneEngine::Model::Resources;

JsonObject DeviceGroupUpdater::getDeviceGroupUpdateData( const JsonObject& dbItem, const Json& params ) {
    auto savedName = json_get_str( dbItem, SceneEngine::Model::Resources::DeviceGroup::Name );
    auto updateName = json_get_str( params, SceneEngine::Model::Resources::DeviceGroup::Name );
    auto savedPersistent = json_get_bool( dbItem, SceneEngine::Model::Resources::DeviceGroup::Persistent );
    auto updatePersistent = json_get_bool( params, SceneEngine::Model::Resources::DeviceGroup::Persistent );
    bool hasUpdatedPersistent =
        json_get( params, SceneEngine::Model::Resources::DeviceGroup::Persistent, updatePersistent );
    Json updatedJson = params;

    if( !hasUpdatedPersistent ) {
        updatedJson[ SceneEngine::Model::Resources::DeviceGroup::Persistent ] = savedPersistent;
    }

    return updatedJson;
}

JsonObject DeviceGroupUpdater::getDeviceGroupUpdateBroadcastData( const JsonObject& updatedData,
                                                                  const JsonObject& savedData ) {
    using namespace SceneEngine::Model::Resources;

    JsonArray categoriesToUpdate;
    JsonArray savedCategories;
    json_get_arr( updatedData, DeviceGroup::Categories, categoriesToUpdate );
    json_get_arr( savedData, DeviceGroup::Categories, savedCategories );
    JsonArray updatedCategories;
    bool isUpdateCategories = !ScenesGroupOperationsUtils::areObjectArraysEqual( categoriesToUpdate, savedCategories );
    JsonArray devicesToUpdate;
    JsonArray savedDevices;
    json_get_arr( updatedData, DeviceGroup::Devices, devicesToUpdate );
    json_get_arr( savedData, DeviceGroup::Devices, savedDevices );
    JsonArray updatedDevices;
    bool areDevicesUpdated = !ScenesGroupOperationsUtils::areStringArraysEqual( devicesToUpdate, savedDevices );
    JsonArray exceptionsToUpdate;
    JsonArray savedExceptions;

    json_get_arr( updatedData, DeviceGroup::Exceptions, exceptionsToUpdate );
    json_get_arr( savedData, DeviceGroup::Exceptions, savedExceptions );
    JsonArray updatedExceptions;
    bool areExceptionsUpdated =
        !ScenesGroupOperationsUtils::areStringArraysEqual( exceptionsToUpdate, savedExceptions );

    auto savedName = json_get_str( savedData, DeviceGroup::Name );
    auto updateName = json_get_str( updatedData, DeviceGroup::Name );
    auto savedPersistent = json_get_bool( savedData, DeviceGroup::Persistent );
    auto updatePersistent = json_get_bool( updatedData, DeviceGroup::Persistent );
    bool hasSavedPersistent = json_get( savedData, DeviceGroup::Persistent, updatePersistent );
    bool hasUpdatedPersistent = json_get( updatedData, DeviceGroup::Persistent, updatePersistent );

    Json updatedJson = JSON_EMPTY_OBJ;
    if( isUpdateCategories ) {
        updatedJson[ DeviceGroup::Categories ] = categoriesToUpdate;
    }

    if( areDevicesUpdated ) {
        updatedJson[ DeviceGroup::Devices ] = devicesToUpdate;
    }

    if( areExceptionsUpdated ) {
        updatedJson[ DeviceGroup::Exceptions ] = exceptionsToUpdate;
    }

    if( !hasUpdatedPersistent && hasSavedPersistent ) {
        updatedJson[ DeviceGroup::Persistent ] = savedPersistent;
    }

    if( savedName != updateName ) {
        updatedJson[ DeviceGroup::Name ] = updateName;
        updatedJson[ DeviceGroup::Sync ] = true;
    }
    else {
        if( !updatedJson.empty() ) {
            updatedJson[ DeviceGroup::Sync ] = false;
        }
    }

    return updatedJson;
}

bool fillDeviceArray( const std::string& removedId, const JsonArray& groupDevices, JsonArrayBuilder& changedDevices ) {
    bool changed = false;
    for( const auto& device : groupDevices ) {
        auto deviceId = device.get< std::string >();
        if( deviceId == removedId ) {
            changed = true;
        }
        else {
            changedDevices << deviceId;
        }
    }

    return changed;
}

void DeviceGroupUpdater::removeDeviceFromGroupOperations( const std::string& deviceId,
                                                          IDbClient& dbClient,
                                                          const BroadcastCallback& sendBroadcast ) {
    auto groups = dbClient.findAll( HaScenesDeviceGroupsDBName, JSON_EMPTY_OBJ, nullptr );
    for( auto& groupOne : groups ) {
        ScenesManager::DeviceGroupInfo info;
        info.fromJson( groupOne );
        auto id = info.getId();
        auto groupDevices = info.getDevices();
        auto groupExceptions = info.getExceptions();
        JsonArrayBuilder changedDevices;
        JsonArrayBuilder changedExceptions;
        bool isDevicesChanged = fillDeviceArray( deviceId, groupDevices, changedDevices );
        bool isExceptionsChanged = fillDeviceArray( deviceId, groupExceptions, changedExceptions );

        dbClient.update( HaScenesDeviceGroupsDBName,
                         JSON( "_id" << id ),
                         JSON( "$set" << JSON( DeviceGroup::Devices << groupDevices << DeviceGroup::Exceptions
                                                                    << changedExceptions ) ),
                         false,
                         true );

        JsonObject group = JSON_EMPTY_OBJ;
        if( sendBroadcast && ( isDevicesChanged || isExceptionsChanged ) ) {
            group[ DeviceGroup::Id ] = id;
            if( isDevicesChanged ) {
                group[ DeviceGroup::Devices ] = changedDevices;
            }
            if( isExceptionsChanged ) {
                group[ DeviceGroup::Exceptions ] = changedExceptions;
            }

            sendBroadcast( SCENES_DEVICE_GROUP_UPDATED, group );
        }
    }
}

}  // namespace ScenesManager
}  // namespace HubPlatform
