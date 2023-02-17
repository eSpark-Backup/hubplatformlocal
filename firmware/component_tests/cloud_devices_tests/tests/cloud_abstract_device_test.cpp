//< EFWR-3896
#include "common_test_resources/fixtures/core_test_fixture.h"
#include "cored/remote_interface/cored_remote_interface.h"
#include "json_utils/json_wrapper.h"
#include "cloud_devices_tests/application.h"
#include <gtest/gtest.h>

namespace HubPlatform {
namespace Tests {

class CloudAbstractDeviceTest : public CoreNmaTest {
public:
    void SetUp() override;
    void TearDown() override;
};

void CloudAbstractDeviceTest::SetUp() {
    CoreNmaTest::SetUp();
    clearExistingDevices( __LINE__ );
    callingFile( __FILE__ );
}

void CloudAbstractDeviceTest::TearDown() {
    clearExistingDevices( __LINE__ );
    CoreNmaTest::TearDown();
}

TEST_F( CloudAbstractDeviceTest, addAbstractDevice ) {
    JsonObject broadcast;
    CoreDRemoteInterface::CoreDRemoteInterface coreRemoteInterface( nma() );
    auto deviceId = addDevice( coreRemoteInterface,
                               "testDevice1",
                               "cloud",
                               "134_3_24",
                               "abstract",
                               "abstract",
                               false /*batteryPowered*/,
                               __LINE__ );

    auto itemId =
        addItem( coreRemoteInterface, deviceId, "testItem1", CoreD::Model::ValueType::Enum::Int, true, true, __LINE__ );

    Json subscriptionCreateCommand = R"({
        "method": "hub.cloud.subscription.create",
        "id": "_ID_",
        "params": {
            "abstractUUID": "1234-abcd-...",
            "mappings": [
                {
                    "capability": "testItem1",
                    "variable": "status",
                    "valueType": "int",
                    "dictionaryType": false,
                    "value": 1
                }
           ]
        }
    })"_json;

    Json answer = nma().sendtoHubAndGetAnswer( subscriptionCreateCommand );
    broadcast = waitForBroadcast( NMA_TEST_MAKE_MESSAGE( "expected gateway.created" ) );
    EXPECT_EQ( json_get_str( broadcast, "msg_subclass" ), "hub.gateway.added" );
    broadcast = waitForBroadcast( NMA_TEST_MAKE_MESSAGE( "expected device.created" ) );
    EXPECT_EQ( json_get_str( broadcast, "msg_subclass" ), "hub.device.added" );
    broadcast = waitForBroadcast( NMA_TEST_MAKE_MESSAGE( "expected item.created" ) );
    EXPECT_EQ( json_get_str( broadcast, "msg_subclass" ), "hub.item.added" );

    coreRemoteInterface.removeItem( itemId );
    broadcast = waitForBroadcast( NMA_TEST_MAKE_MESSAGE( "expected item.removed" ) );
    EXPECT_EQ( json_get_str( broadcast, "msg_subclass" ), "hub.item.removed" );

    coreRemoteInterface.removeDevice( deviceId );
    broadcast = waitForBroadcast( NMA_TEST_MAKE_MESSAGE( "expected device.removed" ) );
    EXPECT_EQ( json_get_str( broadcast, "msg_subclass" ), "hub.device.removed" );
}

}  // namespace Tests
}  // namespace HubPlatform

// EFWR-3896>