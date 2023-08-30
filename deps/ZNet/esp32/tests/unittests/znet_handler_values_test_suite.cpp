#include "fake_owner_fixture.h"
#include "MainFixture.h"
#include <znet_handler_values_extended.hpp>

#include <gtest/gtest.h>
#include <gmock/gmock.h>

namespace znet {

namespace ut {

using ::testing::_;
using ::testing::ElementsAre;

class ZNetHandlerValuesTestSuite : public MainFixture, public FakeOwnerFixture {};

TEST_F( ZNetHandlerValuesTestSuite, requestAllNodesValues ) {
    getOwner().nodeCreate( 1 );
    auto node1 = getOwner().onFindNode( 1 );
    node1->createAdditionalChannels( 2 );
    node1->addCommandClass( 0, ZNET_COMMAND_CLASS_BATTERY );
    node1->addCommandClass( 0, ZNET_COMMAND_CLASS_SWITCH_MULTILEVEL );
    node1->addCommandClass( 0, ZNET_COMMAND_CLASS_SWITCH_BINARY );
    node1->addCommandClass( 0, ZNET_COMMAND_CLASS_MULTI_CHANNEL_ASSOCIATION );  // not askable
    node1->addCommandClass( 0, ZNET_COMMAND_CLASS_ASSOCIATION );                // not askable
    node1->addCommandClass( 1, ZNET_COMMAND_CLASS_SWITCH_BINARY );
    node1->addCommandClass( 2, ZNET_COMMAND_CLASS_SWITCH_MULTILEVEL );

    getOwner().nodeCreate( 3 );
    auto node3 = getOwner().onFindNode( 3 );
    node3->addCommandClass( 0, ZNET_COMMAND_CLASS_SWITCH_MULTILEVEL );
    node3->addCommandClass( 0, ZNET_COMMAND_CLASS_SWITCH_BINARY );

    getOwner().nodeCreate( 4 );  // empty node

    getOwner().nodeCreate( 5 );
    auto node5 = getOwner().onFindNode( 5 );
    node5->addCommandClass( 0, ZNET_COMMAND_CLASS_BATTERY );

    void* callbackArg = &getImpl();
    getImpl().setState( IZNet::Mode::Initializing, ModeStatus::Started );
    getImpl().activateMode( IZNet::Mode::Monitoring );
    getImpl().requestAllNodesValues();
    getImpl().onLoop();

    const auto& callbacks = getImpl().getCallbacks();
    const auto& requestsManager = getImpl().getRequestsManager();

    // node 1, channel 0
    {
        EXPECT_TRUE( requestsManager.isWaitingForRespond( BinarySwitchValueRequest( 1, 0 ) ) );
        EXPECT_FALSE( requestsManager.isWaitingForRespond( MultilevelSwitchValueRequest( 1, 0 ) ) );
        callbacks.node_cmd_binary_switch_result( ZNET_ERR_OK, 1, 0, 1, 0xFE, 0, callbackArg );

        EXPECT_TRUE( requestsManager.isWaitingForRespond( MultilevelSwitchValueRequest( 1, 0 ) ) );
        EXPECT_FALSE( requestsManager.isWaitingForRespond( BatteryLevelRequest( 1, 0 ) ) );
        callbacks.node_cmd_multilevel_switch_result( ZNET_ERR_OK, 1, 0, 1, 1, 0, callbackArg );

        EXPECT_TRUE( requestsManager.isWaitingForRespond( BatteryLevelRequest( 1, 0 ) ) );
        EXPECT_FALSE( requestsManager.isWaitingForRespond( BinarySwitchValueRequest( 1, 1 ) ) );
        callbacks.node_cmd_battery_level_result( ZNET_ERR_OK, 1, 0, 0, callbackArg );
    }

    // node 1, channels 1, 2
    {
        EXPECT_TRUE( requestsManager.isWaitingForRespond( BinarySwitchValueRequest( 1, 1 ) ) );
        EXPECT_FALSE( requestsManager.isWaitingForRespond( MultilevelSwitchValueRequest( 1, 2 ) ) );
        callbacks.node_cmd_binary_switch_result( ZNET_ERR_OK, 1, 1, 1, 0xFE, 0, callbackArg );

        EXPECT_TRUE( requestsManager.isWaitingForRespond( MultilevelSwitchValueRequest( 1, 2 ) ) );
        EXPECT_FALSE( requestsManager.isWaitingForRespond( BinarySwitchValueRequest( 3, 0 ) ) );
        callbacks.node_cmd_multilevel_switch_result( ZNET_ERR_OK, 1, 2, 1, 1, 0, callbackArg );
    }

    // node 3
    {
        EXPECT_TRUE( requestsManager.isWaitingForRespond( BinarySwitchValueRequest( 3, 0 ) ) );
        EXPECT_FALSE( requestsManager.isWaitingForRespond( MultilevelSwitchValueRequest( 3, 0 ) ) );
        callbacks.node_cmd_binary_switch_result( ZNET_ERR_OK, 3, 0, 1, 0xFE, 0, callbackArg );

        EXPECT_TRUE( requestsManager.isWaitingForRespond( MultilevelSwitchValueRequest( 3, 0 ) ) );
        EXPECT_FALSE( requestsManager.isWaitingForRespond( BatteryLevelRequest( 5, 0 ) ) );
        callbacks.node_cmd_multilevel_switch_result( ZNET_ERR_OK, 3, 0, 1, 1, 0, callbackArg );
    }

    // node 5
    {
        EXPECT_TRUE( requestsManager.isWaitingForRespond( BatteryLevelRequest( 5, 0 ) ) );
        callbacks.node_cmd_battery_level_result( ZNET_ERR_OK, 5, 0, 0, callbackArg );
    }

    EXPECT_FALSE( requestsManager.isWaitingForRespond( BinarySwitchValueRequest( 1, 0 ) ) );
    EXPECT_FALSE( requestsManager.isWaitingForRespond( MultilevelSwitchValueRequest( 1, 0 ) ) );
    EXPECT_FALSE( requestsManager.isWaitingForRespond( BatteryLevelRequest( 1, 0 ) ) );
    EXPECT_FALSE( requestsManager.isWaitingForRespond( BinarySwitchValueRequest( 1, 1 ) ) );
    EXPECT_FALSE( requestsManager.isWaitingForRespond( MultilevelSwitchValueRequest( 1, 2 ) ) );
    EXPECT_FALSE( requestsManager.isWaitingForRespond( BinarySwitchValueRequest( 3, 0 ) ) );
    EXPECT_FALSE( requestsManager.isWaitingForRespond( MultilevelSwitchValueRequest( 3, 0 ) ) );
    EXPECT_FALSE( requestsManager.isWaitingForRespond( BatteryLevelRequest( 5, 0 ) ) );
}

TEST_F( ZNetHandlerValuesTestSuite, requestAllNodesValues_withFailes ) {
    getOwner().nodeCreate( 1 );
    auto node1 = getOwner().onFindNode( 1 );
    node1->createAdditionalChannels( 2 );
    node1->addCommandClass( 0, ZNET_COMMAND_CLASS_BATTERY );
    node1->addCommandClass( 0, ZNET_COMMAND_CLASS_SWITCH_MULTILEVEL );
    node1->addCommandClass( 0, ZNET_COMMAND_CLASS_SWITCH_BINARY );
    node1->addCommandClass( 0, ZNET_COMMAND_CLASS_MULTI_CHANNEL_ASSOCIATION );  // not askable
    node1->addCommandClass( 0, ZNET_COMMAND_CLASS_ASSOCIATION );                // not askable
    node1->addCommandClass( 1, ZNET_COMMAND_CLASS_SWITCH_BINARY );
    node1->addCommandClass( 2, ZNET_COMMAND_CLASS_SWITCH_MULTILEVEL );

    getOwner().nodeCreate( 3 );
    auto node3 = getOwner().onFindNode( 3 );
    node3->addCommandClass( 0, ZNET_COMMAND_CLASS_SWITCH_MULTILEVEL );
    node3->addCommandClass( 0, ZNET_COMMAND_CLASS_SWITCH_BINARY );

    getOwner().nodeCreate( 4 );  // empty node

    getOwner().nodeCreate( 5 );
    auto node5 = getOwner().onFindNode( 5 );
    node5->addCommandClass( 0, ZNET_COMMAND_CLASS_BATTERY );

    void* callbackArg = &getImpl();
    getImpl().setState( IZNet::Mode::Initializing, ModeStatus::Started );
    getImpl().activateMode( IZNet::Mode::Monitoring );
    getImpl().requestAllNodesValues();
    getImpl().onLoop();

    const auto& callbacks = getImpl().getCallbacks();
    const auto& requestsManager = getImpl().getRequestsManager();

    // node 1, channel 0
    {
        EXPECT_TRUE( requestsManager.isWaitingForRespond( BinarySwitchValueRequest( 1, 0 ) ) );
        EXPECT_FALSE( requestsManager.isWaitingForRespond( MultilevelSwitchValueRequest( 1, 0 ) ) );
        callbacks.node_cmd_binary_switch_result( ZNET_ERR_OK, 1, 0, 1, 0xFE, 0, callbackArg );

        for( size_t i = 0; i <= MaxFailNumber; ++i ) {
            EXPECT_TRUE( requestsManager.isWaitingForRespond( MultilevelSwitchValueRequest( 1, 0 ) ) );
            EXPECT_FALSE( requestsManager.isWaitingForRespond( BatteryLevelRequest( 1, 0 ) ) );
            callbacks.node_cmd_multilevel_switch_result( ZNET_ERR_NETWORK_BUSY, 1, 0, 1, 1, 0, callbackArg );
        }

        EXPECT_FALSE( requestsManager.isWaitingForRespond( MultilevelSwitchValueRequest( 1, 0 ) ) );
    }

    // node 3
    {
        EXPECT_TRUE( requestsManager.isWaitingForRespond( BinarySwitchValueRequest( 3, 0 ) ) );
        EXPECT_FALSE( requestsManager.isWaitingForRespond( MultilevelSwitchValueRequest( 3, 0 ) ) );

        znet_deinit();
        callbacks.node_cmd_binary_switch_result( ZNET_ERR_OK, 3, 0, 1, 0xFE, 0, callbackArg );
    }

    EXPECT_FALSE( requestsManager.isWaitingForRespond( BinarySwitchValueRequest( 1, 0 ) ) );
    EXPECT_FALSE( requestsManager.isWaitingForRespond( MultilevelSwitchValueRequest( 1, 0 ) ) );
    EXPECT_FALSE( requestsManager.isWaitingForRespond( BatteryLevelRequest( 1, 0 ) ) );
    EXPECT_FALSE( requestsManager.isWaitingForRespond( BinarySwitchValueRequest( 1, 1 ) ) );
    EXPECT_FALSE( requestsManager.isWaitingForRespond( MultilevelSwitchValueRequest( 1, 2 ) ) );
    EXPECT_FALSE( requestsManager.isWaitingForRespond( BinarySwitchValueRequest( 3, 0 ) ) );
    EXPECT_FALSE( requestsManager.isWaitingForRespond( MultilevelSwitchValueRequest( 3, 0 ) ) );
    EXPECT_FALSE( requestsManager.isWaitingForRespond( BatteryLevelRequest( 5, 0 ) ) );
}

TEST_F( ZNetHandlerValuesTestSuite, requestsCcOrder ) {
    {
        CurrentValuesHandler handler( getImpl() );

        EXPECT_THAT( handler.getRequestsCcOrder(),
                     ElementsAre( ZNET_COMMAND_CLASS_ASSOCIATION,
                                  ZNET_COMMAND_CLASS_MULTI_CHANNEL_ASSOCIATION,
                                  ZNET_COMMAND_CLASS_CONFIGURATION,
                                  ZNET_COMMAND_CLASS_SENSOR_BINARY,
                                  ZNET_COMMAND_CLASS_SENSOR_ALARM,
                                  ZNET_COMMAND_CLASS_NOTIFICATION,
                                  ZNET_COMMAND_CLASS_SWITCH_BINARY,
                                  ZNET_COMMAND_CLASS_SENSOR_MULTILEVEL,
                                  ZNET_COMMAND_CLASS_SWITCH_MULTILEVEL,
                                  ZNET_COMMAND_CLASS_DOOR_LOCK,
                                  ZNET_COMMAND_CLASS_THERMOSTAT_MODE,
                                  ZNET_COMMAND_CLASS_THERMOSTAT_OPERATING_STATE,
                                  ZNET_COMMAND_CLASS_THERMOSTAT_FAN_MODE,
                                  ZNET_COMMAND_CLASS_THERMOSTAT_FAN_STATE,
                                  ZNET_COMMAND_CLASS_THERMOSTAT_SETBACK,
                                  ZNET_COMMAND_CLASS_THERMOSTAT_SETPOINT,
                                  ZNET_COMMAND_CLASS_BARRIER_OPERATOR,
                                  ZNET_COMMAND_CLASS_SOUND_SWITCH,
                                  ZNET_COMMAND_CLASS_SWITCH_COLOR,
                                  ZNET_COMMAND_CLASS_METER,
                                  ZNET_COMMAND_CLASS_BATTERY,
                                  ZNET_COMMAND_CLASS_WAKE_UP,
                                  ZNET_COMMAND_CLASS_BASIC ) );
    }

    {
        CurrentValuesExtendedHandler handler( getImpl() );

        EXPECT_THAT( handler.getRequestsCcOrder(),
                     ElementsAre( ZNET_COMMAND_CLASS_ASSOCIATION,
                                  ZNET_COMMAND_CLASS_MULTI_CHANNEL_ASSOCIATION,
                                  ZNET_COMMAND_CLASS_CONFIGURATION,
                                  ZNET_COMMAND_CLASS_SENSOR_BINARY,
                                  ZNET_COMMAND_CLASS_SENSOR_ALARM,
                                  ZNET_COMMAND_CLASS_NOTIFICATION,
                                  ZNET_COMMAND_CLASS_SWITCH_BINARY,
                                  ZNET_COMMAND_CLASS_SENSOR_MULTILEVEL,
                                  ZNET_COMMAND_CLASS_SWITCH_MULTILEVEL,
                                  ZNET_COMMAND_CLASS_DOOR_LOCK,
                                  ZNET_COMMAND_CLASS_THERMOSTAT_MODE,
                                  ZNET_COMMAND_CLASS_THERMOSTAT_OPERATING_STATE,
                                  ZNET_COMMAND_CLASS_THERMOSTAT_FAN_MODE,
                                  ZNET_COMMAND_CLASS_THERMOSTAT_FAN_STATE,
                                  ZNET_COMMAND_CLASS_THERMOSTAT_SETBACK,
                                  ZNET_COMMAND_CLASS_THERMOSTAT_SETPOINT,
                                  ZNET_COMMAND_CLASS_BARRIER_OPERATOR,
                                  ZNET_COMMAND_CLASS_SOUND_SWITCH,
                                  ZNET_COMMAND_CLASS_SWITCH_COLOR,
                                  ZNET_COMMAND_CLASS_METER,
                                  ZNET_COMMAND_CLASS_BATTERY,
                                  ZNET_COMMAND_CLASS_WAKE_UP,
                                  ZNET_COMMAND_CLASS_BASIC,
                                  ZNET_COMMAND_CLASS_USER_CODE,
                                  ZNET_COMMAND_CLASS_SCHEDULE_ENTRY_LOCK ) );
    }
}

}  // namespace ut

}  // namespace znet
