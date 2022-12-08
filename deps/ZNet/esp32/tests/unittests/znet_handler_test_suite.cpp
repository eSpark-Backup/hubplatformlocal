#include "fake_owner_fixture.h"
#include "MainFixture.h"
#include <znet_handler.hpp>

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <thread>

namespace znet {

namespace ut {

using ::testing::_;
using ::testing::ElementsAre;

namespace {
class TestHandler : public ZNetHandler {
public:
    using ZNetHandler::updateReachableState;
    using ZNetHandler::ZNetHandler;
};
}  // namespace

class ZNetHandlerTestSuite : public MainFixture, public FakeOwnerFixture {};

TEST_F( ZNetHandlerTestSuite, updateReachableState ) {
    getOwner().nodeCreate( 2 );
    auto node = getOwner().onFindNode( 2 );

    EXPECT_EQ( node->isReachable(), ReachableState::Unknown );

    auto& currentValuesHandler = getCurrentValuesHandler( getImpl() );
    auto& currentValuesExtendedHandler = getCurrentValuesExtendedHandler( getImpl() );

    TestHandler handler( getImpl() );

    // set reachable from unknown state
    {
        EXPECT_CALL( getOwner(), onUpdateReachable( 2, true ) );

        handler.updateReachableState( *node );
        EXPECT_EQ( node->isReachable(), ReachableState::Reachable );

        EXPECT_FALSE( currentValuesHandler.isDirty() );
        EXPECT_FALSE( currentValuesExtendedHandler.isDirty() );
    }

    auto lastUpdateTime = node->getLastUpdateTime();

    std::this_thread::sleep_for( std::chrono::milliseconds( 1 ) );

    // set reachable from reachable state
    {
        EXPECT_CALL( getOwner(), onUpdateReachable( _, _ ) ).Times( 0 );

        handler.updateReachableState( *node );
        EXPECT_EQ( node->isReachable(), ReachableState::Reachable );
        EXPECT_LT( lastUpdateTime, node->getLastUpdateTime() );

        EXPECT_FALSE( currentValuesHandler.isDirty() );
        EXPECT_FALSE( currentValuesExtendedHandler.isDirty() );
    }

    node->setReachable( ReachableState::Unreachable );
    lastUpdateTime = node->getLastUpdateTime();
    std::this_thread::sleep_for( std::chrono::milliseconds( 1 ) );

    // set reachable from unreachable state
    {
        EXPECT_CALL( getOwner(), onUpdateReachable( 2, true ) );

        handler.updateReachableState( *node );
        EXPECT_EQ( node->isReachable(), ReachableState::Reachable );
        EXPECT_LT( lastUpdateTime, node->getLastUpdateTime() );

        EXPECT_TRUE( currentValuesHandler.isNodeAddedToProcess( 2 ) );
        EXPECT_FALSE( currentValuesExtendedHandler.isDirty() );
    }
}

}  // namespace ut
}  // namespace znet
