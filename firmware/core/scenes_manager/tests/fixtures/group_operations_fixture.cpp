#include "scenes_manager/tests/fixtures/group_operations_fixture.h"
#include "scenes_manager/tests/resources/generic_scene_json.h"
#include "scenes_manager/scenes_engine/model/model_resources.h"

namespace HubPlatform {
namespace UT {

using KeyStringList = std::vector< std::pair< std::string, std::string > >;

const std::string IdField = "_id";
const std::string NameField = "name";

JsonObject GroupOperationsFixture::validateDeviceGroupCreate( JsonObject& message ) {
    return m_scenesHandler->validateCreateGroupRequest( message, true );
}

JsonObject GroupOperationsFixture::validateItemGroupCreate( JsonObject& message ) {
    return m_scenesHandler->validateItemGroupRequest( message, true );
}

JsonObject GroupOperationsFixture::getDeviceGroupUpdateBroadcastData( const JsonObject& updated,
                                                                      const JsonObject& saved ) {
    return m_scenesHandler->getDeviceGroupUpdateBroadcastData( updated, saved );
}

void GroupOperationsFixture::addDevice( const std::string& deviceId ) {
    getService().getDBClient().insert( HaDevicesDBName, JSON( IdField << deviceId ) );
}

void GroupOperationsFixture::addDeviceGroup( const std::string& groupId, const std::string& name ) {
    getService().getDBClient().insert( HaScenesDeviceGroupsDBName, JSON( IdField << groupId << NameField << name ) );
}

void GroupOperationsFixture::addItemGroup( const std::string& groupId, const std::string& name ) {
    getService().getDBClient().insert( HaScenesItemGroupsDBName, JSON( IdField << groupId << NameField << name ) );
}

JsonObject GroupOperationsFixture::getDeviceGroupsList( const JsonObject& params ) {
    return m_scenesHandler->processDeviceGroupsListRequest( params );
}

JsonObject GroupOperationsFixture::getDeviceGroupGet( const JsonObject& params ) {
    return m_scenesHandler->processDeviceGroupGetRequest( params );
}

JsonObject GroupOperationsFixture::getItemGroupsList( const JsonObject& params ) {
    return m_scenesHandler->processItemGroupsListRequest( params );
}

JsonObject GroupOperationsFixture::getItemGroupGet( const JsonObject& params ) {
    return m_scenesHandler->processItemGroupGetRequest( params );
}

JsonObject GroupOperationsFixture::getItemGroupUpdateBroadcastData( const JsonObject& updated,
                                                                    const JsonObject& saved ) {
    return m_scenesHandler->getItemGroupUpdateBroadcastData( updated, saved );
}

}  // namespace UT
}  // namespace HubPlatform
