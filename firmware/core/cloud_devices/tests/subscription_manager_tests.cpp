#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "cloud_devices/subscription_manager/subscription_manager.h"
#include "cloud_devices/subscription_manager/subscription_helper.h"
#include "cloud_devices/resources/cloud_devices_resources.h"
#include "cloud_devices/subscription_manager/subscription_utils.h"

#include "cloud_devices/tests/fakes/fake_cloud_devices_service.h"
#include "cloud_devices/tests/fakes/fake_cored_remote_interface.h"
#include "cloud_devices/tests/mocks/subscription_helper_mock.h"

#include <chrono>

/*

TESTPLAN

DONE    1. CloudDevices tests
DONE        1.1  Create subscription
DONE        1.2  Delete subscription
DONE        1.3  Delete unknown subscription
DONE        1.4  Send subscriptions list
DONE        1.5  Send empty subscriptions list
DONE        1.6  Update subscription
DONE        1.7  Remove item by update subscription
DONE        1.8  Create subscription with gateway
DONE        1.9  Update subscription with gateway
DONE        1.10 Delete subscription with gateway

*/

namespace HubPlatform {
namespace UT {

using namespace CloudDevices;

class SubscriptionManagerTestSuite : public FakeCloudDevicesService {
protected:
    Json createGateway( const std::string& gatewayName ) {
        return JSON(
            "createAbstractSubscriptionCommand"
            << ( "HUB:" + gatewayName + "/scripts/create_abstract_subscription" ) << "updateAbstractSubscriptionCommand"
            << ( "HUB:" + gatewayName + "/scripts/update_abstract_subscription" ) << "deleteAbstractSubscriptionCommand"
            << ( "HUB:" + gatewayName + "/scripts/delete_abstract_subscription" ) );
    }
};

TEST_F( SubscriptionManagerTestSuite, CreateSubscriptions ) /* 1.1 */ {
    using namespace Resources;

    auto& dbClient = getHostService().getDBClient();
    FakeCoreDRemoteInterface core( getHostService() );
    SubscriptionHelper helper( getHostService(), core );

    SubscriptionManager manager( helper, dbClient );

    auto request1 = Json::parse( R"({
    "abstractUUID": "uuid_1",
    "mappings": [{
        "capability": "capability_1",
        "variable": "variable_1",
        "valueType": "string",
        "dictionaryType": false,
        "value": "value_1"
    }]
    })" );

    auto request2 = Json::parse( R"({
    "abstractUUID": "uuid_2",
    "mappings": [{
        "capability": "capability_2",
        "variable": "variable_2",
        "valueType": "bool",
        "dictionaryType": false,
        "value": true
    },
    {
        "capability": "capability_3",
        "variable": "variable_3",
        "valueType": "int",
        "dictionaryType": true,
        "value": { "key": 3 }
    }]
    })" );

    EXPECT_FALSE( hasError( manager.addSubscription( request1 ) ) );
    EXPECT_FALSE( hasError( manager.addSubscription( request2 ) ) );

    ASSERT_EQ( core.m_devices.size(), 2 );
    ASSERT_EQ( core.m_items.size(), 3 );

    auto table = dbClient.findAll( DB::TableNames::Subscriptions, JSON_EMPTY_OBJ, nullptr );
    ASSERT_EQ( table.size(), 2 );

    for( auto& subscription : table ) {
        auto deviceId = json_get_str( subscription, DB::SubscriptionFields::DeviceId );
        EXPECT_TRUE( core.m_devices.count( deviceId ) ) << "Device Id should be present: " << deviceId;

        auto& mappings = subscription.at( DB::SubscriptionFields::Mappings );
        for( auto& mapping : mappings ) {
            auto itemId = json_get_str( mapping, DB::SubscriptionFields::ItemId );
            EXPECT_TRUE( core.m_items.count( itemId ) ) << "Item Id should be present: " << itemId;
        }
    }

    for( auto& subscription : table ) {
        subscription.at( DB::SubscriptionFields::DeviceId ) = "deviceId";
        auto& mappings = subscription.at( DB::SubscriptionFields::Mappings );
        for( auto& mapping : mappings )
            mapping.at( DB::SubscriptionFields::ItemId ) = "itemId";
    }

    auto expectedSubscription1 = Json::parse( R"({
        "_id": "uuid_1",
        "deviceId": "deviceId",
        "mappings": [{
            "capability": "capability_1",
            "variable": "variable_1",
            "itemId": "itemId"
        }]
    })" );

    auto& subscription1 = table.front();
    EXPECT_EQ( subscription1.dump(), expectedSubscription1.dump() );

    auto expectedSubscription2 = Json::parse( R"({
        "_id": "uuid_2",
        "deviceId": "deviceId",
        "mappings": [{
            "capability": "capability_2",
            "variable": "variable_2",
            "itemId": "itemId"
        },
        {
            "capability": "capability_3",
            "variable": "variable_3",
            "itemId": "itemId"
        }]
    })" );

    auto& subscription2 = table.back();
    EXPECT_EQ( subscription2.dump(), expectedSubscription2.dump() );
};

TEST_F( SubscriptionManagerTestSuite, DeleteSubscription ) /* 1.2 */ {
    using namespace Resources;

    auto& dbClient = getHostService().getDBClient();
    FakeCoreDRemoteInterface core( getHostService() );
    SubscriptionHelper helper( getHostService(), core );

    SubscriptionManager manager( helper, dbClient );

    auto request1 = Json::parse( R"({
    "abstractUUID": "uuid_1",
    "mappings": [{
        "capability": "capability_1",
        "variable": "variable_1",
        "valueType": "string",
        "dictionaryType": false,
        "value": "value_1"
    }]
    })" );

    auto request2 = Json::parse( R"({
    "abstractUUID": "uuid_2",
    "mappings": [{
        "capability": "capability_2",
        "variable": "variable_2",
        "valueType": "string",
        "dictionaryType": false,
        "value": "value_2"
    }]
    })" );

    EXPECT_FALSE( hasError( manager.addSubscription( request1 ) ) );
    EXPECT_FALSE( hasError( manager.addSubscription( request2 ) ) );

    ASSERT_EQ( core.m_devices.size(), 2 );
    ASSERT_EQ( core.m_items.size(), 2 );

    EXPECT_FALSE( hasError( manager.deleteSubscription( JSON( RPCParams::AbstractUUID << "uuid_2" ) ) ) );

    ASSERT_EQ( core.m_devices.size(), 1 );
    ASSERT_EQ( core.m_items.size(), 1 );

    auto table = dbClient.findAll( DB::TableNames::Subscriptions, JSON_EMPTY_OBJ, nullptr );
    ASSERT_EQ( table.size(), 1 );

    auto uuid = json_get_str( table.front(), DB::SubscriptionFields::Id );
    EXPECT_EQ( uuid, "uuid_1" );
};

TEST_F( SubscriptionManagerTestSuite, DeleteUnknownSubscription ) /* 1.3 */ {
    using namespace Resources;

    auto& dbClient = getHostService().getDBClient();
    FakeCoreDRemoteInterface core( getHostService() );
    SubscriptionHelper helper( getHostService(), core );

    SubscriptionManager manager( helper, dbClient );
    EXPECT_TRUE( hasError( manager.deleteSubscription( JSON( RPCParams::AbstractUUID << "unknownUUID" ) ) ) );
};

TEST_F( SubscriptionManagerTestSuite, SendSubscriptionsListToCloud ) /* 1.4 */ {
    using namespace Resources;

    auto& dbClient = getHostService().getDBClient();
    FakeCoreDRemoteInterface core( getHostService() );
    SubscriptionHelper helper( getHostService(), core );
    SubscriptionManager manager( helper, dbClient );

    auto request1 = Json::parse( R"({
    "abstractUUID": "uuid_1",
    "mappings": [{
        "capability": "capability_1",
        "variable": "variable_1",
        "valueType": "string",
        "dictionaryType": false,
        "value": "value_1"
    }]
    })" );

    auto request2 = Json::parse( R"({
    "abstractUUID": "uuid_2",
    "mappings": [{
        "capability": "capability_2",
        "variable": "variable_2",
        "valueType": "bool",
        "dictionaryType": false,
        "value": true
    },
    {
        "capability": "capability_3",
        "variable": "variable_3",
        "valueType": "int",
        "dictionaryType": false,
        "value": 3
    },
    {
        "capability": "capability_4",
        "variable": "variable_4",
        "valueType": "int",
        "dictionaryType": true,
        "value": { "key": 4 }
    }]
    })" );

    EXPECT_FALSE( hasError( manager.addSubscription( request1 ) ) );
    EXPECT_FALSE( hasError( manager.addSubscription( request2 ) ) );

    manager.sendSubscriptionList();
    auto message = getFakeResponseBus().getMessage();

    EXPECT_EQ( message.group, GroupsNames::UI_GROUP );
    EXPECT_EQ( message.msgSubClass, CLOUD_MESSAGE );

    auto& data = message.data;
    ASSERT_TRUE( !data.empty() );

    auto& requestBody = data.at( "requestBody" );
    ASSERT_TRUE( !requestBody.empty() );

    EXPECT_EQ( json_get_int( requestBody, API::General::Api ), 1 );
    EXPECT_EQ( json_get_str( requestBody, API::General::Method ), CloudMethods::CloudControllerSubscribeAbstracts );

    auto& params = requestBody.at( API::General::Params );
    auto& abstracts = params.at( CloudRPCParams::Abstracts );
    EXPECT_EQ( abstracts.size(), 2 );

    for( auto& abstract : abstracts ) {
        auto deviceId = json_get_str( abstract, CloudRPCParams::DeviceId );
        EXPECT_TRUE( core.m_devices.count( deviceId ) ) << "Device id should be presend: " << deviceId;
        abstract.at( CloudRPCParams::DeviceId ) = "device_id";

        auto& mappings = abstract.at( CloudRPCParams::Mappings );
        for( auto& mapping : mappings ) {
            auto itemId = json_get_str( mapping, CloudRPCParams::ItemId );
            EXPECT_TRUE( core.m_items.count( itemId ) ) << "Item Id should be present: " << itemId;
            mapping.at( CloudRPCParams::ItemId ) = "item_id";
        }
    }

    auto expectedParams = Json::parse( R"({
        "abstracts": [{
            "abstract_uuid": "uuid_1",
            "device_id": "device_id",
            "mappings": [{
                "capability": "capability_1",
                "item_id": "item_id",
                "value": "value_1",
                "variable": "variable_1"
            }]
        },
        {
            "abstract_uuid": "uuid_2",
            "device_id": "device_id",
            "mappings": [{
                "capability": "capability_2",
                "item_id": "item_id",
                "value": 1,
                "variable": "variable_2"
            },
            {
                "capability": "capability_3",
                "item_id": "item_id",
                "value": 3,
                "variable": "variable_3"
            },
            {
                "capability": "capability_4",
                "item_id": "item_id",
                "value": "{\"key\":4}",
                "variable": "variable_4"
            }]
        }]
    })" );

    EXPECT_EQ( params.dump(), expectedParams.dump() );
};

TEST_F( SubscriptionManagerTestSuite, SendEmptySubscriptionsListToCloud ) /* 1.5 */ {
    using namespace Resources;

    auto& dbClient = getHostService().getDBClient();
    FakeCoreDRemoteInterface core( getHostService() );
    SubscriptionHelper helper( getHostService(), core );
    SubscriptionManager manager( helper, dbClient );

    manager.sendSubscriptionList();
    auto message = getFakeResponseBus().getMessage();

    auto& data = message.data;
    auto& requestBody = data.at( "requestBody" );
    auto& params = requestBody.at( API::General::Params );

    auto expectedParams = JSON( CloudRPCParams::Abstracts << JSON_EMPTY_ARR );

    EXPECT_EQ( params.dump(), expectedParams.dump() );
};

TEST_F( SubscriptionManagerTestSuite, UpdateSubscriptions ) { /* 1.6 */
    using namespace Resources;
    auto& dbClient = getHostService().getDBClient();
    FakeCoreDRemoteInterface core( getHostService() );
    SubscriptionHelper helper( getHostService(), core );

    SubscriptionManager manager( helper, dbClient );

    auto request1 = Json::parse( R"({
            "abstractUUID": "uuid_1",
            "mappings": [{
                "capability": "capability_1",
                "variable": "variable_1",
                "valueType": "int",
                "dictionaryType": false,
                "value": 10,
                "itemId": "itemId"
            }]
    })" );

    EXPECT_FALSE( hasError( manager.addSubscription( request1 ) ) );
    ASSERT_EQ( core.m_devices.size(), 1 );
    ASSERT_EQ( core.m_items.size(), 1 );

    auto table = dbClient.findAll( DB::TableNames::Subscriptions, JSON_EMPTY_OBJ, nullptr );
    ASSERT_EQ( table.size(), 1 );

    auto request2 = Json::parse( R"({
            "abstractUUID": "uuid_1",
            "mappings": [{
                "capability": "capability_1",
                "variable": "variable_1",
                "valueType": "int",
                "dictionaryType": false,
                "value": 20,
                "itemId": "itemId"
            }]
    })" );
    EXPECT_FALSE( hasError( manager.updateSubscription( request2 ) ) );

    for( auto item : core.m_items ) {
        auto itemInfo = item.second;
        EXPECT_TRUE( json_get_str( itemInfo, "name" ) == "capability_1.variable_1" );
        EXPECT_TRUE( json_get_int( itemInfo, "value" ) == 20 );
    }

    auto request3 = Json::parse( R"({
            "abstractUUID": "uuid_1",
            "mappings": [{
                "capability": "capability_1",
                "variable": "variable_2",
                "valueType": "int",
                "dictionaryType": false,
                "value": 20,
                "maxValue": 30,
                "minValue": 10,
                "itemId": "itemId",
                "enum": ["one", "two"]
            }]
    })" );
    EXPECT_FALSE( hasError( manager.updateSubscription( request3 ) ) );
    ASSERT_EQ( core.m_devices.size(), 1 );
    ASSERT_EQ( core.m_items.size(), 2 );

    for( auto item : core.m_items ) {
        auto itemInfo = item.second;
        std::string name = json_get_str( itemInfo, "name" );
        if( name == "capability_1.variable_2" ) {
            EXPECT_TRUE( json_get_int( itemInfo, "valueMax" ) == 30 );
            EXPECT_TRUE( json_get_int( itemInfo, "value" ) == 20 );
            EXPECT_TRUE( json_get_int( itemInfo, "valueMin" ) == 10 );
            JsonArray arrEnum;
            json_get_arr( itemInfo, "enum", arrEnum );
            EXPECT_EQ( arrEnum.size(), 2 );
        }
    }
}

TEST_F( SubscriptionManagerTestSuite, RemoveItemByUpdateSubscriptions ) { /* 1.7 */
    using namespace Resources;

    auto& dbClient = getHostService().getDBClient();
    FakeCoreDRemoteInterface core( getHostService() );
    SubscriptionHelper helper( getHostService(), core );

    SubscriptionManager manager( helper, dbClient );

    auto request1 = Json::parse( R"({
            "abstractUUID": "uuid_1",
            "mappings": [{
                "capability": "capability_1",
                "variable": "variable_1",
                "valueType": "int",
                "dictionaryType": false,
                "value": 10,
                "itemId": "itemId"
            },
            {
                "capability": "capability_1",
                "variable": "variable_2",
                "valueType": "int",
                "dictionaryType": false,
                "value": 10,
                "itemId": "itemId1"
            }]
    })" );

    EXPECT_FALSE( hasError( manager.addSubscription( request1 ) ) );

    ASSERT_EQ( core.m_devices.size(), 1 );
    ASSERT_EQ( core.m_items.size(), 2 );

    auto table = dbClient.findAll( DB::TableNames::Subscriptions, JSON_EMPTY_OBJ, nullptr );
    ASSERT_EQ( table.size(), 1 );

    auto request2 = Json::parse( R"({
            "abstractUUID": "uuid_1",
            "mappings": [{
                "capability": "capability_1",
                "variable": "variable_1"
            }]
    })" );
    EXPECT_FALSE( hasError( manager.updateSubscription( request2 ) ) );

    ASSERT_EQ( core.m_devices.size(), 1 );
    ASSERT_EQ( core.m_items.size(), 1 );

    auto request3 = Json::parse( R"({
            "abstractUUID": "uuid_1",
            "mappings": [{
                "capability": "capability_1",
                "variable": "variable_2"
            }]
    })" );
    EXPECT_FALSE( hasError( manager.updateSubscription( request3 ) ) );
    // ASSERT_EQ( core.m_devices.size(), 0 ); // TODO check if subscription should be removed if no items left
    ASSERT_EQ( core.m_items.size(), 0 );
};

TEST_F( SubscriptionManagerTestSuite, CreateSubscriptionsWithGateway ) /* 1.8 */ {
    using namespace Resources;

    auto& dbClient = getHostService().getDBClient();
    FakeCoreDRemoteInterface core( getHostService() );
    SubscriptionHelperMock helper( getHostService(), core );
    SubscriptionManager manager( helper, dbClient );

    auto request1 = Json::parse( R"({
    "gatewayName": "gateway1",
    "abstractUUID": "uuid_1",
    "mappings": [{
        "capability": "capability_1",
        "variable": "variable_1",
        "valueType": "string",
        "dictionaryType": false,
        "value": "value_1"
    }]
    })" );

    EXPECT_CALL( helper, getGateway( testing::_ ) ).WillOnce( testing::Return( createGateway( "gateway1" ) ) );
    EXPECT_CALL( helper, sendExecutePluginScript( "HUB:gateway1/scripts/create_abstract_subscription", testing::_ ) )
        .Times( 1 );
    EXPECT_FALSE( hasError( manager.addSubscription( request1 ) ) );
};

TEST_F( SubscriptionManagerTestSuite, UpdateSubscriptionsWithGateway ) /* 1.8 */ {
    using namespace Resources;

    auto& dbClient = getHostService().getDBClient();
    FakeCoreDRemoteInterface core( getHostService() );
    SubscriptionHelperMock helper( getHostService(), core );
    SubscriptionManager manager( helper, dbClient );

    auto subscriptionRecord = Json::parse( R"({
    "_id": "uuid_1",
    "deviceId": "abcd",
    "mappings": [
        {
            "capability": "capability_1",
            "variable": "variable_1",
            "itemId": "itemId_1"
        }
    ]})" );

    SubscriptionUtils::storeSubscriptionToDb( dbClient, subscriptionRecord );

    auto request1 = Json::parse( R"({
    "gatewayName": "gateway1",
    "abstractUUID": "uuid_1",
    "mappings": [{
        "capability": "capability_1",
        "variable": "variable_1",
        "valueType": "string",
        "dictionaryType": false,
        "value": "value_1"
    }]
    })" );

    EXPECT_CALL( helper, getGateway( testing::_ ) ).WillOnce( testing::Return( createGateway( "gateway1" ) ) );
    EXPECT_CALL( helper, sendExecutePluginScript( "HUB:gateway1/scripts/update_abstract_subscription", testing::_ ) )
        .Times( 1 );
    EXPECT_FALSE( hasError( manager.updateSubscription( request1 ) ) );
};

TEST_F( SubscriptionManagerTestSuite, DeleteSubscriptionsWithGateway ) /* 1.8 */ {
    using namespace Resources;

    auto& dbClient = getHostService().getDBClient();
    FakeCoreDRemoteInterface core( getHostService() );
    SubscriptionHelperMock helper( getHostService(), core );
    SubscriptionManager manager( helper, dbClient );

    auto subscriptionRecord = Json::parse( R"({
    "_id": "uuid_1",
    "deviceId": "abcd",
    "mappings": [
        {
            "capability": "capability_1",
            "variable": "variable_1",
            "itemId": "itemId_1"
        }
    ]})" );

    SubscriptionUtils::storeSubscriptionToDb( dbClient, subscriptionRecord );

    auto request1 = Json::parse( R"({
    "gatewayName": "gateway1",
    "abstractUUID": "uuid_1"
    })" );

    EXPECT_CALL( helper, getGateway( testing::_ ) ).WillOnce( testing::Return( createGateway( "gateway1" ) ) );
    EXPECT_CALL( helper, sendExecutePluginScript( "HUB:gateway1/scripts/delete_abstract_subscription", testing::_ ) )
        .Times( 1 );
    EXPECT_FALSE( hasError( manager.deleteSubscription( request1 ) ) );
};

}  // namespace UT
}  // namespace HubPlatform
