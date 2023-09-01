#include "common_test_resources/fixtures/core_test_fixture.h"
#include "common_test_resources/resources/cored_resources.h"
#include "test_utils/test_utils.h"

#include <gtest/gtest.h>

/*

TERMS

Optional collections =
    disarmed_devices && alarms_off_devices && cameras_off_devices && bypass_devices;

TESTPLAN

DONE    1. house_modes.get_mode
DONE        1.1 with empty optional collections
DONE        1.2 filled optional collections
DONE    2. house_modes.find_mode_by_name
DONE        2.1 with empty optional collections
DONE        2.2 filled optional collections

*/

namespace HubPlatform {
namespace Tests {

namespace Resources {
static char GetModeScript[] = "HUB:house_modes_test/scripts/get_mode";
static char FindModeByNameScript[] = "HUB:house_modes_test/scripts/find_mode_by_name";
}  // namespace Resources

class LuaHouseModesTests : public CoreNmaTest {
public:
    void SetUp() override;
    void TearDown() override;

    Json executeScript( const Json& scriptParams, const std::string& script, int codeLine );

    std::string createDevice( int codeLine );
    std::string createSecurityItem( const std::string& deviceId, int codeLine );

    std::vector< std::string > fillOptionalCollections( const std::string& modeId, int codeLine );
    void clearOptionalCollections( const std::string& modeId,
                                   const std::vector< std::string >& deviceIds,
                                   int codeLine );
    void switchDisarmedDefault( const std::string& modeId, bool disarmedDefault, int codeLine );

    static Json makeExpectedHouseMode( Json disarmedDevices = JSON_EMPTY_OBJ,
                                       Json alarmOffDevices = JSON_EMPTY_OBJ,
                                       Json camerasOffDevices = JSON_EMPTY_OBJ,
                                       Json bypassDevices = JSON_EMPTY_OBJ,
                                       const std::string& modeId = "1",
                                       const std::string& name = "Home",
                                       bool disarmedDefault = true,
                                       bool armed = false );
};

void LuaHouseModesTests::SetUp() {
    CoreNmaTest::SetUp();
    callingFile( __FILE__ );
}

void LuaHouseModesTests::TearDown() {
    clearExistingDevices( __LINE__ );
    CoreNmaTest::TearDown();
}

Json LuaHouseModesTests::executeScript( const Json& scriptParams, const std::string& script, int codeLine ) {
    auto request = replaceTokens( hubExtensionsPluginRun, { { "__SCRIPT__", script } } );
    request[ "params" ][ "scriptParams" ] = scriptParams;

    Json result;

    auto answer = nma().sendtoHubAndGetAnswer( request );
    auto error = answer[ "error" ];
    EXPECT_TRUE( error.empty() ) << error.dump( 4 ) << getCodeLineString( codeLine );
    if( error.empty() ) {
        auto broadcast = waitForBroadcast( "hub.extensions.plugin.ui_broadcast: " + getCodeLineString( codeLine ) );
        EXPECT_FALSE( broadcast.empty() ) << getCodeLineString( codeLine );

        Json broadcastResult;
        EXPECT_TRUE( json_get_obj( broadcast, "result", broadcastResult ) ) << getCodeLineString( codeLine );
        if( !broadcastResult.empty() )
            EXPECT_TRUE( json_get( broadcastResult, "value", result ) );

        checkExtensionsPluginRunProgressBroadcast( "started", __LINE__ );
        checkExtensionsPluginRunProgressBroadcast( "finished", __LINE__ );
    }

    return result;
}

std::string LuaHouseModesTests::createDevice( int codeLine ) {
    const bool batteryPowered = false;
    const std::string category = "security_sensor";
    const std::string name = "test_device";
    return addDevice( makeRemote(), name, "sensor", "134_3_24", category, "door", batteryPowered, codeLine );
}

std::string LuaHouseModesTests::createSecurityItem( const std::string& deviceId, int codeLine ) {
    const auto itemType = CoreD::Model::ValueType::Enum::Bool;
    return addItem( makeRemote(), deviceId, "security_threat", itemType, true, true, codeLine );
}

std::vector< std::string > LuaHouseModesTests::fillOptionalCollections( const std::string& modeId, int codeLine ) {
    CoreDRemoteInterface::CoreDInterface& coreRemote = makeRemote();

    std::string deviceId1 = createDevice( codeLine );  // disarmed device
    createSecurityItem( deviceId1, codeLine );
    checkDeviceUpdateBroadcast( deviceId1, KeyJsonList{}, codeLine );

    std::string deviceId2 = createDevice( codeLine );
    coreRemote.addAlarmsOffDevice( modeId, deviceId2 );
    Json broadcast = waitForBroadcast( __FILE__, codeLine );
    EXPECT_EQ( json_get_str( broadcast, "msg_subclass" ), "hub.modes.alarms_off.added" );

    broadcast = waitForBroadcast( __FILE__, codeLine );
    EXPECT_EQ( json_get_str( broadcast, "msg_subclass" ), "hub.modes.changed" );

    std::string deviceId3 = createDevice( codeLine );
    coreRemote.addCamerasOffDevice( modeId, deviceId3 );
    broadcast = waitForBroadcast( __FILE__, codeLine );
    EXPECT_EQ( json_get_str( broadcast, "msg_subclass" ), "hub.modes.cameras_off.added" );

    std::string deviceId4 = createDevice( codeLine );
    coreRemote.addBypassDevices( modeId, { deviceId4 } );
    broadcast = waitForBroadcast( __FILE__, codeLine );
    EXPECT_EQ( json_get_str( broadcast, "msg_subclass" ), "hub.modes.bypass_devices.added" );

    return { std::move( deviceId1 ), std::move( deviceId2 ), std::move( deviceId3 ), std::move( deviceId4 ) };
}

void LuaHouseModesTests::clearOptionalCollections( const std::string& modeId,
                                                   const std::vector< std::string >& deviceIds,
                                                   int codeLine ) {
    CoreDRemoteInterface::CoreDInterface& coreRemote = makeRemote();

    coreRemote.removeAlarmsOffDevice( modeId, deviceIds[ 1 ] );
    Json broadcast = waitForBroadcast( __FILE__, codeLine );
    EXPECT_EQ( json_get_str( broadcast, "msg_subclass" ), "hub.modes.alarms_off.removed" );

    coreRemote.removeCamerasOffDevice( modeId, deviceIds[ 2 ] );
    broadcast = waitForBroadcast( __FILE__, codeLine );
    EXPECT_EQ( json_get_str( broadcast, "msg_subclass" ), "hub.modes.cameras_off.removed" );

    coreRemote.removeBypassDevices( modeId, { deviceIds[ 3 ] } );
    broadcast = waitForBroadcast( __FILE__, codeLine );
    EXPECT_EQ( json_get_str( broadcast, "msg_subclass" ), "hub.modes.bypass_devices.removed" );
}

void LuaHouseModesTests::switchDisarmedDefault( const std::string& modeId, bool disarmedDefault, int codeLine ) {
    makeRemote().setDisarmedDefault( modeId, disarmedDefault );
    auto broadcast = waitForBroadcast( __FILE__, codeLine );
    EXPECT_EQ( json_get_str( broadcast, "msg_subclass" ), "hub.modes.changed" );
    int64_t timestamp;
    EXPECT_TRUE( json_get( broadcast[ "result" ], "timestamp", timestamp ) );
}

Json LuaHouseModesTests::makeExpectedHouseMode( Json disarmedDevices,
                                                Json alarmOffDevices,
                                                Json camerasOffDevices,
                                                Json bypassDevices,
                                                const std::string& modeId,
                                                const std::string& name,
                                                bool disarmedDefault,
                                                bool armed ) {
    return { { "id", modeId },
             { "name", name },
             { "default", disarmedDefault },
             { "disarmed_devices", std::move( disarmedDevices ) },
             { "alarms_off_devices", std::move( alarmOffDevices ) },
             { "cameras_off_devices", std::move( camerasOffDevices ) },
             { "bypass_devices", std::move( bypassDevices ) },
             { "armed", armed } };
}

TEST_F( LuaHouseModesTests, getMode_emptyOptionalCollections ) {  // 1.1
    const Json expected = makeExpectedHouseMode();
    const Json scriptResult = executeScript( { { "mode_id", "1" } }, Resources::GetModeScript, __LINE__ );
    EXPECT_EQ( expected, scriptResult );
}

TEST_F( LuaHouseModesTests, getMode_filledOptionalCollections ) {  // 1.2
    const std::string modeId = "1";
    const std::vector< std::string > deviceIds = fillOptionalCollections( modeId, __LINE__ );

    const Json expected = makeExpectedHouseMode( JSON_ARRAY_( deviceIds[ 0 ] ),
                                                 JSON_ARRAY_( deviceIds[ 1 ] ),
                                                 JSON_ARRAY_( deviceIds[ 2 ] ),
                                                 JSON_ARRAY_( deviceIds[ 3 ] ),
                                                 modeId,
                                                 "Home",
                                                 false,
                                                 false /* armed */ );
    const Json scriptResult = executeScript( { { "mode_id", modeId } }, Resources::GetModeScript, __LINE__ );
    EXPECT_EQ( expected, scriptResult );

    clearOptionalCollections( modeId, deviceIds, __LINE__ );
    switchDisarmedDefault( modeId, true, __LINE__ );
}

TEST_F( LuaHouseModesTests, findModeByName_emptyOptionalCollections ) {  // 2.1
    const Json expected = makeExpectedHouseMode();
    const Json scriptResult = executeScript( { { "mode_name", "Home" } }, Resources::FindModeByNameScript, __LINE__ );
    EXPECT_EQ( expected, scriptResult );
}

TEST_F( LuaHouseModesTests, findModeByName_filledOptionalCollections ) {  // 2.2
    const std::string modeId = "1";
    const std::vector< std::string > deviceIds = fillOptionalCollections( modeId, __LINE__ );

    const Json expected = makeExpectedHouseMode( JSON_ARRAY_( deviceIds[ 0 ] ),
                                                 JSON_ARRAY_( deviceIds[ 1 ] ),
                                                 JSON_ARRAY_( deviceIds[ 2 ] ),
                                                 JSON_ARRAY_( deviceIds[ 3 ] ),
                                                 modeId,
                                                 "Home",
                                                 false );
    const Json scriptResult = executeScript( { { "mode_name", "Home" } }, Resources::FindModeByNameScript, __LINE__ );
    EXPECT_EQ( expected, scriptResult );

    clearOptionalCollections( modeId, deviceIds, __LINE__ );
    switchDisarmedDefault( modeId, true, __LINE__ );
}

}  // namespace Tests
}  // namespace HubPlatform
