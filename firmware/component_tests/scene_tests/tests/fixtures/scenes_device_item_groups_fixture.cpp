#include "scenes_device_item_groups_fixture.h"
#include "scenes_manager/remote_interface/scenes_remote_interface.h"

#include <gtest/gtest.h>

namespace HubPlatform {
namespace Tests {

SceneDeviceItemGroupsFixture::SceneDeviceItemGroupsFixture()
: m_remote( makeRemote( nma() ) ) {}

void SceneDeviceItemGroupsFixture::SetUp() {
    SceneManagerTestsFixture::SetUp();
}

void SceneDeviceItemGroupsFixture::TearDown() {
    SceneManagerTestsFixture::TearDown();
}

JsonObject SceneDeviceItemGroupsFixture::waitForDeviceGroupCreatedBroadcast( int codeLine ) {
    std::string codeLineStr = getCodeLineString( codeLine );
    JsonObject broadcast = waitForBroadcast( codeLineStr );
    EXPECT_EQ( json_get_str( broadcast, "msg_subclass" ), "hub.scenes.device.group.created" ) << codeLineStr;
    return broadcast;
}

JsonObject SceneDeviceItemGroupsFixture::waitForDeviceGroupDeletedBroadcast( int codeLine ) {
    std::string codeLineStr = getCodeLineString( codeLine );
    JsonObject broadcast = waitForBroadcast( codeLineStr );
    EXPECT_EQ( json_get_str( broadcast, "msg_subclass" ), "hub.scenes.device.group.deleted" ) << codeLineStr;
    return broadcast;
}

std::string SceneDeviceItemGroupsFixture::createDeviceGroup( const JsonObject& request, int codeLine ) {
    std::string codeLineStr = getCodeLineString( codeLine );
    sendRequestAndCheckAnswer( request, codeLine );
    auto br = waitForDeviceGroupCreatedBroadcast( codeLine );
    std::string deviceGroupId = br[ "result" ][ "_id" ];
    EXPECT_FALSE( deviceGroupId.empty() ) << codeLineStr;
    return deviceGroupId;
}

void SceneDeviceItemGroupsFixture::deleteDeviceGroup( const std::string& groupId, int codeLine ) {
    ScenesManager::RemoteInterface scenesRemote( nma() );
    std::string codeLineStr = getCodeLineString( codeLine );

    Json answer = scenesRemote.groupOperations()->deviceGroup().deleteGroup( JSON( "id" << groupId ) );
    ASSERT_FALSE( hasError( answer ) ) << answer;

    auto br = waitForDeviceGroupDeletedBroadcast( codeLine );
    std::string deviceGroupId = br[ "result" ][ "_id" ];
    EXPECT_EQ( deviceGroupId, groupId ) << codeLineStr;
}

std::string SceneDeviceItemGroupsFixture::createItemGroup( const JsonObject& request, int codeLine ) {
    std::string codeLineStr = getCodeLineString( codeLine );
    sendRequestAndCheckAnswer( request, codeLine );
    JsonObject broadcast = waitForBroadcast( codeLineStr );
    EXPECT_EQ( json_get_str( broadcast, "msg_subclass" ), "hub.scenes.item.group.created" ) << codeLineStr;
    std::string groupId = broadcast[ "result" ][ "_id" ];
    EXPECT_FALSE( groupId.empty() ) << codeLineStr;
    return groupId;
}

void SceneDeviceItemGroupsFixture::deleteItemGroup( const std::string& groupId, int codeLine ) {
    ScenesManager::RemoteInterface scenesRemote( nma() );
    std::string codeLineStr = getCodeLineString( codeLine );

    Json params = JSON( "id" << groupId << "force" << false );
    Json answer = scenesRemote.groupOperations()->itemGroup().deleteGroup( params );
    ASSERT_FALSE( hasError( answer ) ) << answer;

    JsonObject broadcast = waitForBroadcast( codeLineStr );
    EXPECT_EQ( json_get_str( broadcast, "msg_subclass" ), "hub.scenes.item.group.deleted" ) << codeLineStr;
    std::string deviceGroupId = broadcast[ "result" ][ "_id" ];
    EXPECT_EQ( deviceGroupId, groupId ) << codeLineStr;
}

std::string SceneDeviceItemGroupsFixture::makeDevice( const std::string& category,
                                                      const std::string& subcategory,
                                                      int codeLine ) {
    return addDevice(
        m_remote, "Controller device-item group device", category, category, category, subcategory, false, codeLine );
}

std::string SceneDeviceItemGroupsFixture::makeIntItem( const std::string& deviceId,
                                                       const std::string& itemName,
                                                       int value,
                                                       int codeLine ) {
    using namespace CoreD::Model;
    auto valueType = ValueType::Enum::Int;
    CoreD::Model::Item item( deviceId,
                             itemName,
                             ValueType::Enum::Int,
                             true,
                             true,
                             ValueFactory::createItemValue( valueType, Json( value ) ) );
    return addItem( m_remote, item, codeLine, nullptr );
}

std::string SceneDeviceItemGroupsFixture::makeStringItem( const std::string& deviceId,
                                                          const std::string& itemName,
                                                          const std::string& value,
                                                          int codeLine ) {
    using namespace CoreD::Model;
    auto valueType = ValueType::Enum::String;
    CoreD::Model::Item item(
        deviceId, itemName, valueType, true, true, ValueFactory::createItemValue( valueType, Json( value ) ) );
    return addItem( m_remote, item, codeLine, nullptr );
}

}  // namespace Tests
}  // namespace HubPlatform
