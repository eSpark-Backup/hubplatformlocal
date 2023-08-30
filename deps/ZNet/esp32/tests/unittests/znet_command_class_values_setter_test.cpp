#include <znet/znet_command_class_values_setter.hpp>
#include <znet/znet_utils.hpp>
#include <znet/znet_node.hpp>
#include <znet/cc_0x25_switch.hpp>
#include <znet/cc_0x26_switch_multi_level.hpp>
#include <znet/cc_0x66_barrier_operator.hpp>
#include <znet/cc_0x33_color_switch.hpp>

#include "znet_node_requests_managing_fixture.h"
#include "fake_file.h"

#include "znet_mock_command_class.h"

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <vector>

namespace znet {
namespace ut {

namespace {
struct Param {
    MulticastMode mode;
    uint8_t setValueCallCount;
    uint8_t requestValueCallCount;
    bool chipSinglecasts;
};
}  // namespace

using ::testing::_;
using ::testing::Invoke;
using ::testing::MockFunction;

class ZNetCommandClassValuesSetterTestSuite : public ::testing::Test,
                                              public ::testing::WithParamInterface<Param>,
                                              public NodeRequestsManagingFixture {
protected:
    std::unique_ptr<CommandClassValuesSetter> createCommandClassValuesSetter() {
        return std::unique_ptr<CommandClassValuesSetter>(
            new CommandClassValuesSetter( getNodesProvider(), getTimerProvider() ) );
    }

    template <typename CC>
    std::shared_ptr<MockCommandClass<CC> > createCommandClassWithNode(
        znet_node_id_t nodeId,
        SecurityMode::Class security = SecurityMode::Class::None ) {
        getNodesProvider().nodeCreate( nodeId );
        auto node = getNodesProvider().onFindNode( nodeId );
        node->setSecurity( security );
        auto cc = std::make_shared<MockCommandClass<CC> >( getNodeChannel( *node, 0 ) );
        getNodeChannel( *node, 0 ).addCommandClass( cc );
        return cc;
    }

    template <typename CC>
    std::vector<std::shared_ptr<MockCommandClass<CC> > > createChannelCommandClassesWithNode( znet_node_id_t nodeId,
                                                                                              uint8_t channelsCount ) {
        assert( channelsCount );

        getNodesProvider().nodeCreate( nodeId );
        auto node = getNodesProvider().onFindNode( nodeId );
        node->createAdditionalChannels( channelsCount - 1 );

        std::vector<std::shared_ptr<MockCommandClass<CC> > > result;
        result.reserve( channelsCount );

        for( uint8_t channelId = 0; channelId < channelsCount; ++channelId ) {
            auto cc = std::make_shared<MockCommandClass<CC> >( getNodeChannel( *node, channelId ) );
            getNodeChannel( *node, channelId ).addCommandClass( cc );
            result.push_back( cc );
        }

        return result;
    }
};

INSTANTIATE_TEST_SUITE_P( ZNetCommandClassValuesSetterTestSuite_param,
                          ZNetCommandClassValuesSetterTestSuite,
                          ::testing::Values( Param{ MulticastMode::SequentSinglecasts, 1, 1, false },
                                             Param{ MulticastMode::ChipSinglecasts, 0, 1, true },
                                             Param{ MulticastMode::NoValuesGetting, 0, 0, true } ) );

TEST_P( ZNetCommandClassValuesSetterTestSuite, switchMulticastOnce ) {
    auto valuesSetter = createCommandClassValuesSetter();

    auto cc1 = createCommandClassWithNode<ZCc0x25Switch>( 1 );
    auto cc2 = createCommandClassWithNode<ZCc0x25Switch>( 2 );
    auto cc3 = createCommandClassWithNode<ZCc0x25Switch>( 3 );
    auto cc4 = createCommandClassWithNode<ZCc0x25Switch>( 4 );

    const uint8_t nodeIds[] = { 1, 2, 4 };

    const uint8_t value = 0xFF;

    EXPECT_CALL( *cc1, setTokenMulticast ).Times( 0 );
    EXPECT_CALL( *cc1, setToken ).Times( 0 );
    EXPECT_CALL( *cc1, setRgbMulticast ).Times( 0 );
    EXPECT_CALL( *cc1, setRgb ).Times( 0 );

    // do multicast
    {
        EXPECT_CALL( *cc1, setValueMulticast( 3, _, value, GetParam().chipSinglecasts ) ).Times( 1 );
        EXPECT_CALL( *cc1, setValue( value ) ).Times( GetParam().setValueCallCount );
        EXPECT_CALL( *cc1, onValueRequested ).Times( GetParam().requestValueCallCount );

        EXPECT_CALL( *cc2, setValueMulticast ).Times( 0 );
        EXPECT_CALL( *cc2, setValue ).Times( 0 );
        EXPECT_CALL( *cc2, onValueRequested ).Times( 0 );

        EXPECT_CALL( *cc3, setValueMulticast ).Times( 0 );
        EXPECT_CALL( *cc3, setValue ).Times( 0 );
        EXPECT_CALL( *cc3, onValueRequested ).Times( 0 );

        EXPECT_CALL( *cc4, setValueMulticast ).Times( 0 );
        EXPECT_CALL( *cc4, setValue ).Times( 0 );
        EXPECT_CALL( *cc4, onValueRequested ).Times( 0 );

        valuesSetter->setValueMulticast( 3, nodeIds, 0, ZCc0x25Switch::ID::value, value, GetParam().mode );
    }

    // 1st node response
    {
        EXPECT_CALL( *cc1, setValueMulticast ).Times( 0 );
        EXPECT_CALL( *cc1, setValue ).Times( 0 );
        EXPECT_CALL( *cc1, onValueRequested ).Times( 0 );

        EXPECT_CALL( *cc2, setValueMulticast ).Times( 0 );
        EXPECT_CALL( *cc2, setValue( value ) ).Times( GetParam().setValueCallCount );
        EXPECT_CALL( *cc2, onValueRequested ).Times( GetParam().requestValueCallCount );

        EXPECT_CALL( *cc3, setValueMulticast ).Times( 0 );
        EXPECT_CALL( *cc3, setValue ).Times( 0 );
        EXPECT_CALL( *cc3, onValueRequested ).Times( 0 );

        EXPECT_CALL( *cc4, setValueMulticast ).Times( 0 );
        EXPECT_CALL( *cc4, setValue ).Times( 0 );
        EXPECT_CALL( *cc4, onValueRequested ).Times( 0 );

        getRequestsManager().onRequestSent( BinarySwitchValueRequest( 1, 0 ) );
        getRequestsManager().onRespond( BinarySwitchValueRequest( 1, 0 ), value, 0xFE, 0 );
    }

    // 2nd node response
    {
        EXPECT_CALL( *cc1, setValueMulticast ).Times( 0 );
        EXPECT_CALL( *cc1, setValue ).Times( 0 );
        EXPECT_CALL( *cc1, onValueRequested ).Times( 0 );

        EXPECT_CALL( *cc2, setValueMulticast ).Times( 0 );
        EXPECT_CALL( *cc2, setValue ).Times( 0 );
        EXPECT_CALL( *cc2, onValueRequested ).Times( 0 );

        EXPECT_CALL( *cc3, setValueMulticast ).Times( 0 );
        EXPECT_CALL( *cc3, setValue ).Times( 0 );
        EXPECT_CALL( *cc3, onValueRequested ).Times( 0 );

        EXPECT_CALL( *cc4, setValueMulticast ).Times( 0 );
        EXPECT_CALL( *cc4, setValue( value ) ).Times( GetParam().setValueCallCount );
        EXPECT_CALL( *cc4, onValueRequested ).Times( GetParam().requestValueCallCount );

        getRequestsManager().onRequestSent( BinarySwitchValueRequest( 2, 0 ) );
        getRequestsManager().onRespond( BinarySwitchValueRequest( 2, 0 ), value, 0xFE, 0 );
    }

    // 4th node response
    {
        EXPECT_CALL( *cc1, setValueMulticast ).Times( 0 );
        EXPECT_CALL( *cc1, setValue ).Times( 0 );
        EXPECT_CALL( *cc1, onValueRequested ).Times( 0 );

        EXPECT_CALL( *cc2, setValueMulticast ).Times( 0 );
        EXPECT_CALL( *cc2, setValue ).Times( 0 );
        EXPECT_CALL( *cc2, onValueRequested ).Times( 0 );

        EXPECT_CALL( *cc3, setValueMulticast ).Times( 0 );
        EXPECT_CALL( *cc3, setValue ).Times( 0 );
        EXPECT_CALL( *cc3, onValueRequested ).Times( 0 );

        EXPECT_CALL( *cc4, setValueMulticast ).Times( 0 );
        EXPECT_CALL( *cc4, setValue ).Times( 0 );
        EXPECT_CALL( *cc4, onValueRequested ).Times( 0 );

        getRequestsManager().onRequestSent( BinarySwitchValueRequest( 4, 0 ) );
        getRequestsManager().onRespond( BinarySwitchValueRequest( 4, 0 ), value, 0xFE, 0 );
    }
}

TEST_P( ZNetCommandClassValuesSetterTestSuite, switchMulticastAndSingleSetters ) {
    auto valuesSetter = createCommandClassValuesSetter();

    auto cc1 = createCommandClassWithNode<ZCc0x25Switch>( 1 );
    auto cc2 = createCommandClassWithNode<ZCc0x25Switch>( 2 );
    auto cc3 = createCommandClassWithNode<ZCc0x25Switch>( 3 );
    auto cc4 = createCommandClassWithNode<ZCc0x25Switch>( 4 );

    const uint8_t value = 0xFF;

    // set value for 1st node
    {
        EXPECT_CALL( *cc1, setValueMulticast ).Times( 0 );
        EXPECT_CALL( *cc1, setValue( value ) ).Times( 1 );
        EXPECT_CALL( *cc1, onValueRequested ).Times( 0 );

        EXPECT_CALL( *cc2, setValueMulticast ).Times( 0 );
        EXPECT_CALL( *cc2, setValue ).Times( 0 );
        EXPECT_CALL( *cc2, onValueRequested ).Times( 0 );

        EXPECT_CALL( *cc3, setValueMulticast ).Times( 0 );
        EXPECT_CALL( *cc3, setValue ).Times( 0 );
        EXPECT_CALL( *cc3, onValueRequested ).Times( 0 );

        EXPECT_CALL( *cc4, setValueMulticast ).Times( 0 );
        EXPECT_CALL( *cc4, setValue ).Times( 0 );
        EXPECT_CALL( *cc4, onValueRequested ).Times( 0 );

        valuesSetter->setValue( 1, 0, ZCc0x25Switch::ID::value, value );
    }

    // do multicast
    {
        EXPECT_CALL( *cc1, setValueMulticast( 4, _, value, GetParam().chipSinglecasts ) ).Times( 1 );
        EXPECT_CALL( *cc1, setValue( value ) ).Times( GetParam().setValueCallCount );
        EXPECT_CALL( *cc1, onValueRequested ).Times( GetParam().requestValueCallCount );

        EXPECT_CALL( *cc2, setValueMulticast ).Times( 0 );
        EXPECT_CALL( *cc2, setValue ).Times( 0 );
        EXPECT_CALL( *cc2, onValueRequested ).Times( 0 );

        EXPECT_CALL( *cc3, setValueMulticast ).Times( 0 );
        EXPECT_CALL( *cc3, setValue ).Times( 0 );
        EXPECT_CALL( *cc3, onValueRequested ).Times( 0 );

        EXPECT_CALL( *cc4, setValueMulticast ).Times( 0 );
        EXPECT_CALL( *cc4, setValue ).Times( 0 );
        EXPECT_CALL( *cc4, onValueRequested ).Times( 0 );

        const uint8_t nodeIds[] = { 1, 2, 3, 4 };
        valuesSetter->setValueMulticast( 4, nodeIds, 0, ZCc0x25Switch::ID::value, value, GetParam().mode );
    }

    // set values for 2nd and 4th nodes
    {
        EXPECT_CALL( *cc1, setValueMulticast ).Times( 0 );
        EXPECT_CALL( *cc1, setValue ).Times( 0 );
        EXPECT_CALL( *cc1, onValueRequested ).Times( 0 );

        EXPECT_CALL( *cc2, setValueMulticast ).Times( 0 );
        EXPECT_CALL( *cc2, setValue( value ) ).Times( 1 );
        EXPECT_CALL( *cc2, onValueRequested ).Times( 0 );

        EXPECT_CALL( *cc3, setValueMulticast ).Times( 0 );
        EXPECT_CALL( *cc3, setValue ).Times( 0 );
        EXPECT_CALL( *cc3, onValueRequested ).Times( 0 );

        EXPECT_CALL( *cc4, setValueMulticast ).Times( 0 );
        EXPECT_CALL( *cc4, setValue( value ) ).Times( 1 );
        EXPECT_CALL( *cc4, onValueRequested ).Times( 0 );

        valuesSetter->setValue( 2, 0, ZCc0x25Switch::ID::value, value );
        valuesSetter->setValue( 4, 0, ZCc0x25Switch::ID::value, value );
    }

    // response for 1st node
    {
        EXPECT_CALL( *cc1, setValueMulticast ).Times( 0 );
        EXPECT_CALL( *cc1, setValue ).Times( 0 );
        EXPECT_CALL( *cc1, onValueRequested ).Times( 0 );

        EXPECT_CALL( *cc2, setValueMulticast ).Times( 0 );
        EXPECT_CALL( *cc2, setValue ).Times( 0 );
        EXPECT_CALL( *cc2, onValueRequested ).Times( 0 );

        EXPECT_CALL( *cc3, setValueMulticast ).Times( 0 );
        EXPECT_CALL( *cc3, setValue( value ) ).Times( GetParam().setValueCallCount );
        EXPECT_CALL( *cc3, onValueRequested ).Times( GetParam().requestValueCallCount );

        EXPECT_CALL( *cc4, setValueMulticast ).Times( 0 );
        EXPECT_CALL( *cc4, setValue ).Times( 0 );
        EXPECT_CALL( *cc4, onValueRequested ).Times( 0 );

        getRequestsManager().onRequestSent( BinarySwitchValueRequest( 1, 0 ) );
        getRequestsManager().onRespond( BinarySwitchValueRequest( 1, 0 ), value, 0xFE, 0 );
    }

    // response for 3rd node
    {
        EXPECT_CALL( *cc1, setValueMulticast ).Times( 0 );
        EXPECT_CALL( *cc1, setValue ).Times( 0 );
        EXPECT_CALL( *cc1, onValueRequested ).Times( 0 );

        EXPECT_CALL( *cc2, setValueMulticast ).Times( 0 );
        EXPECT_CALL( *cc2, setValue ).Times( 0 );
        EXPECT_CALL( *cc2, onValueRequested ).Times( 0 );

        EXPECT_CALL( *cc3, setValueMulticast ).Times( 0 );
        EXPECT_CALL( *cc3, setValue ).Times( 0 );
        EXPECT_CALL( *cc3, onValueRequested ).Times( 0 );

        EXPECT_CALL( *cc4, setValueMulticast ).Times( 0 );
        EXPECT_CALL( *cc4, setValue ).Times( 0 );
        EXPECT_CALL( *cc4, onValueRequested ).Times( 0 );

        getRequestsManager().onRequestSent( BinarySwitchValueRequest( 3, 0 ) );
        getRequestsManager().onRespond( BinarySwitchValueRequest( 3, 0 ), value, 0xFE, 0 );
    }
}

TEST_P( ZNetCommandClassValuesSetterTestSuite, switchMultilevelMulticastTwiceForTheSameNodes ) {
    auto valuesSetter = createCommandClassValuesSetter();

    auto cc1 = createCommandClassWithNode<ZCc0x26SwitchMultiLevel>( 1 );
    auto cc2 = createCommandClassWithNode<ZCc0x26SwitchMultiLevel>( 2 );
    auto cc3 = createCommandClassWithNode<ZCc0x26SwitchMultiLevel>( 3 );

    const uint8_t nodeIds[] = { 1, 2, 3 };

    const uint8_t value = 22;

    // do first multicast
    {
        EXPECT_CALL( *cc1, setValueMulticast( 3, _, value, GetParam().chipSinglecasts ) ).Times( 1 );
        EXPECT_CALL( *cc1, setValue( value ) ).Times( GetParam().setValueCallCount );
        EXPECT_CALL( *cc1, onValueRequested ).Times( GetParam().requestValueCallCount );

        EXPECT_CALL( *cc2, setValueMulticast ).Times( 0 );
        EXPECT_CALL( *cc2, setValue ).Times( 0 );
        EXPECT_CALL( *cc2, onValueRequested ).Times( 0 );

        EXPECT_CALL( *cc3, setValueMulticast ).Times( 0 );
        EXPECT_CALL( *cc3, setValue ).Times( 0 );
        EXPECT_CALL( *cc3, onValueRequested ).Times( 0 );

        valuesSetter->setValueMulticast( 3, nodeIds, 0, ZCc0x26SwitchMultiLevel::ID::value, value, GetParam().mode );
    }

    // response received for 1st node
    {
        EXPECT_CALL( *cc1, setValueMulticast ).Times( 0 );
        EXPECT_CALL( *cc1, setValue ).Times( 0 );
        EXPECT_CALL( *cc1, onValueRequested ).Times( 0 );

        EXPECT_CALL( *cc2, setValueMulticast ).Times( 0 );
        EXPECT_CALL( *cc2, setValue( value ) ).Times( GetParam().setValueCallCount );
        EXPECT_CALL( *cc2, onValueRequested ).Times( GetParam().requestValueCallCount );

        EXPECT_CALL( *cc3, setValueMulticast ).Times( 0 );
        EXPECT_CALL( *cc3, setValue ).Times( 0 );
        EXPECT_CALL( *cc3, onValueRequested ).Times( 0 );

        getRequestsManager().onRequestSent( MultilevelSwitchValueRequest( 1, 0 ) );
        getRequestsManager().onRespond(
            MultilevelSwitchValueRequest( 1, 0 ), value, ZNET_CMD_MULTILEVEL_SWITCH_VALUE_UNKNOWN, 0 );
    }

    // do second multicast
    {
        EXPECT_CALL( *cc1, setValueMulticast( 3, _, value, GetParam().chipSinglecasts ) ).Times( 1 );
        EXPECT_CALL( *cc1, setValue( value ) ).Times( GetParam().setValueCallCount );
        EXPECT_CALL( *cc1, onValueRequested ).Times( GetParam().requestValueCallCount );

        EXPECT_CALL( *cc2, setValueMulticast ).Times( 0 );
        EXPECT_CALL( *cc2, setValue ).Times( 0 );
        EXPECT_CALL( *cc2, onValueRequested ).Times( 0 );

        EXPECT_CALL( *cc3, setValueMulticast ).Times( 0 );
        EXPECT_CALL( *cc3, setValue ).Times( 0 );
        EXPECT_CALL( *cc3, onValueRequested ).Times( 0 );

        valuesSetter->setValueMulticast( 3, nodeIds, 0, ZCc0x26SwitchMultiLevel::ID::value, value, GetParam().mode );
    }

    // response for 2nd node does nothing ( 1st multicast is inactive )
    {
        EXPECT_CALL( *cc1, setValueMulticast ).Times( 0 );
        EXPECT_CALL( *cc1, setValue ).Times( 0 );
        EXPECT_CALL( *cc1, onValueRequested ).Times( 0 );

        EXPECT_CALL( *cc2, setValueMulticast ).Times( 0 );
        EXPECT_CALL( *cc2, setValue ).Times( 0 );
        EXPECT_CALL( *cc2, onValueRequested ).Times( 0 );

        EXPECT_CALL( *cc3, setValueMulticast ).Times( 0 );
        EXPECT_CALL( *cc3, setValue ).Times( 0 );
        EXPECT_CALL( *cc3, onValueRequested ).Times( 0 );

        getRequestsManager().onRequestSent( MultilevelSwitchValueRequest( 2, 0 ) );
        getRequestsManager().onRespond(
            MultilevelSwitchValueRequest( 2, 0 ), value, ZNET_CMD_MULTILEVEL_SWITCH_VALUE_UNKNOWN, 0 );
    }

    // check 2nd multicast is active ( response for first node causes next setValue )
    {
        EXPECT_CALL( *cc1, setValueMulticast ).Times( 0 );
        EXPECT_CALL( *cc1, setValue ).Times( 0 );
        EXPECT_CALL( *cc1, onValueRequested ).Times( 0 );

        EXPECT_CALL( *cc2, setValueMulticast ).Times( 0 );
        EXPECT_CALL( *cc2, setValue( value ) ).Times( GetParam().setValueCallCount );
        EXPECT_CALL( *cc2, onValueRequested ).Times( GetParam().requestValueCallCount );

        EXPECT_CALL( *cc3, setValueMulticast ).Times( 0 );
        EXPECT_CALL( *cc3, setValue ).Times( 0 );
        EXPECT_CALL( *cc3, onValueRequested ).Times( 0 );

        getRequestsManager().onRequestSent( MultilevelSwitchValueRequest( 1, 0 ) );
        getRequestsManager().onRespond(
            MultilevelSwitchValueRequest( 1, 0 ), value, ZNET_CMD_MULTILEVEL_SWITCH_VALUE_UNKNOWN, 0 );
    }
}

TEST_P( ZNetCommandClassValuesSetterTestSuite, switchMultilevelMulticastThreeTimesForDifferentNodes ) {
    auto valuesSetter = createCommandClassValuesSetter();

    auto cc1 = createCommandClassWithNode<ZCc0x26SwitchMultiLevel>( 1 );
    auto cc2 = createCommandClassWithNode<ZCc0x26SwitchMultiLevel>( 2 );
    auto cc3 = createCommandClassWithNode<ZCc0x26SwitchMultiLevel>( 3 );
    auto cc4 = createCommandClassWithNode<ZCc0x26SwitchMultiLevel>( 4 );

    const uint8_t value = 22;

    // do multicast for nodes 1, 2, 3
    {
        EXPECT_CALL( *cc1, setValueMulticast( 3, _, value, GetParam().chipSinglecasts ) ).Times( 1 );
        EXPECT_CALL( *cc1, setValue( value ) ).Times( GetParam().setValueCallCount );
        EXPECT_CALL( *cc1, onValueRequested ).Times( GetParam().requestValueCallCount );

        EXPECT_CALL( *cc2, setValueMulticast ).Times( 0 );
        EXPECT_CALL( *cc2, setValue ).Times( 0 );
        EXPECT_CALL( *cc2, onValueRequested ).Times( 0 );

        EXPECT_CALL( *cc3, setValueMulticast ).Times( 0 );
        EXPECT_CALL( *cc3, setValue ).Times( 0 );
        EXPECT_CALL( *cc3, onValueRequested ).Times( 0 );

        EXPECT_CALL( *cc4, setValueMulticast ).Times( 0 );
        EXPECT_CALL( *cc4, setValue ).Times( 0 );
        EXPECT_CALL( *cc4, onValueRequested ).Times( 0 );

        const uint8_t nodeIds[] = { 1, 2, 3 };
        valuesSetter->setValueMulticast( 3, nodeIds, 0, ZCc0x26SwitchMultiLevel::ID::value, value, GetParam().mode );
    }

    // do multicast for nodes 2, 4
    {
        EXPECT_CALL( *cc1, setValueMulticast ).Times( 0 );
        EXPECT_CALL( *cc1, setValue ).Times( 0 );
        EXPECT_CALL( *cc1, onValueRequested ).Times( 0 );

        EXPECT_CALL( *cc2, setValueMulticast( 2, _, value, GetParam().chipSinglecasts ) ).Times( 1 );
        EXPECT_CALL( *cc2, setValue( value ) ).Times( GetParam().setValueCallCount );
        EXPECT_CALL( *cc2, onValueRequested ).Times( GetParam().requestValueCallCount );

        EXPECT_CALL( *cc3, setValueMulticast ).Times( 0 );
        EXPECT_CALL( *cc3, setValue ).Times( 0 );
        EXPECT_CALL( *cc3, onValueRequested ).Times( 0 );

        EXPECT_CALL( *cc4, setValueMulticast ).Times( 0 );
        EXPECT_CALL( *cc4, setValue ).Times( 0 );
        EXPECT_CALL( *cc4, onValueRequested ).Times( 0 );

        const uint8_t nodeIds[] = { 2, 4 };
        valuesSetter->setValueMulticast( 2, nodeIds, 0, ZCc0x26SwitchMultiLevel::ID::value, value, GetParam().mode );
    }

    // response for node 1
    {
        EXPECT_CALL( *cc1, setValueMulticast ).Times( 0 );
        EXPECT_CALL( *cc1, setValue ).Times( 0 );
        EXPECT_CALL( *cc1, onValueRequested ).Times( 0 );

        EXPECT_CALL( *cc2, setValueMulticast ).Times( 0 );
        EXPECT_CALL( *cc2, setValue ).Times( 0 );
        EXPECT_CALL( *cc2, onValueRequested ).Times( 0 );

        EXPECT_CALL( *cc3, setValueMulticast ).Times( 0 );
        EXPECT_CALL( *cc3, setValue( value ) ).Times( GetParam().setValueCallCount );
        EXPECT_CALL( *cc3, onValueRequested ).Times( GetParam().requestValueCallCount );

        EXPECT_CALL( *cc4, setValueMulticast ).Times( 0 );
        EXPECT_CALL( *cc4, setValue ).Times( 0 );
        EXPECT_CALL( *cc4, onValueRequested ).Times( 0 );

        getRequestsManager().onRequestSent( MultilevelSwitchValueRequest( 1, 0 ) );
        getRequestsManager().onRespond(
            MultilevelSwitchValueRequest( 1, 0 ), value, ZNET_CMD_MULTILEVEL_SWITCH_VALUE_UNKNOWN, 0 );
    }

    // do multicast for node 1
    {
        EXPECT_CALL( *cc1, setValueMulticast( 1, _, value, GetParam().chipSinglecasts ) ).Times( 1 );
        EXPECT_CALL( *cc1, setValue( value ) ).Times( GetParam().setValueCallCount );
        EXPECT_CALL( *cc1, onValueRequested ).Times( GetParam().requestValueCallCount );

        EXPECT_CALL( *cc2, setValueMulticast ).Times( 0 );
        EXPECT_CALL( *cc2, setValue ).Times( 0 );
        EXPECT_CALL( *cc2, onValueRequested ).Times( 0 );

        EXPECT_CALL( *cc3, setValueMulticast ).Times( 0 );
        EXPECT_CALL( *cc3, setValue ).Times( 0 );
        EXPECT_CALL( *cc3, onValueRequested ).Times( 0 );

        EXPECT_CALL( *cc4, setValueMulticast ).Times( 0 );
        EXPECT_CALL( *cc4, setValue ).Times( 0 );
        EXPECT_CALL( *cc4, onValueRequested ).Times( 0 );

        const uint8_t nodeIds[] = { 1 };
        valuesSetter->setValueMulticast( 1, nodeIds, 0, ZCc0x26SwitchMultiLevel::ID::value, value, GetParam().mode );
    }

    // response for node 2
    {
        EXPECT_CALL( *cc1, setValueMulticast ).Times( 0 );
        EXPECT_CALL( *cc1, setValue ).Times( 0 );
        EXPECT_CALL( *cc1, onValueRequested ).Times( 0 );

        EXPECT_CALL( *cc2, setValueMulticast ).Times( 0 );
        EXPECT_CALL( *cc2, setValue ).Times( 0 );
        EXPECT_CALL( *cc2, onValueRequested ).Times( 0 );

        EXPECT_CALL( *cc3, setValueMulticast ).Times( 0 );
        EXPECT_CALL( *cc3, setValue ).Times( 0 );
        EXPECT_CALL( *cc3, onValueRequested ).Times( 0 );

        EXPECT_CALL( *cc4, setValueMulticast ).Times( 0 );
        EXPECT_CALL( *cc4, setValue( value ) ).Times( GetParam().setValueCallCount );
        EXPECT_CALL( *cc4, onValueRequested ).Times( GetParam().requestValueCallCount );

        getRequestsManager().onRequestSent( MultilevelSwitchValueRequest( 2, 0 ) );
        getRequestsManager().onRespond(
            MultilevelSwitchValueRequest( 2, 0 ), value, ZNET_CMD_MULTILEVEL_SWITCH_VALUE_UNKNOWN, 0 );
    }

    // response for node 4
    {
        EXPECT_CALL( *cc1, setValueMulticast ).Times( 0 );
        EXPECT_CALL( *cc1, setValue ).Times( 0 );
        EXPECT_CALL( *cc1, onValueRequested ).Times( 0 );

        EXPECT_CALL( *cc2, setValueMulticast ).Times( 0 );
        EXPECT_CALL( *cc2, setValue ).Times( 0 );
        EXPECT_CALL( *cc2, onValueRequested ).Times( 0 );

        EXPECT_CALL( *cc3, setValueMulticast ).Times( 0 );
        EXPECT_CALL( *cc3, setValue ).Times( 0 );
        EXPECT_CALL( *cc3, onValueRequested ).Times( 0 );

        EXPECT_CALL( *cc4, setValueMulticast ).Times( 0 );
        EXPECT_CALL( *cc4, setValue ).Times( 0 );
        EXPECT_CALL( *cc4, onValueRequested ).Times( 0 );

        getRequestsManager().onRequestSent( MultilevelSwitchValueRequest( 4, 0 ) );
        getRequestsManager().onRespond(
            MultilevelSwitchValueRequest( 4, 0 ), value, ZNET_CMD_MULTILEVEL_SWITCH_VALUE_UNKNOWN, 0 );
    }
}

TEST_P( ZNetCommandClassValuesSetterTestSuite, colorSwitchMulticastsForTwoChannels ) {
    auto valuesSetter = createCommandClassValuesSetter();

    auto cc1 = createChannelCommandClassesWithNode<ZCc0x33ColorSwitch>( 10, 2 );
    auto cc2 = createChannelCommandClassesWithNode<ZCc0x33ColorSwitch>( 20, 2 );

    FakeFile file( static_cast<uint16_t>( 0x7 ) );  // 111

    auto readFromFile = [&]( ZClass& cc ) {
        file.setReadPos( 0 );
        cc.restoreData( file, 0 );
    };

    readFromFile( *cc1[0] );
    readFromFile( *cc1[1] );
    readFromFile( *cc2[0] );
    readFromFile( *cc2[1] );

    const ColorComponents value{ { znet_cl_component_t{ .component_id = 0, .value = 0 },
                                   znet_cl_component_t{ .component_id = 1, .value = 0 },
                                   znet_cl_component_t{ .component_id = 2, .value = 0 } } };

    const uint8_t nodeIds[] = { 10, 20 };

    EXPECT_CALL( *cc1[0], setValueMulticast ).Times( 0 );
    EXPECT_CALL( *cc1[0], setValue ).Times( 0 );
    EXPECT_CALL( *cc1[0], setTokenMulticast ).Times( 0 );
    EXPECT_CALL( *cc1[0], setToken ).Times( 0 );

    // do multicasts for 0 and 1 channels
    {
        EXPECT_CALL( *cc1[0], setRgbMulticast ).Times( 1 );
        EXPECT_CALL( *cc1[0], setRgb ).Times( GetParam().setValueCallCount );
        EXPECT_CALL( *cc1[0], onValueRequested ).Times( GetParam().requestValueCallCount );

        EXPECT_CALL( *cc1[1], setRgbMulticast ).Times( 1 );
        EXPECT_CALL( *cc1[1], setRgb ).Times( GetParam().setValueCallCount );
        EXPECT_CALL( *cc1[1], onValueRequested ).Times( GetParam().requestValueCallCount );

        EXPECT_CALL( *cc2[0], setRgbMulticast ).Times( 0 );
        EXPECT_CALL( *cc2[0], setRgb ).Times( 0 );
        EXPECT_CALL( *cc2[0], onValueRequested ).Times( 0 );

        EXPECT_CALL( *cc2[1], setRgbMulticast ).Times( 0 );
        EXPECT_CALL( *cc2[1], setRgb ).Times( 0 );
        EXPECT_CALL( *cc2[1], onValueRequested ).Times( 0 );

        valuesSetter->setValueMulticast( 2, nodeIds, 0, ZCc0x33ColorSwitch::ID::value, value, GetParam().mode );
        valuesSetter->setValueMulticast( 2, nodeIds, 1, ZCc0x33ColorSwitch::ID::value, value, GetParam().mode );
    }

    // 0 channel, response for two components, no expected calls
    {
        EXPECT_CALL( *cc1[0], setRgbMulticast ).Times( 0 );
        EXPECT_CALL( *cc1[0], setRgb ).Times( 0 );
        EXPECT_CALL( *cc1[0], onValueRequested ).Times( 0 );

        EXPECT_CALL( *cc1[1], setRgbMulticast ).Times( 0 );
        EXPECT_CALL( *cc1[1], setRgb ).Times( 0 );
        EXPECT_CALL( *cc1[1], onValueRequested ).Times( 0 );

        EXPECT_CALL( *cc2[0], setRgbMulticast ).Times( 0 );
        EXPECT_CALL( *cc2[0], setRgb ).Times( 0 );
        EXPECT_CALL( *cc2[0], onValueRequested ).Times( 0 );

        EXPECT_CALL( *cc2[1], setRgbMulticast ).Times( 0 );
        EXPECT_CALL( *cc2[1], setRgb ).Times( 0 );
        EXPECT_CALL( *cc2[1], onValueRequested ).Times( 0 );

        getRequestsManager().onRequestSent( ColorSwitchValueRequest( 10, 0, 0 ) );
        getRequestsManager().onRespond( ColorSwitchValueRequest( 10, 0, 0 ), znet_color_switch_report_t{} );

        getRequestsManager().onRequestSent( ColorSwitchValueRequest( 10, 0, 1 ) );
        getRequestsManager().onRespond( ColorSwitchValueRequest( 10, 0, 1 ), znet_color_switch_report_t{} );
    }

    // 0 channel, response for 3d component
    {
        EXPECT_CALL( *cc1[0], setRgbMulticast ).Times( 0 );
        EXPECT_CALL( *cc1[0], setRgb ).Times( 0 );
        EXPECT_CALL( *cc1[0], onValueRequested ).Times( 0 );

        EXPECT_CALL( *cc1[1], setRgbMulticast ).Times( 0 );
        EXPECT_CALL( *cc1[1], setRgb ).Times( 0 );
        EXPECT_CALL( *cc1[1], onValueRequested ).Times( 0 );

        EXPECT_CALL( *cc2[0], setRgbMulticast ).Times( 0 );
        EXPECT_CALL( *cc2[0], setRgb ).Times( GetParam().setValueCallCount );
        EXPECT_CALL( *cc2[0], onValueRequested ).Times( GetParam().requestValueCallCount );

        EXPECT_CALL( *cc2[1], setRgbMulticast ).Times( 0 );
        EXPECT_CALL( *cc2[1], setRgb ).Times( 0 );
        EXPECT_CALL( *cc2[1], onValueRequested ).Times( 0 );

        getRequestsManager().onRequestSent( ColorSwitchValueRequest( 10, 0, 2 ) );
        getRequestsManager().onRespond( ColorSwitchValueRequest( 10, 0, 2 ), znet_color_switch_report_t{} );
    }

    // 1st channel, response for 3 components
    {
        EXPECT_CALL( *cc1[0], setRgbMulticast ).Times( 0 );
        EXPECT_CALL( *cc1[0], setRgb ).Times( 0 );
        EXPECT_CALL( *cc1[0], onValueRequested ).Times( 0 );

        EXPECT_CALL( *cc1[1], setRgbMulticast ).Times( 0 );
        EXPECT_CALL( *cc1[1], setRgb ).Times( 0 );
        EXPECT_CALL( *cc1[1], onValueRequested ).Times( 0 );

        EXPECT_CALL( *cc2[0], setRgbMulticast ).Times( 0 );
        EXPECT_CALL( *cc2[0], setRgb ).Times( 0 );
        EXPECT_CALL( *cc2[0], onValueRequested ).Times( 0 );

        EXPECT_CALL( *cc2[1], setRgbMulticast ).Times( 0 );
        EXPECT_CALL( *cc2[1], setRgb ).Times( GetParam().setValueCallCount );
        EXPECT_CALL( *cc2[1], onValueRequested ).Times( GetParam().requestValueCallCount );

        getRequestsManager().onRequestSent( ColorSwitchValueRequest( 10, 1, 0 ) );
        getRequestsManager().onRespond( ColorSwitchValueRequest( 10, 1, 0 ), znet_color_switch_report_t{} );

        getRequestsManager().onRequestSent( ColorSwitchValueRequest( 10, 1, 1 ) );
        getRequestsManager().onRespond( ColorSwitchValueRequest( 10, 1, 1 ), znet_color_switch_report_t{} );

        getRequestsManager().onRequestSent( ColorSwitchValueRequest( 10, 1, 2 ) );
        getRequestsManager().onRespond( ColorSwitchValueRequest( 10, 1, 2 ), znet_color_switch_report_t{} );
    }
}

TEST_P( ZNetCommandClassValuesSetterTestSuite, barrierOperatorMulticastOnceForSecondChannel ) {
    auto valuesSetter = createCommandClassValuesSetter();

    auto cc1 = createChannelCommandClassesWithNode<ZCc0x66BarrierOperator>( 1, 3 )[2];
    auto cc2 = createChannelCommandClassesWithNode<ZCc0x66BarrierOperator>( 2, 3 )[2];

    const uint8_t nodeIds[] = { 2, 1 };

    const std::string value = "some_value";

    EXPECT_CALL( *cc1, setValueMulticast ).Times( 0 );
    EXPECT_CALL( *cc1, setValue ).Times( 0 );
    EXPECT_CALL( *cc1, setRgbMulticast ).Times( 0 );
    EXPECT_CALL( *cc1, setRgb ).Times( 0 );

    // do multicast
    {
        EXPECT_CALL( *cc1, setTokenMulticast ).Times( 1 );
        EXPECT_CALL( *cc1, setToken ).Times( GetParam().setValueCallCount );
        EXPECT_CALL( *cc1, onValueRequested ).Times( GetParam().requestValueCallCount );

        EXPECT_CALL( *cc2, setTokenMulticast ).Times( 0 );
        EXPECT_CALL( *cc2, setToken ).Times( 0 );
        EXPECT_CALL( *cc2, onValueRequested ).Times( 0 );

        valuesSetter->setValueMulticast( 2, nodeIds, 2, ZCc0x66BarrierOperator::ID::value, value, GetParam().mode );
    }

    // unexpected response
    {
        EXPECT_CALL( *cc1, setTokenMulticast ).Times( 0 );
        EXPECT_CALL( *cc1, setToken ).Times( 0 );
        EXPECT_CALL( *cc1, onValueRequested ).Times( 0 );

        EXPECT_CALL( *cc2, setTokenMulticast ).Times( 0 );
        EXPECT_CALL( *cc2, setToken ).Times( 0 );
        EXPECT_CALL( *cc2, onValueRequested ).Times( 0 );

        getRequestsManager().onRequestSent( BarrierOperatorRequest( 2, 2 ) );
        getRequestsManager().onRespond( BarrierOperatorRequest( 2, 2 ), uint8_t( 0 ) );
    }

    // expected response for first node
    {
        EXPECT_CALL( *cc1, setTokenMulticast ).Times( 0 );
        EXPECT_CALL( *cc1, setToken ).Times( 0 );
        EXPECT_CALL( *cc1, onValueRequested ).Times( 0 );

        EXPECT_CALL( *cc2, setTokenMulticast ).Times( 0 );
        EXPECT_CALL( *cc2, setToken ).Times( GetParam().setValueCallCount );
        EXPECT_CALL( *cc2, onValueRequested ).Times( GetParam().requestValueCallCount );

        getRequestsManager().onRequestSent( BarrierOperatorRequest( 1, 2 ) );
        getRequestsManager().onRespond( BarrierOperatorRequest( 1, 2 ), uint8_t( 0 ) );
    }

    // expected response for second node
    {
        EXPECT_CALL( *cc1, setTokenMulticast ).Times( 0 );
        EXPECT_CALL( *cc1, setToken ).Times( 0 );
        EXPECT_CALL( *cc1, onValueRequested ).Times( 0 );

        EXPECT_CALL( *cc2, setTokenMulticast ).Times( 0 );
        EXPECT_CALL( *cc2, setToken ).Times( 0 );
        EXPECT_CALL( *cc2, onValueRequested ).Times( 0 );

        getRequestsManager().onRequestSent( BarrierOperatorRequest( 2, 2 ) );
        getRequestsManager().onRespond( BarrierOperatorRequest( 2, 2 ), uint8_t( 0 ) );
    }
}

TEST_F( ZNetCommandClassValuesSetterTestSuite, secureAndUnsecureNodesWithChipMulticast ) {
    auto valuesSetter = createCommandClassValuesSetter();

    auto cc1 = createCommandClassWithNode<ZCc0x25Switch>( 1 );
    auto cc2 = createCommandClassWithNode<ZCc0x25Switch>( 2, SecurityMode::Class::S0 );
    auto cc3 = createCommandClassWithNode<ZCc0x25Switch>( 3 );

    const uint8_t nodeIds[] = { 1, 2, 3 };

    const uint8_t value = 0xFF;

    // do multicast
    {
        EXPECT_CALL( *cc1, setValueMulticast( 3, _, value, true ) ).Times( 1 );
        EXPECT_CALL( *cc1, setValue ).Times( 0 );
        EXPECT_CALL( *cc1, onValueRequested ).Times( 1 );

        EXPECT_CALL( *cc2, setValueMulticast ).Times( 0 );
        EXPECT_CALL( *cc2, setValue ).Times( 0 );
        EXPECT_CALL( *cc2, onValueRequested ).Times( 0 );

        EXPECT_CALL( *cc3, setValueMulticast ).Times( 0 );
        EXPECT_CALL( *cc3, setValue ).Times( 0 );
        EXPECT_CALL( *cc3, onValueRequested ).Times( 0 );

        valuesSetter->setValueMulticast(
            3, nodeIds, 0, ZCc0x25Switch::ID::value, value, MulticastMode::ChipSinglecasts );
    }

    // 1st node response
    {
        EXPECT_CALL( *cc1, setValueMulticast ).Times( 0 );
        EXPECT_CALL( *cc1, setValue ).Times( 0 );
        EXPECT_CALL( *cc1, onValueRequested ).Times( 0 );

        EXPECT_CALL( *cc2, setValueMulticast ).Times( 0 );
        EXPECT_CALL( *cc2, setValue( value ) ).Times( 1 );
        EXPECT_CALL( *cc2, onValueRequested ).Times( 1 );

        EXPECT_CALL( *cc3, setValueMulticast ).Times( 0 );
        EXPECT_CALL( *cc3, setValue ).Times( 0 );
        EXPECT_CALL( *cc3, onValueRequested ).Times( 0 );

        getRequestsManager().onRequestSent( BinarySwitchValueRequest( 1, 0 ) );
        getRequestsManager().onRespond( BinarySwitchValueRequest( 1, 0 ), value, 0xFE, 0 );
    }

    // 2nd node response
    {
        EXPECT_CALL( *cc1, setValueMulticast ).Times( 0 );
        EXPECT_CALL( *cc1, setValue ).Times( 0 );
        EXPECT_CALL( *cc1, onValueRequested ).Times( 0 );

        EXPECT_CALL( *cc2, setValueMulticast ).Times( 0 );
        EXPECT_CALL( *cc2, setValue ).Times( 0 );
        EXPECT_CALL( *cc2, onValueRequested ).Times( 0 );

        EXPECT_CALL( *cc3, setValueMulticast ).Times( 0 );
        EXPECT_CALL( *cc3, setValue ).Times( 0 );
        EXPECT_CALL( *cc3, onValueRequested ).Times( 1 );

        getRequestsManager().onRequestSent( BinarySwitchValueRequest( 2, 0 ) );
        getRequestsManager().onRespond( BinarySwitchValueRequest( 2, 0 ), value, 0xFE, 0 );
    }
}

TEST_F( ZNetCommandClassValuesSetterTestSuite, secureNodesWithChipMulticast ) {
    auto valuesSetter = createCommandClassValuesSetter();

    auto cc1 = createCommandClassWithNode<ZCc0x25Switch>( 1, SecurityMode::Class::S2_Unauthenticated );
    auto cc2 = createCommandClassWithNode<ZCc0x25Switch>( 2, SecurityMode::Class::S0 );
    auto cc3 = createCommandClassWithNode<ZCc0x25Switch>( 3, SecurityMode::Class::S2_Authenticated );

    const uint8_t nodeIds[] = { 1, 2, 3 };

    const uint8_t value = 0xFF;

    // do multicast
    {
        EXPECT_CALL( *cc1, setValueMulticast( 3, _, value, false ) ).Times( 1 );
        EXPECT_CALL( *cc1, setValue ).Times( 1 );
        EXPECT_CALL( *cc1, onValueRequested ).Times( 1 );

        EXPECT_CALL( *cc2, setValueMulticast ).Times( 0 );
        EXPECT_CALL( *cc2, setValue ).Times( 0 );
        EXPECT_CALL( *cc2, onValueRequested ).Times( 0 );

        EXPECT_CALL( *cc3, setValueMulticast ).Times( 0 );
        EXPECT_CALL( *cc3, setValue ).Times( 0 );
        EXPECT_CALL( *cc3, onValueRequested ).Times( 0 );

        valuesSetter->setValueMulticast(
            3, nodeIds, 0, ZCc0x25Switch::ID::value, value, MulticastMode::ChipSinglecasts );
    }

    // 1st node response
    {
        EXPECT_CALL( *cc1, setValueMulticast ).Times( 0 );
        EXPECT_CALL( *cc1, setValue ).Times( 0 );
        EXPECT_CALL( *cc1, onValueRequested ).Times( 0 );

        EXPECT_CALL( *cc2, setValueMulticast ).Times( 0 );
        EXPECT_CALL( *cc2, setValue( value ) ).Times( 1 );
        EXPECT_CALL( *cc2, onValueRequested ).Times( 1 );

        EXPECT_CALL( *cc3, setValueMulticast ).Times( 0 );
        EXPECT_CALL( *cc3, setValue ).Times( 0 );
        EXPECT_CALL( *cc3, onValueRequested ).Times( 0 );

        getRequestsManager().onRequestSent( BinarySwitchValueRequest( 1, 0 ) );
        getRequestsManager().onRespond( BinarySwitchValueRequest( 1, 0 ), value, 0xFE, 0 );
    }

    // 2nd node response
    {
        EXPECT_CALL( *cc1, setValueMulticast ).Times( 0 );
        EXPECT_CALL( *cc1, setValue ).Times( 0 );
        EXPECT_CALL( *cc1, onValueRequested ).Times( 0 );

        EXPECT_CALL( *cc2, setValueMulticast ).Times( 0 );
        EXPECT_CALL( *cc2, setValue ).Times( 0 );
        EXPECT_CALL( *cc2, onValueRequested ).Times( 0 );

        EXPECT_CALL( *cc3, setValueMulticast ).Times( 0 );
        EXPECT_CALL( *cc3, setValue ).Times( 1 );
        EXPECT_CALL( *cc3, onValueRequested ).Times( 1 );

        getRequestsManager().onRequestSent( BinarySwitchValueRequest( 2, 0 ) );
        getRequestsManager().onRespond( BinarySwitchValueRequest( 2, 0 ), value, 0xFE, 0 );
    }
}

TEST_F( ZNetCommandClassValuesSetterTestSuite, secureNodesWithoutValuesGetting ) {
    auto valuesSetter = createCommandClassValuesSetter();

    auto cc1 = createCommandClassWithNode<ZCc0x25Switch>( 1, SecurityMode::Class::S2_Unauthenticated );
    auto cc2 = createCommandClassWithNode<ZCc0x25Switch>( 2, SecurityMode::Class::S0 );
    auto cc3 = createCommandClassWithNode<ZCc0x25Switch>( 3, SecurityMode::Class::S2_Authenticated );

    const uint8_t nodeIds[] = { 1, 2, 3 };

    const uint8_t value = 0xFF;

    // do multicast
    {
        EXPECT_CALL( *cc1, setValueMulticast( 3, _, value, false ) ).Times( 1 );
        EXPECT_CALL( *cc1, setValue ).Times( 1 );
        EXPECT_CALL( *cc1, onValueRequested ).Times( 0 );

        EXPECT_CALL( *cc2, setValueMulticast ).Times( 0 );
        EXPECT_CALL( *cc2, setValue ).Times( 1 );
        EXPECT_CALL( *cc2, onValueRequested ).Times( 0 );

        EXPECT_CALL( *cc3, setValueMulticast ).Times( 0 );
        EXPECT_CALL( *cc3, setValue ).Times( 1 );
        EXPECT_CALL( *cc3, onValueRequested ).Times( 0 );

        valuesSetter->setValueMulticast(
            3, nodeIds, 0, ZCc0x25Switch::ID::value, value, MulticastMode::NoValuesGetting );
    }
}

TEST_F( ZNetCommandClassValuesSetterTestSuite, secureAndUnsecureNodesWithoutValuesGetting ) {
    auto valuesSetter = createCommandClassValuesSetter();

    auto cc1 = createCommandClassWithNode<ZCc0x25Switch>( 1, SecurityMode::Class::None );
    auto cc2 = createCommandClassWithNode<ZCc0x25Switch>( 2, SecurityMode::Class::S0 );
    auto cc3 = createCommandClassWithNode<ZCc0x25Switch>( 3, SecurityMode::Class::None );

    const uint8_t nodeIds[] = { 1, 2, 3 };

    const uint8_t value = 0xFF;

    // do multicast
    {
        EXPECT_CALL( *cc1, setValueMulticast( 3, _, value, true ) ).Times( 1 );
        EXPECT_CALL( *cc1, setValue ).Times( 0 );
        EXPECT_CALL( *cc1, onValueRequested ).Times( 0 );

        EXPECT_CALL( *cc2, setValueMulticast ).Times( 0 );
        EXPECT_CALL( *cc2, setValue ).Times( 1 );
        EXPECT_CALL( *cc2, onValueRequested ).Times( 0 );

        EXPECT_CALL( *cc3, setValueMulticast ).Times( 0 );
        EXPECT_CALL( *cc3, setValue ).Times( 0 );
        EXPECT_CALL( *cc3, onValueRequested ).Times( 0 );

        valuesSetter->setValueMulticast(
            3, nodeIds, 0, ZCc0x25Switch::ID::value, value, MulticastMode::NoValuesGetting );
    }
}

}  // namespace ut
}  // namespace znet
