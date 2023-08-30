#include "fake_owner_fixture.h"
#include "MainFixture.h"

#include <gtest/gtest.h>
#include <gmock/gmock.h>

namespace znet {

namespace ut {

using ::testing::_;
using ::testing::ElementsAre;

class ZNetHandlerNodeCheckTestSuite : public MainFixture, public FakeOwnerFixture {};

TEST_F( ZNetHandlerNodeCheckTestSuite, checkAllNodesValues ) {
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

    void* callbackArg = &getImpl();
    getImpl().setState( IZNet::Mode::Initializing, ModeStatus::Started );
    getImpl().activateMode( IZNet::Mode::Monitoring );
    std::vector<std::pair<uint8_t, uint8_t> > dataToCheck = { { 0, ZNET_COMMAND_CLASS_SWITCH_BINARY },
                                                              { 0, ZNET_COMMAND_CLASS_SWITCH_MULTILEVEL },
                                                              { 0, ZNET_COMMAND_CLASS_BATTERY },
                                                              { 1, ZNET_COMMAND_CLASS_SWITCH_BINARY },
                                                              { 2, ZNET_COMMAND_CLASS_SWITCH_MULTILEVEL } };
    getImpl().checkNodeValues( 1, std::chrono::milliseconds{ 30 }, dataToCheck );
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

    EXPECT_FALSE( requestsManager.isWaitingForRespond( BinarySwitchValueRequest( 1, 0 ) ) );
    EXPECT_FALSE( requestsManager.isWaitingForRespond( MultilevelSwitchValueRequest( 1, 0 ) ) );
    EXPECT_FALSE( requestsManager.isWaitingForRespond( BatteryLevelRequest( 1, 0 ) ) );
    EXPECT_FALSE( requestsManager.isWaitingForRespond( BinarySwitchValueRequest( 1, 1 ) ) );
    EXPECT_FALSE( requestsManager.isWaitingForRespond( MultilevelSwitchValueRequest( 1, 2 ) ) );
}

TEST_F( ZNetHandlerNodeCheckTestSuite, requestFiltered ) {
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

    void* callbackArg = &getImpl();
    getImpl().setState( IZNet::Mode::Initializing, ModeStatus::Started );
    getImpl().activateMode( IZNet::Mode::Monitoring );
    std::vector<std::pair<uint8_t, uint8_t> > dataToCheck = {
        { 0, ZNET_COMMAND_CLASS_SWITCH_BINARY },
        { 1, ZNET_COMMAND_CLASS_SWITCH_BINARY },
    };
    getImpl().checkNodeValues( 1, std::chrono::milliseconds{ 30 }, dataToCheck );
    getImpl().onLoop();

    const auto& callbacks = getImpl().getCallbacks();
    const auto& requestsManager = getImpl().getRequestsManager();

    // node 1, channel 0
    {
        EXPECT_TRUE( requestsManager.isWaitingForRespond( BinarySwitchValueRequest( 1, 0 ) ) );
        EXPECT_FALSE( requestsManager.isWaitingForRespond( BinarySwitchValueRequest( 1, 1 ) ) );
        callbacks.node_cmd_binary_switch_result( ZNET_ERR_OK, 1, 0, 1, 0xFE, 0, callbackArg );
    }

    // node 1, channels 1, 2
    {
        EXPECT_TRUE( requestsManager.isWaitingForRespond( BinarySwitchValueRequest( 1, 1 ) ) );
        callbacks.node_cmd_binary_switch_result( ZNET_ERR_OK, 1, 1, 1, 0xFE, 0, callbackArg );
    }

    EXPECT_FALSE( requestsManager.isWaitingForRespond( BinarySwitchValueRequest( 1, 0 ) ) );
    EXPECT_FALSE( requestsManager.isWaitingForRespond( MultilevelSwitchValueRequest( 1, 0 ) ) );
    EXPECT_FALSE( requestsManager.isWaitingForRespond( BatteryLevelRequest( 1, 0 ) ) );
    EXPECT_FALSE( requestsManager.isWaitingForRespond( BinarySwitchValueRequest( 1, 1 ) ) );
    EXPECT_FALSE( requestsManager.isWaitingForRespond( MultilevelSwitchValueRequest( 1, 2 ) ) );
}

TEST_F( ZNetHandlerNodeCheckTestSuite, nodeCheck_withFailes ) {
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

    void* callbackArg = &getImpl();
    getImpl().setState( IZNet::Mode::Initializing, ModeStatus::Started );
    getImpl().activateMode( IZNet::Mode::Monitoring );
    std::vector<std::pair<uint8_t, uint8_t> > dataToCheck = { { 0, ZNET_COMMAND_CLASS_SWITCH_BINARY },
                                                              { 0, ZNET_COMMAND_CLASS_SWITCH_MULTILEVEL },
                                                              { 0, ZNET_COMMAND_CLASS_BATTERY },
                                                              { 1, ZNET_COMMAND_CLASS_SWITCH_BINARY },
                                                              { 2, ZNET_COMMAND_CLASS_SWITCH_MULTILEVEL } };
    getImpl().checkNodeValues( 1, std::chrono::milliseconds{ 30 }, dataToCheck );
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

    EXPECT_FALSE( requestsManager.isWaitingForRespond( BinarySwitchValueRequest( 1, 0 ) ) );
    EXPECT_FALSE( requestsManager.isWaitingForRespond( MultilevelSwitchValueRequest( 1, 0 ) ) );
    EXPECT_FALSE( requestsManager.isWaitingForRespond( BatteryLevelRequest( 1, 0 ) ) );
    EXPECT_FALSE( requestsManager.isWaitingForRespond( BinarySwitchValueRequest( 1, 1 ) ) );
    EXPECT_FALSE( requestsManager.isWaitingForRespond( MultilevelSwitchValueRequest( 1, 2 ) ) );
}

}  // namespace ut

}  // namespace znet
