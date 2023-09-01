#include "scenes_manager/remote_interface/scenes_remote_interface.h"

#include "json_utils/json_fwd.h"
#include "jsonrpc/message.h"
#include "item_group_operation.h"

namespace HubPlatform {
namespace ScenesManager {

ItemGroupOperation::ItemGroupOperation( Services::Service& service )
: m_hostService( service ) {}

JsonObject ItemGroupOperation::create( const JsonObject& params ) {
    auto response =
        m_hostService.sendRequest( GroupsNames::SCENE_MANAGER, params, MessageSubClasses::SCENES_ITEM_GROUP_CREATE );
    return response;
};

JsonObject ItemGroupOperation::deleteGroup( const JsonObject& params ) {
    auto response =
        m_hostService.sendRequest( GroupsNames::SCENE_MANAGER, params, MessageSubClasses::SCENES_ITEM_GROUP_DELETE );
    return response;
};

JsonObject ItemGroupOperation::update( const JsonObject& params ) {
    auto response =
        m_hostService.sendRequest( GroupsNames::SCENE_MANAGER, params, MessageSubClasses::SCENES_ITEM_GROUP_UPDATE );
    return response;
}

JsonObject ItemGroupOperation::list( const JsonObject& params ) {
    auto response =
        m_hostService.sendRequest( GroupsNames::SCENE_MANAGER, params, MessageSubClasses::SCENES_ITEM_GROUPS_LIST );
    return response;
}

JsonObject ItemGroupOperation::get( const JsonObject& params ) {
    auto response =
        m_hostService.sendRequest( GroupsNames::SCENE_MANAGER, params, MessageSubClasses::SCENES_ITEM_GROUP_GET );
    return response;
}

}  // namespace ScenesManager
}  // namespace HubPlatform
