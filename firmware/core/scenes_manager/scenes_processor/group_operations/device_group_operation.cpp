#include "json_utils/json_fwd.h"
#include "device_group_operation.h"

namespace HubPlatform {
namespace ScenesManager {

DeviceGroupOperation::DeviceGroupOperation( Services::Service& service )
: m_hostService( service ) {}

JsonObject DeviceGroupOperation::create( const JsonObject& params ) {
    auto response =
        m_hostService.sendRequest( GroupsNames::SCENE_MANAGER, params, MessageSubClasses::SCENES_DEVICE_GROUP_CREATE );
    return response;
};

JsonObject DeviceGroupOperation::deleteGroup( const JsonObject& params ) {
    auto response =
        m_hostService.sendRequest( GroupsNames::SCENE_MANAGER, params, MessageSubClasses::SCENES_DEVICE_GROUP_DELETE );
    return response;
};

JsonObject DeviceGroupOperation::update( const JsonObject& params ) {
    auto response =
        m_hostService.sendRequest( GroupsNames::SCENE_MANAGER, params, MessageSubClasses::SCENES_DEVICE_GROUP_UPDATE );
    return response;
}

JsonObject DeviceGroupOperation::list( const JsonObject& params ) {
    auto response =
        m_hostService.sendRequest( GroupsNames::SCENE_MANAGER, params, MessageSubClasses::SCENES_DEVICE_GROUPS_LIST );
    return response;
}

JsonObject DeviceGroupOperation::get( const JsonObject& params ) {
    auto response =
        m_hostService.sendRequest( GroupsNames::SCENE_MANAGER, params, MessageSubClasses::SCENES_DEVICE_GROUP_GET );
    return response;
}

}  // namespace ScenesManager
}  // namespace HubPlatform
