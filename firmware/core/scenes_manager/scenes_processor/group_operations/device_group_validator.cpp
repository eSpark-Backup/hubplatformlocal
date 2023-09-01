#include "device_group_validator.h"
#include "scenes_manager/modify_scene/scene_scale_modifier.h"
#include "scenes_manager/scenes_engine/model/model_resources.h"
#include "group_operations_utils.h"
#include "database_client/dbqueryparser.h"
#include "jsonrpc/jsonrpc.h"
#include "common_utils/hub_info_helper.h"
#include "cored/model/values/value_factory.h"

#include "database_client/idbclient.h"

namespace HubPlatform {
namespace ScenesManager {
using namespace SceneEngine::Model::Resources;

JsonObject DeviceGroupValidator::checkDevices( const JsonArray& devices, JsonPath& path ) {
    JsonObject answer;
    using namespace HubPlatform::ScenesManager;
    for( const auto& device : devices ) {
        auto id = device.get< std::string >();
        auto existingDevice = m_dbClient.findOne( HaDevicesDBName, JSON( "_id" << id ), nullptr );
        if( existingDevice.empty() ) {
            std::string reason = "Device id: " + id;
            setErrorWithField( answer, Errors::NO_DEVICE_FOUND, reason, path.toString() );
            return answer;
        }
    }
    return JSON_EMPTY_OBJ;
}

JsonObject DeviceGroupValidator::validateCreateGroupRequest( const JsonObject& params, bool checkRequiredParams ) {
    using namespace HubPlatform::ScenesManager;
    JsonObject answer = JSON_EMPTY_OBJ;
    JsonPath path;
    auto gv = path.makeGuard( "params" );
    auto checkCategory = [&]( const JsonObject& category, JsonPath& path ) {
        auto gv = path.makeGuard( DeviceGroup::Category );
        if( !ScenesGroupOperationsUtils::checkStringParam( answer, category, DeviceGroup::Category, path, false ) ||
            !ScenesGroupOperationsUtils::checkStringArrayParam(
                answer, category, DeviceGroup::Subcategories, path, false ) ||
            !ScenesGroupOperationsUtils::checkStringArrayParam(
                answer, category, DeviceGroup::Exceptions, path, false ) ) {
            return false;
        }

        return true;
    };

    if( !ScenesGroupOperationsUtils::checkStringParam( answer, params, DeviceGroup::Name, path, checkRequiredParams ) ||
        !ScenesGroupOperationsUtils::checkArrayParam( answer, params, DeviceGroup::Categories, path ) ||
        !ScenesGroupOperationsUtils::checkStringArrayParam( answer, params, DeviceGroup::Devices, path, false ) ||
        !ScenesGroupOperationsUtils::checkStringArrayParam( answer, params, DeviceGroup::Exceptions, path, false ) ) {
        return answer;
    }

    std::string name;
    name = json_get_str( params, DeviceGroup::Name );
    try {
        jsonCheckString( name, JsonStringChecks::RESTRICTED_LENGTH );
    }
    catch( ... ) {
        setErrorWithField( answer, Errors::TOO_LONG, "", DeviceGroup::Name );
        return answer;
    }
    JsonArray categories;
    JsonArray devices;
    JsonArray exceptions;
    bool hasCategories = json_get_arr( params, DeviceGroup::Categories, categories ) && !categories.empty();
    bool hasDevices = json_get_arr( params, DeviceGroup::Devices, devices ) && !devices.empty();
    bool hasExceptions = json_get_arr( params, DeviceGroup::Exceptions, exceptions ) && !exceptions.empty();

    if( hasCategories ) {
        auto gv = path.makeGuard( DeviceGroup::Categories );
        for( const auto& category : categories ) {
            if( !checkCategory( category, path ) ) {
                return answer;
            }
        }
    }

    if( hasDevices ) {
        auto gv = path.makeGuard( DeviceGroup::Devices );
        answer = checkDevices( devices, path );
        if( !answer.empty() ) {
            return answer;
        }
    }
    if( hasExceptions ) {
        auto gv = path.makeGuard( DeviceGroup::Exceptions );
        answer = checkDevices( exceptions, path );
        if( !answer.empty() ) {
            return answer;
        }
    }

    return answer;
}

}  // namespace ScenesManager
}  // namespace HubPlatform
