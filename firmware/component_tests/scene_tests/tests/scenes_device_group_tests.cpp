#include "common_test_resources/fixtures/scene_manager_tests_fixture.h"
#include "common_test_resources/resources/scene_manager_resources.h"
#include "common_test_resources/resources/cored_resources.h"

#include "cored/model/values/value_factory.h"
#include <gtest/gtest.h>
#include "tiny/tiny_consolecolors.h"

namespace HubPlatform {
namespace Tests {

/*

TESTPLAN

DONE    1 Create device group
DODE        1.1 Create device group
DONE    2 Delete device group
DODE        2.1 Delete device group
DODE        2.2 Delete persistent device group error
DODE        2.3 Delete persistent device group with 'force' flag
DODE        2.4 Delete device group error
DONE    3 Update device group
DODE        3.1 Update device group
DODE        3.2 Update device group error
DONE    4 Device groups list
DONE        4.1 Device groups list success
DONE    5 Device groups list
DONE        5.1 Device groups list success
DONE        5.2 Device groups list error
DONE   6 Device removing test
DONE        6.1 Remove device which include in group['device']
DONE        6.2 Remove device which not include in group['device']
DONE        6.3 Remove device which include in group['exceptions']
*/

class SceneDeviceGroupTests : public SceneManagerTestsFixture {
public:
    static void SetUpTestCase();
    void SetUp() override;

    void removeDevice( const std::string& deviceId, int codeLine );
};

void SceneDeviceGroupTests::SetUpTestCase() {}

void SceneDeviceGroupTests::SetUp() {
    SceneManagerTestsFixture::SetUp();
    callingFile( __FILE__ );
}

void SceneDeviceGroupTests::removeDevice( const std::string& deviceId, int codeLine ) {
    auto& remoteInterface = makeRemote( nma() );
    auto itemsNumber = remoteInterface.getItemsByDeviceId( deviceId ).size();
    CoreNmaTest::removeDevice( remoteInterface, deviceId, itemsNumber, 0, codeLine );
}

TEST_F( SceneDeviceGroupTests, createDeviceGroup ) {  // 1.1
    auto request =
        replaceTokensInString( scenesDeviceGroupCreate, { { "_DEVICE_GROUP_NAME_", "testCreateDeviceGroup" } } );
    sendRequestAndCheckAnswer( Json::parse( request ), __LINE__ );
    JsonObject broadcast = waitForBroadcast( getCodeLineString( __LINE__ ) );
    EXPECT_EQ( json_get_str( broadcast, "msg_subclass" ), "hub.scenes.device.group.created" );
}

TEST_F( SceneDeviceGroupTests, deleteDeviceGroup ) {  // 2.1
    auto request = replaceTokensInString( scenesDeviceGroupCreate, { { "_DEVICE_GROUP_NAME_", "nameDeleted" } } );
    sendRequestAndCheckAnswer( Json::parse( request ), __LINE__ );
    JsonObject broadcast = waitForBroadcast( getCodeLineString( __LINE__ ) );
    auto id = json_get_str( broadcast[ "result" ], "_id" );
    request = replaceTokensInString( scenesDeviceGroupDelete, { { "__GROUP_ID__", id }, { "__FORCE__", "false" } } );
    sendRequestAndCheckAnswer( Json::parse( request ), __LINE__ );
    broadcast = waitForBroadcast( getCodeLineString( __LINE__ ) );
    EXPECT_EQ( json_get_str( broadcast, "msg_subclass" ), "hub.scenes.device.group.deleted" );
}

TEST_F( SceneDeviceGroupTests, deletePersistentDeviceGroup ) {  // 2.2
    auto request = replaceTokensInString( scenesPersistentDeviceGroupCreate,
                                          { { "_DEVICE_GROUP_NAME_", "nameDeleted1" }, { "__PERSISTENT__", "true" } } );
    sendRequestAndCheckAnswer( Json::parse( request ), __LINE__ );
    JsonObject broadcast = waitForBroadcast( getCodeLineString( __LINE__ ) );
    auto id = json_get_str( broadcast[ "result" ], "_id" );
    request = replaceTokensInString( scenesDeviceGroupDelete, { { "__GROUP_ID__", id }, { "__FORCE__", "false" } } );
    auto answer = nma().sendtoHubAndGetAnswer( Json::parse( request ) );
    const auto& error = answer[ "error" ];
    EXPECT_FALSE( error.empty() );
    EXPECT_EQ( json_get_int( error, "code" ), -32602 );
    EXPECT_EQ( json_get_str( error, "data" ), "ezlo.delete.group.impossible" );
    EXPECT_EQ( json_get_str( error, "message" ), "Can't delete persistent group" );
}

TEST_F( SceneDeviceGroupTests, deleteForcePersistentDeviceGroup ) {  // 2.3
    auto request = replaceTokensInString( scenesPersistentDeviceGroupCreate,
                                          { { "_DEVICE_GROUP_NAME_", "nameDeleted2" }, { "__PERSISTENT__", "true" } } );
    sendRequestAndCheckAnswer( Json::parse( request ), __LINE__ );
    JsonObject broadcast = waitForBroadcast( getCodeLineString( __LINE__ ) );
    auto id = json_get_str( broadcast[ "result" ], "_id" );
    request = replaceTokensInString( scenesDeviceGroupDelete, { { "__GROUP_ID__", id }, { "__FORCE__", "true" } } );
    sendRequestAndCheckAnswer( Json::parse( request ), __LINE__ );
    broadcast = waitForBroadcast( getCodeLineString( __LINE__ ) );
    EXPECT_EQ( json_get_str( broadcast, "msg_subclass" ), "hub.scenes.device.group.deleted" );
}

TEST_F( SceneDeviceGroupTests, deleteDeviceGroupError ) {  // 2.4
    auto request =
        replaceTokensInString( scenesDeviceGroupDelete, { { "__GROUP_ID__", "badId" }, { "__FORCE__", "false" } } );

    auto answer = nma().sendtoHubAndGetAnswer( Json::parse( request ) );
    const auto& error = answer[ "error" ];
    EXPECT_FALSE( error.empty() );
    EXPECT_EQ( json_get_str( error, "data" ), "ezlo.not.exist" );
    EXPECT_EQ( json_get_str( error, "field" ), "_id" );
}

TEST_F( SceneDeviceGroupTests, updateDeviceGroup ) {  // 3.1
    auto request = replaceTokensInString( scenesDeviceGroupCreate, { { "_DEVICE_GROUP_NAME_", "name2" } } );
    sendRequestAndCheckAnswer( Json::parse( request ), __LINE__ );
    JsonObject broadcast = waitForBroadcast( getCodeLineString( __LINE__ ) );
    EXPECT_EQ( json_get_str( broadcast, "msg_subclass" ), "hub.scenes.device.group.created" );
    auto deviceGroupId = json_get_str( broadcast[ "result" ], "_id" );

    request = replaceTokensInString( scenesDeviceGroupUpdate, { { "__GROUP_ID__", deviceGroupId } } );
    sendRequestAndCheckAnswer( Json::parse( request ), __LINE__ );
    broadcast = waitForBroadcast( getCodeLineString( __LINE__ ) );
    EXPECT_EQ( json_get_str( broadcast, "msg_subclass" ), "hub.scenes.device.group.updated" );
}

TEST_F( SceneDeviceGroupTests, updateGroupError ) {  // 3.2
    auto request = replaceTokensInString( scenesDeviceGroupUpdate, { { "__GROUP_ID__", "badId" } } );
    auto answer = nma().sendtoHubAndGetAnswer( Json::parse( request ) );
    const auto& error = answer[ "error" ];
    EXPECT_FALSE( error.empty() );
    EXPECT_EQ( json_get_str( error, "data" ), "ezlo.not.exist" );
    EXPECT_EQ( json_get_str( error, "field" ), "_id" );
}

TEST_F( SceneDeviceGroupTests, deviceGroupsListTest ) {  // 4.1
    auto request = replaceTokensInString( scenesDeviceGroupCreate, { { "_DEVICE_GROUP_NAME_", "name3" } } );
    sendRequestAndCheckAnswer( Json::parse( request ), __LINE__ );
    JsonObject broadcast = waitForBroadcast( getCodeLineString( __LINE__ ) );
    auto deviceGroupId = json_get_str( broadcast[ "result" ], "_id" );
    EXPECT_EQ( json_get_str( broadcast, "msg_subclass" ), "hub.scenes.device.group.created" );
    auto answer = nma().sendtoHubAndGetAnswer( Json::parse( deviceGroupsList ) );
    const auto& error = answer[ "error" ];
    EXPECT_TRUE( error.empty() );
    const auto& deviceGroups = answer[ "result" ][ "deviceGroups" ];
    EXPECT_FALSE( deviceGroups.empty() );
    bool idExist = false;
    for( auto group : deviceGroups ) {
        EXPECT_FALSE( group.empty() );
        const auto id = json_get_str( group, "_id" );
        if( id == deviceGroupId ) {
            idExist = true;
            EXPECT_EQ( json_get_str( group, "name" ), "name3" );
            JsonArray categories;
            json_get_arr( group, "categories", categories );
            EXPECT_EQ(
                categories.dump(),
                R"([{"category":"security_sensor","subcategories":["co2","leak"]},{"category":"scene_controller","exceptions":["panic"]}])" );
            ASSERT_TRUE( json_get_arr( group, "devices", categories ) );
            ASSERT_TRUE( json_get_arr( group, "exceptions", categories ) );
        }
    }

    EXPECT_TRUE( idExist );
}

TEST_F( SceneDeviceGroupTests, deviceGroupGetTest ) {  // 5.1
    auto request = replaceTokensInString( scenesDeviceGroupCreate, { { "_DEVICE_GROUP_NAME_", "name4" } } );
    sendRequestAndCheckAnswer( Json::parse( request ), __LINE__ );
    JsonObject broadcast = waitForBroadcast( getCodeLineString( __LINE__ ) );
    EXPECT_EQ( json_get_str( broadcast, "msg_subclass" ), "hub.scenes.device.group.created" );
    auto deviceGroupId = json_get_str( broadcast[ "result" ], "_id" );

    request = replaceTokensInString( deviceGroupGet, { { "__GROUP_ID__", deviceGroupId } } );
    auto answer = nma().sendtoHubAndGetAnswer( Json::parse( request ) );
    const auto& error = answer[ "error" ];
    EXPECT_TRUE( error.empty() );
    const auto& result = answer[ "result" ];
    EXPECT_FALSE( result.empty() );
    EXPECT_EQ( json_get_str( result, "_id" ), deviceGroupId );
    EXPECT_EQ( json_get_str( result, "name" ), "name4" );
    JsonArray categories;
    json_get_arr( result, "categories", categories );
    EXPECT_EQ(
        categories.dump(),
        R"([{"category":"security_sensor","subcategories":["co2","leak"]},{"category":"scene_controller","exceptions":["panic"]}])" );
    json_get_arr( result, "devices", categories );
    json_get_arr( result, "exceptions", categories );
}

TEST_F( SceneDeviceGroupTests, deviceGroupGetErrorTest ) {  // 5.2
    auto request = replaceTokensInString( scenesDeviceGroupCreate, { { "_DEVICE_GROUP_NAME_", "name5" } } );
    sendRequestAndCheckAnswer( Json::parse( request ), __LINE__ );
    JsonObject broadcast = waitForBroadcast( getCodeLineString( __LINE__ ) );
    EXPECT_EQ( json_get_str( broadcast, "msg_subclass" ), "hub.scenes.device.group.created" );
    auto deviceGroupId = json_get_str( broadcast[ "result" ], "_id" );

    request = replaceTokensInString( deviceGroupGet, { { "__GROUP_ID__", "badId" } } );
    auto answer = nma().sendtoHubAndGetAnswer( Json::parse( request ) );
    const auto& error = answer[ "error" ];
    EXPECT_FALSE( error.empty() );
    EXPECT_EQ( json_get_str( error, "data" ), "ezlo.not.exist" );
    EXPECT_EQ( json_get_str( error, "field" ), "_id" );
    EXPECT_EQ( json_get_str( error, "message" ), "Record with the key does not exist" );
}

TEST_F( SceneDeviceGroupTests, checkDeviceGroupAfterDeviceRemoving ) {  // 6.1
    using namespace CoreD::Model::Resources;
    auto deviceInfo = addDeviceWithOneItem( CoreD::Model::ValueType::Enum::Int, __LINE__ );
    auto deviceId = deviceInfo.m_deviceId;
    std::string devices;
    devices += "[\"" + deviceId + "\"]";

    auto request = replaceTokensInString(
        scenesDeviceGroupCreateWithDevices,
        { { "_DEVICE_GROUP_NAME_", "DeviceRemoving" }, { "__DEVICES__", devices }, { "__EXCEPTIONS__", "[]" } } );
    sendRequestAndCheckAnswer( Json::parse( request ), __LINE__ );
    JsonObject broadcast = waitForBroadcast( getCodeLineString( __LINE__ ) );
    EXPECT_EQ( json_get_str( broadcast, "msg_subclass" ), "hub.scenes.device.group.created" );

    removeDevice( deviceInfo.m_deviceId, __LINE__ );
    broadcast = waitForBroadcast( getCodeLineString( __LINE__ ) );
    const auto& result = broadcast[ "result" ];
    EXPECT_FALSE( result.empty() );

    auto broadcastName = json_get_str( broadcast[ "result" ], "name" );
    JsonArray broadcastDevicesArray;
    json_get_arr( broadcast[ "result" ], "devices", broadcastDevicesArray );
    EXPECT_EQ( json_get_str( broadcast, "msg_subclass" ), "hub.scenes.device.group.updated" );
    EXPECT_TRUE( broadcastDevicesArray.empty() );
}

TEST_F( SceneDeviceGroupTests, checkDeviceGroupAfterDeleteAnotherDevice ) {  // 6.2
    using namespace CoreD::Model::Resources;
    auto deviceInfo = addDeviceWithOneItem( CoreD::Model::ValueType::Enum::Int, __LINE__ );
    auto deviceId = deviceInfo.m_deviceId;
    std::string devices;
    devices += "[\"" + deviceId + "\"]";

    auto request = replaceTokensInString(
        scenesDeviceGroupCreateWithDevices,
        { { "DeviceNotRemoving", "DeviceNotRemoving" }, { "__DEVICES__", devices }, { "__EXCEPTIONS__", "[]" } } );
    sendRequestAndCheckAnswer( Json::parse( request ), __LINE__ );
    JsonObject broadcast = waitForBroadcast( getCodeLineString( __LINE__ ) );
    EXPECT_EQ( json_get_str( broadcast, "msg_subclass" ), "hub.scenes.device.group.created" );
    auto deviceGroupId = json_get_str( broadcast[ "result" ], "_id" );
    auto deviceInfo1 = addDeviceWithOneItem( CoreD::Model::ValueType::Enum::Int, __LINE__ );
    removeDevice( deviceInfo1.m_deviceId, __LINE__ );

    request = replaceTokensInString( deviceGroupGet, { { "__GROUP_ID__", deviceGroupId } } );
    auto answer = nma().sendtoHubAndGetAnswer( Json::parse( request ) );
    const auto& error = answer[ "error" ];
    EXPECT_TRUE( error.empty() );
    const auto& result = answer[ "result" ];
    EXPECT_FALSE( result.empty() );
    JsonArray devicesArray;
    json_get_arr( result, "devices", devicesArray );
    EXPECT_EQ( devicesArray.size(), 1 );

    removeDevice( deviceInfo.m_deviceId, __LINE__ );
    broadcast = waitForBroadcast( getCodeLineString( __LINE__ ) );
    EXPECT_FALSE( broadcast[ "result" ].empty() );
}

TEST_F( SceneDeviceGroupTests, checkDeviceGroupAfterExceptionRemoving ) {  // 6.3
    using namespace CoreD::Model::Resources;
    auto deviceInfo = addDeviceWithOneItem( CoreD::Model::ValueType::Enum::Int, __LINE__ );
    std::string devices;
    devices += "[\"" + deviceInfo.m_deviceId + "\"]";
    auto exceptionDeviceInfo = addDeviceWithOneItem( CoreD::Model::ValueType::Enum::Int, __LINE__ );
    std::string exceptions;
    exceptions += "[\"" + exceptionDeviceInfo.m_deviceId + "\"]";

    auto request = replaceTokensInString( scenesDeviceGroupCreateWithDevices,
                                          { { "_DEVICE_GROUP_NAME_", "ExceptionRemoving" },
                                            { "__DEVICES__", devices },
                                            { "__EXCEPTIONS__", exceptions } } );
    sendRequestAndCheckAnswer( Json::parse( request ), __LINE__ );
    JsonObject broadcast = waitForBroadcast( getCodeLineString( __LINE__ ) );
    EXPECT_EQ( json_get_str( broadcast, "msg_subclass" ), "hub.scenes.device.group.created" );

    removeDevice( exceptionDeviceInfo.m_deviceId, __LINE__ );
    broadcast = waitForBroadcast( getCodeLineString( __LINE__ ) );
    const auto& result = broadcast[ "result" ];
    EXPECT_FALSE( result.empty() );

    auto broadcastName = json_get_str( broadcast[ "result" ], "name" );
    JsonArray broadcastDevicesArray;
    json_get_arr( broadcast[ "result" ], "exceptions", broadcastDevicesArray );
    EXPECT_EQ( json_get_str( broadcast, "msg_subclass" ), "hub.scenes.device.group.updated" );
    EXPECT_TRUE( broadcastDevicesArray.empty() );

    removeDevice( deviceInfo.m_deviceId, __LINE__ );
    broadcast = waitForBroadcast( getCodeLineString( __LINE__ ) );
    EXPECT_FALSE( broadcast[ "result" ].empty() );
}

}  // namespace Tests
}  // namespace HubPlatform
