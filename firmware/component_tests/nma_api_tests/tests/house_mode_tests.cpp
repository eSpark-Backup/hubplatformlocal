#include "common_test_resources/fixtures/core_test_fixture.h"
#include "cored/model/resources/cored_resources.h"
#include "cored/house_modes/resources/house_mode_resources.h"
#include "nma_api_tests/tests/fixtures/house_mode_fixture.h"
#include "cored/remote_interface/cored_remote_interface.h"

namespace HubPlatform {
namespace Tests {

/*

TESTPLAN:

DONE    1. Set disarmed default
DONE        1.2 Set disarmed default for another mode, use device.houseModesOptions to tune mode."disarmed"
DONE    2. check default notifications
DONE    3. check disarmed default logic

DONE    5. Broadcasts
DONE        5.1 hub.modes.alarms_off
DONE        5.2 hub.modes.disarmed_devices
DONE        5.3 hub.modes.alarmed

DONE    6. Check mode.armed field

*/

class TestsHouseMode : public TestsHouseModeFixture {
public:
    void SetUp() override;
};

void TestsHouseMode::SetUp() {
    TestsHouseModeFixture::SetUp();
    callingFile( __FILE__ );
}

using namespace CoreDHouseModes::Resources;

TEST_F( TestsHouseMode, setDisarmedDefault /*1*/ ) {
    using namespace CoreD::Model::Resources;
    CoreDRemoteInterface::CoreDRemoteInterface coreRemoteInterface( nma() );
    JsonObject broadcast;

    auto gatewayId = getMainGateway( coreRemoteInterface, __LINE__ );
    auto houseModeId = coreRemoteInterface.getCurrentHouseModeId();
    if( houseModeId != "1" ) {
        coreRemoteInterface.setHouseMode( "1" );
        checkHouseModeSwitched( NMA_TEST_MAKE_MESSAGE( "" ) );
        checkHouseModeSwitched( NMA_TEST_MAKE_MESSAGE( "" ) );
    }
    houseModeId = "1";

    // add armed device1 and disarmed device2 in HouseMode 1;
    CoreD::Model::Device device01(
        gatewayId, "testDevice1", "smoke", "134_3_24", "security_sensor", false /*batteryPowered*/ );
    device01.setSubcategory( "smoke" );
    bool isGood;
    std::string device1Id, device2Id;
    std::tie( device1Id, isGood ) = coreRemoteInterface.addDevice( device01 );
    ASSERT_TRUE( isGood ) << "device wasn't created";

    checkBroadcast( device1Id,
                    "hub.device.added",
                    { { "$has-not", DeviceFields::ArmedFieldName } },
                    NMA_TEST_MAKE_MESSAGE( "device1 created" ) );

    auto securityItemId1 = addItem(
        coreRemoteInterface, device1Id, "security_threat", CoreD::Model::ValueType::Enum::Bool, true, true, __LINE__ );
    checkDeviceUpdateBroadcast( device1Id, { { DeviceFields::ArmedFieldName, true } }, __LINE__ );

    CoreD::Model::Device device02(
        gatewayId, "testDevice2", "motion", "134_3_24", "security_sensor", false /*batteryPowered*/ );
    device02.setSubcategory( "motion" );
    std::tie( device2Id, isGood ) = coreRemoteInterface.addDevice( device02 );
    ASSERT_TRUE( isGood );

    checkBroadcast( device2Id,
                    "hub.device.added",
                    { { "$has-not", DeviceFields::ArmedFieldName } },
                    NMA_TEST_MAKE_MESSAGE( "device2 created" ) );

    auto securityItemId2 = addItem(
        coreRemoteInterface, device2Id, "security_threat", CoreD::Model::ValueType::Enum::Bool, true, true, __LINE__ );
    checkDeviceUpdateBroadcast( device2Id, { { DeviceFields::ArmedFieldName, false } }, __LINE__ );

    // check DisarmedDefault  in HouseMode 1;
    {
        auto device1 = coreRemoteInterface.getSingleDevice( device1Id );
        EXPECT_TRUE( json_get_bool( device1, DeviceFields::ArmedFieldName ) );
        auto device2 = coreRemoteInterface.getSingleDevice( device2Id );
        EXPECT_FALSE( json_get_bool( device2, DeviceFields::ArmedFieldName ) );

        auto houseMode = coreRemoteInterface.getHouseModeById( "1" );
        EXPECT_TRUE( houseMode->m_disarmedDefault ) << houseMode->toJson();
        std::set< std::string > expected{ device2Id };
        EXPECT_EQ( houseMode->m_disarmedDeviceIds, expected );
    }
    // add DisarmedDevice (change setDisarmedDefault to false) and check DisarmedDefault in HouseMode 1;
    {
        const auto answer =
            nma().sendtoHubAndGetAnswer( { { "method", "hub.modes.disarmed_devices.add" },
                                           { "params", JSON( "modeId" << houseModeId << "deviceId" << device1Id ) } } );

        EXPECT_TRUE( answer[ "result" ].empty() );
        EXPECT_TRUE( answer[ "error" ].empty() );
        broadcast = NMA_TEST_WAIT_FOR_BROADCAST();
        EXPECT_EQ( json_get_str( broadcast, "msg_subclass" ), "hub.device.updated" );
        broadcast = NMA_TEST_WAIT_FOR_BROADCAST();
        EXPECT_EQ( json_get_str( broadcast, "msg_subclass" ), "hub.modes.changed" );
        EXPECT_FALSE( json_get_bool( broadcast[ "result" ], "disarmedDefault" ) );
        broadcast = NMA_TEST_WAIT_FOR_BROADCAST();
        EXPECT_EQ( json_get_str( broadcast, "msg_subclass" ), "hub.modes.disarmed_devices.added" );

        coreRemoteInterface.setDeviceArmed( device2Id, true );

        broadcast = NMA_TEST_WAIT_FOR_BROADCAST();
        EXPECT_EQ( json_get_str( broadcast, "msg_subclass" ), "hub.device.updated" );

        auto device1 = coreRemoteInterface.getSingleDevice( device1Id );
        EXPECT_FALSE( json_get_bool( device1, DeviceFields::ArmedFieldName ) );
        auto device2 = coreRemoteInterface.getSingleDevice( device2Id );
        EXPECT_TRUE( json_get_bool( device2, DeviceFields::ArmedFieldName ) );

        auto houseMode = coreRemoteInterface.getHouseModeById( "1" );
        EXPECT_FALSE( houseMode->m_disarmedDefault );
        std::set< std::string > expected{ device2Id, device1Id };
        EXPECT_EQ( houseMode->m_disarmedDeviceIds, expected );
    }
    // change setDisarmedDefault to true and check DisarmedDefault in HouseMode 1;
    {
        const auto answer = nma().sendtoHubAndGetAnswer(
            { { "method", "hub.modes.disarmed_default.set" },
              { "params", JSON( "modeId" << houseModeId << "disarmedDefault" << true ) } } );
        EXPECT_TRUE( answer[ "result" ].empty() );
        EXPECT_TRUE( answer[ "error" ].empty() );

        broadcast = NMA_TEST_WAIT_FOR_BROADCAST();
        EXPECT_EQ( json_get_str( broadcast, "msg_subclass" ), "hub.device.updated" );
        broadcast = NMA_TEST_WAIT_FOR_BROADCAST();
        EXPECT_EQ( json_get_str( broadcast, "msg_subclass" ), "hub.device.updated" );
        broadcast = NMA_TEST_WAIT_FOR_BROADCAST();
        EXPECT_EQ( json_get_str( broadcast, "msg_subclass" ), "hub.modes.changed" );
        EXPECT_TRUE( json_get_bool( broadcast[ "result" ], "disarmedDefault" ) );

        auto device1 = coreRemoteInterface.getSingleDevice( device1Id );
        EXPECT_TRUE( json_get_bool( device1, DeviceFields::ArmedFieldName ) );
        auto device2 = coreRemoteInterface.getSingleDevice( device2Id );
        EXPECT_FALSE( json_get_bool( device2, DeviceFields::ArmedFieldName ) );

        auto houseMode1 = coreRemoteInterface.getHouseModeById( "1" );
        EXPECT_TRUE( houseMode1->m_disarmedDefault );
        std::set< std::string > expected{ device2Id };
        EXPECT_EQ( houseMode1->m_disarmedDeviceIds, expected );
    }
    // remove devices
    size_t itemsNumber = coreRemoteInterface.getItemsByDeviceId( device1Id ).size();
    removeDevice( coreRemoteInterface, device1Id, itemsNumber, 0, __LINE__ );

    itemsNumber = coreRemoteInterface.getItemsByDeviceId( device2Id ).size();
    removeDevice( coreRemoteInterface, device2Id, itemsNumber, 0, __LINE__ );
}

TEST_F( TestsHouseMode, checkDefaultNotifications /*2*/ ) {
    const auto answer = nma().sendtoHubAndGetAnswer( { { "method", "hub.modes.get" }, { "params", JSON_EMPTY_OBJ } } );
    JsonObject result;
    JsonArray houseModes;
    ASSERT_TRUE( json_get_obj( answer, "result", result ) );
    ASSERT_TRUE( json_get_arr( result, "modes", houseModes ) );
    for( const auto& mode : houseModes ) {
        const JsonArray& notifications = mode.at( "notifications" );
        EXPECT_TRUE( notifications.is_null() );
    }
}

TEST_F( TestsHouseMode, checkDisarmedDefaultLogic /*3*/ ) {
    auto checkHouseModes =
        []( std::function< void( const JsonArray& houseModes, const std::string& codeLineStr ) > callback,
            int codeLine ) {
            std::string codeLineStr = "code line: " + std::to_string( codeLine );
            const auto answer =
                nma().sendtoHubAndGetAnswer( { { "method", "hub.modes.get" }, { "params", JSON_EMPTY_OBJ } } );
            JsonObject result;
            JsonArray houseModes;
            ASSERT_TRUE( json_get_obj( answer, "result", result ) ) << codeLineStr;
            ASSERT_TRUE( json_get_arr( result, "modes", houseModes ) ) << codeLineStr;
            callback( houseModes, codeLineStr );
        };
    checkHouseModes(
        []( const JsonArray& houseModes, const std::string& codeLineStr ) {
            for( const auto& mode : houseModes ) {
                EXPECT_TRUE( json_get_bool( mode, "disarmedDefault" ) ) << codeLineStr;
            }
        },
        __LINE__ );
    for( std::size_t i = 1; i <= 4; ++i ) {
        std::string id = std::to_string( i );
        std::string userId = "userId" + id;
        const auto answer = nma().sendtoHubAndGetAnswer(
            { { "method", "hub.modes.notifications.set" },
              { "params", JSON( "modeId" << id << "all" << false << "userIds" << JSON_ARRAY( userId ) ) } } );
        EXPECT_TRUE( answer[ "error" ].empty() );

        auto broadcast = NMA_TEST_WAIT_FOR_BROADCAST();
        EXPECT_EQ( json_get_str( broadcast, "msg_subclass" ), "hub.modes.changed" );
        EXPECT_FALSE( json_get_bool( broadcast[ "result" ], "disarmedDefault" ) );

        broadcast = NMA_TEST_WAIT_FOR_BROADCAST();
        EXPECT_EQ( json_get_str( broadcast, "msg_subclass" ), "hub.modes.notifications.notify_all" );
        EXPECT_FALSE( json_get_bool( broadcast[ "result" ], "all" ) );

        broadcast = NMA_TEST_WAIT_FOR_BROADCAST();
        EXPECT_EQ( json_get_str( broadcast, "msg_subclass" ), "hub.modes.notifications.added" );
        EXPECT_EQ( json_get_str( broadcast[ "result" ], "notification" ), userId );
    }
    checkHouseModes(
        []( const JsonArray& houseModes, const std::string& codeLineStr ) {
            std::size_t i = 1;
            for( const auto& mode : houseModes ) {
                std::string userId = "userId" + std::to_string( i );
                const JsonArray& notifications = mode.at( "notifications" );
                for( auto it = notifications.begin(); it != notifications.end(); ++it ) {
                    EXPECT_EQ( json_get_str( *it ), userId ) << codeLineStr;
                }
                EXPECT_FALSE( json_get_bool( mode, "disarmedDefault" ) ) << codeLineStr;
                ++i;
            }
        },
        __LINE__ );

    for( std::size_t i = 1; i <= 4; ++i ) {
        std::string id = std::to_string( i );
        const auto answer =
            nma().sendtoHubAndGetAnswer( { { "method", "hub.modes.disarmed_default.set" },
                                           { "params", JSON( "modeId" << id << "disarmedDefault" << true ) } } );
        EXPECT_TRUE( answer[ "error" ].empty() );

        auto broadcast = NMA_TEST_WAIT_FOR_BROADCAST();
        EXPECT_EQ( json_get_str( broadcast, "msg_subclass" ), "hub.modes.changed" );
        EXPECT_TRUE( json_get_bool( broadcast[ "result" ], "disarmedDefault" ) );
    }
    checkHouseModes(
        []( const JsonArray& houseModes, const std::string& codeLineStr ) {
            for( const auto& mode : houseModes ) {
                const JsonArray& notifications = mode.at( "notifications" );
                EXPECT_TRUE( notifications.is_null() ) << codeLineStr;
                EXPECT_TRUE( json_get_bool( mode, "disarmedDefault" ) ) << codeLineStr;
            }
        },
        __LINE__ );
}

TEST_F( TestsHouseMode, checkBroadcastsAlarmsOff /*5.1*/ ) {
    CoreDRemoteInterface::CoreDRemoteInterface coreRemoteInterface( nma() );

    const std::string houseModeId( "1" );
    // deviceId will be added by default to every houseMode's alarmsOffDevices (4 times). So, first, remove, than add.
    const auto deviceId =
        addDevice( coreRemoteInterface, "Controller Siren", "siren", "134_3_24", "siren", "", false, __LINE__ );

    // hub.modes.alarms_off.added
    {
        const auto answer = nma().sendtoHubAndGetAnswer( { { "method", "hub.modes.alarms_off.add" },
                                                           { "params",
                                                             JSON( "modeId"
                                                                   << "1"
                                                                   << "deviceId" << deviceId ) } } );

        EXPECT_TRUE( answer[ "result" ].empty() );
        EXPECT_TRUE( answer[ "error" ].empty() );

        auto broadcast = NMA_TEST_WAIT_FOR_BROADCAST();
        EXPECT_EQ( json_get_str( broadcast, "msg_subclass" ), "hub.modes.alarms_off.added" );
        EXPECT_EQ( json_get_str( broadcast[ "result" ], "modeId" ), houseModeId );
        EXPECT_EQ( json_get_str( broadcast[ "result" ], BroadcastFields::AlarmsOffDeviceFieldName ), deviceId );

        broadcast = waitForBroadcast( NMA_TEST_MAKE_MESSAGE( "disarmedDefault change broadcast" ) );
        EXPECT_EQ( json_get_str( broadcast, "msg_subclass" ), "hub.modes.changed" );
        EXPECT_FALSE( json_get_bool( broadcast[ "result" ], "disarmedDefault" ) );
    }

    // hub.modes.alarms_off.removed
    {
        const auto answer = nma().sendtoHubAndGetAnswer( { { "method", "hub.modes.alarms_off.remove" },
                                                           { "params",
                                                             JSON( "modeId"
                                                                   << "1"
                                                                   << "deviceId" << deviceId ) } } );

        EXPECT_TRUE( answer[ "result" ].empty() );
        EXPECT_TRUE( answer[ "error" ].empty() );

        auto broadcast = NMA_TEST_WAIT_FOR_BROADCAST();
        EXPECT_EQ( json_get_str( broadcast, "msg_subclass" ), "hub.modes.alarms_off.removed" );
        EXPECT_EQ( json_get_str( broadcast[ "result" ], "modeId" ), houseModeId );
        EXPECT_EQ( json_get_str( broadcast[ "result" ], BroadcastFields::AlarmsOffDeviceFieldName ), deviceId );
    }
}

TEST_F( TestsHouseMode, checkBroadcastsDisarmedDevices /*5.2*/ ) {
    CoreDRemoteInterface::CoreDRemoteInterface coreRemoteInterface( nma() );

    const std::string houseModeId( "1" );
    const auto deviceId = addDevice(
        coreRemoteInterface, "Smoke sensor", "smoke", "134_3_24", "security_sensor", "smoke", false, __LINE__ );

    using namespace CoreD::Model::Resources;
    auto securityItemId1 = addItem(
        coreRemoteInterface, deviceId, "security_threat", CoreD::Model::ValueType::Enum::Bool, true, true, __LINE__ );
    checkDeviceUpdateBroadcast( deviceId, { { DeviceFields::ArmedFieldName, true } }, __LINE__ );

    // hub.modes.disarmed_devices.added
    {
        const auto answer = nma().sendtoHubAndGetAnswer( { { "method", "hub.modes.disarmed_devices.add" },
                                                           { "params",
                                                             JSON( "modeId"
                                                                   << "1"
                                                                   << "deviceId" << deviceId ) } } );

        EXPECT_TRUE( answer[ "result" ].empty() );
        EXPECT_TRUE( answer[ "error" ].empty() );

        auto broadcast = NMA_TEST_WAIT_FOR_BROADCAST();
        EXPECT_EQ( json_get_str( broadcast, "msg_subclass" ), "hub.device.updated" );

        broadcast = NMA_TEST_WAIT_FOR_BROADCAST();
        EXPECT_EQ( json_get_str( broadcast, "msg_subclass" ), "hub.modes.changed" );
        EXPECT_FALSE( json_get_bool( broadcast[ "result" ], "disarmedDefault" ) );

        broadcast = NMA_TEST_WAIT_FOR_BROADCAST();
        EXPECT_EQ( json_get_str( broadcast, "msg_subclass" ), "hub.modes.disarmed_devices.added" );
        EXPECT_EQ( json_get_str( broadcast[ "result" ], "modeId" ), houseModeId );
        EXPECT_EQ( json_get_str( broadcast[ "result" ], BroadcastFields::DisarmedDeviceFieldName ), deviceId );
    }

    // hub.modes.disarmed_devices.removed
    {
        const auto answer = nma().sendtoHubAndGetAnswer( { { "method", "hub.modes.disarmed_devices.remove" },
                                                           { "params",
                                                             JSON( "modeId"
                                                                   << "1"
                                                                   << "deviceId" << deviceId ) } } );

        EXPECT_TRUE( answer[ "result" ].empty() );
        EXPECT_TRUE( answer[ "error" ].empty() );

        auto broadcast = NMA_TEST_WAIT_FOR_BROADCAST();
        EXPECT_EQ( json_get_str( broadcast, "msg_subclass" ), "hub.device.updated" );

        broadcast = NMA_TEST_WAIT_FOR_BROADCAST();
        EXPECT_EQ( json_get_str( broadcast, "msg_subclass" ), "hub.modes.disarmed_devices.removed" );
        EXPECT_EQ( json_get_str( broadcast[ "result" ], "modeId" ), houseModeId );
        EXPECT_EQ( json_get_str( broadcast[ "result" ], BroadcastFields::DisarmedDeviceFieldName ), deviceId );
    }
}

TEST_F( TestsHouseMode, checkBroadcastsHouseModeAlarmed /*5.3*/ ) {
    CoreDRemoteInterface::CoreDRemoteInterface coredRemote( nma() );

    const std::string houseModeId( "1" );
    const auto deviceId =
        addDevice( coredRemote, "Smoke sensor", "smoke", "134_3_24", "security_sensor", "smoke", false, __LINE__ );

    const auto valueType = CoreD::Model::ValueType::Enum::Bool;
    const auto itemId = addItem( coredRemote, deviceId, "security_threat", valueType, true, true, __LINE__ );

    using namespace CoreD::Model::Resources;
    checkDeviceUpdateBroadcast( deviceId, { { DeviceFields::ArmedFieldName, true } }, __LINE__ );

    coredRemote.setItemValue( JSON( ItemFields::IdFieldName << itemId << ItemFields::ValueFieldName << true ) );
    const Json broadcast = waitForBroadcast( __FILE__, __LINE__ );
    EXPECT_EQ( json_get_str( broadcast, "msg_subclass" ), "hub.item.updated" );

    testHouseModeAlarmedBroadcast( houseModeId, deviceId, __LINE__ );
}

TEST_F( TestsHouseMode, setDisarmedDefaultForDeviceHouseModeOptions /*1.2*/ ) {
    LOGGER_TRACE( "setDisarmedDefaultForDeviceHouseModeOptions STARTED..." );
    CoreDRemoteInterface::CoreDRemoteInterface coreRemoteInterface( nma() );
    JsonObject broadcast;

    const std::string houseModeHome = "1";
    const std::string houseModeNight = "3";

    auto gatewayId = getMainGateway( coreRemoteInterface, __LINE__ );
    auto houseModeId = coreRemoteInterface.getCurrentHouseModeId();
    if( houseModeId != houseModeHome ) {
        coreRemoteInterface.setHouseMode( houseModeHome );
        checkHouseModeSwitched( NMA_TEST_MAKE_MESSAGE( "" ) );
    }

    houseModeId = houseModeHome;

    // add a device here with houseModesOptions dictionary set.
    namespace Fields = HubPlatform::CoreD::Model::Resources::DeviceFields;
    CoreD::Model::Device device02(
        gatewayId, "testDeviceWithHouseModesOptions2", "motion", "sensor.motion", "security_sensor", false );

    Json houseModeOptions = { { Fields::HouseModesOptions::DisarmedFieldName,
                                Json::array( { houseModeHome, houseModeNight } ) } };
    device02.setHouseModesOptions( houseModeOptions );

    device02.setSubcategory( "motion" );

    bool isGood;
    std::string device2Id;
    std::tie( device2Id, isGood ) = coreRemoteInterface.addDevice( device02 );
    ASSERT_TRUE( isGood );

    checkBroadcast( device2Id,
                    "hub.device.added",
                    { { "$has-not", "armed" }, { "subcategory", "motion" } },
                    NMA_TEST_MAKE_MESSAGE( "motion sensor device added" ) );

    using namespace CoreD::Model::Resources;
    auto securityItemId2 = addItem(
        coreRemoteInterface, device2Id, "security_threat", CoreD::Model::ValueType::Enum::Bool, true, true, __LINE__ );
    checkDeviceUpdateBroadcast( device2Id, { { DeviceFields::ArmedFieldName, false } }, __LINE__ );

    {
        auto houseMode = coreRemoteInterface.getHouseModeById( houseModeHome );
        LOGGER_TRACE( houseMode->toJson().dump() );
        EXPECT_TRUE( houseMode->m_disarmedDefault );
        std::set< std::string > expected{ device2Id };
        EXPECT_EQ( houseMode->m_disarmedDeviceIds, expected );

        houseMode = coreRemoteInterface.getHouseModeById( houseModeNight );
        LOGGER_TRACE( houseMode->toJson().dump() );
        std::set< std::string > expectedAtNight{ device2Id };
        EXPECT_EQ( houseMode->m_disarmedDeviceIds, expectedAtNight );

        // armed
        houseMode = coreRemoteInterface.getHouseModeById( "2" );
        LOGGER_TRACE( houseMode->toJson().dump() );
        EXPECT_TRUE( houseMode->m_disarmedDeviceIds.empty() ) << "Should be armed when Away is selected";

        houseMode = coreRemoteInterface.getHouseModeById( "4" );
        LOGGER_TRACE( houseMode->toJson().dump() );
        EXPECT_TRUE( houseMode->m_disarmedDeviceIds.empty() ) << "Should be armed when Vacation is selected";
    }

    // remove Device from disarmed devices of Night house mode: it will set "disarmedDefault" to false and removes from
    // "disarmed" in HouseMode 2;
    {
        const auto answer = nma().sendtoHubAndGetAnswer(
            { { "method", "hub.modes.disarmed_devices.remove" },
              { "params", JSON( "modeId" << houseModeNight << "deviceId" << device2Id ) } } );

        EXPECT_TRUE( answer[ "result" ].empty() );
        EXPECT_TRUE( answer[ "error" ].empty() );
        checkModesChangedDisarmedDefault( false, NMA_TEST_MAKE_MESSAGE( "disarmedDefault should change to false" ) );
        broadcast = waitForBroadcast( NMA_TEST_MAKE_MESSAGE( "" ) );
        EXPECT_EQ( json_get_str( broadcast, "msg_subclass" ), "hub.modes.disarmed_devices.removed" );

        coreRemoteInterface.setDeviceArmed( device2Id, true );

        broadcast = waitForBroadcast( NMA_TEST_MAKE_MESSAGE( "" ) );
        EXPECT_EQ( json_get_str( broadcast, "msg_subclass" ), "hub.device.updated" );

        auto device2 = coreRemoteInterface.getSingleDevice( device2Id );
        EXPECT_TRUE( json_get_bool( device2, "armed" ) );

        auto houseMode = coreRemoteInterface.getHouseModeById( houseModeNight );
        EXPECT_FALSE( houseMode->m_disarmedDefault );
        std::set< std::string > expected{};
        EXPECT_EQ( houseMode->m_disarmedDeviceIds, expected );
    }

    // change setDisarmedDefault to true and check DisarmedDefault in HouseMode 3;
    {
        const bool disarmedDefault = true;
        setDisarmedDefault(
            houseModeNight, disarmedDefault, NMA_TEST_MAKE_MESSAGE( "disarmedDefault should change to true" ) );

        auto houseMode = coreRemoteInterface.getHouseModeById( houseModeNight );
        EXPECT_NE( houseMode, nullptr );
        EXPECT_TRUE( houseMode->m_disarmedDefault );
        std::set< std::string > expected{ device2Id };
        EXPECT_EQ( houseMode->m_disarmedDeviceIds, expected );
    }
    // remove devices
    unsigned long itemsNumber = coreRemoteInterface.getItemsByDeviceId( device2Id ).size();
    removeDevice( coreRemoteInterface, device2Id, itemsNumber, 0, __LINE__ );
}

TEST_F( TestsHouseMode, checkArmedField /* 6 */ ) {
    const auto answer = nma().sendtoHubAndGetAnswer( { { "method", "hub.modes.get" }, { "params", JSON_EMPTY_OBJ } } );
    JsonObject result;
    JsonArray houseModes;
    ASSERT_TRUE( json_get_obj( answer, "result", result ) );
    ASSERT_TRUE( json_get_arr( result, "modes", houseModes ) );
    for( const auto& mode : houseModes ) {
        Json armed;
        ASSERT_TRUE( json_get( mode, "armed", armed ) );
        ASSERT_TRUE( armed.is_boolean() );
    }
}

}  // namespace Tests
}  // namespace HubPlatform
