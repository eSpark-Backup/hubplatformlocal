#include <znet/znet_node.hpp>
#include <znet/cc_0x66_barrier_operator.hpp>
#include <znet/znet_utils.hpp>

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "znet_node_requests_managing_fixture.h"
#include "fake_file.h"

namespace znet {

namespace ut {

using ::testing::_;
using ::testing::ElementsAre;
using ::testing::Invoke;
using ::testing::MockFunction;

class ZCcBarrierOperatorTestSuite : public ::testing::Test, public NodeRequestsManagingFixture {
public:
};

TEST_F( ZCcBarrierOperatorTestSuite, interview ) {
    getNodesProvider().nodeCreate( 1 );
    auto node = getNodesProvider().onFindNode( 1 );

    MockFunction<void( RequestFailTypeOpt )> callback;

    node->addCommandClass( 0, ZNET_COMMAND_CLASS_BARRIER_OPERATOR, 8 );
    auto cc = node->getCommandClass<ZCc0x66BarrierOperator>( 0 );

    EXPECT_CALL( callback, Call( _ ) ).Times( 0 );

    EXPECT_TRUE( cc->interview( callback.AsStdFunction(), {} ) );

    uint8_t bitmask[32] = {
        0,
    };
    znet::utils::setBit( bitmask, 3 );
    znet::utils::setBit( bitmask, 6 );
    znet::utils::setBit( bitmask, 50 );

    node->getRequestsManager().onRespond( BarrierOperatorSupportedSignalsRequest( 1, 0 ), bitmask, 32 );

    node->getRequestsManager().onRespond( BarrierOperatorSignalRequest( 1, 0, 4 ), 1 );
    node->getRequestsManager().onRespond( BarrierOperatorSignalRequest( 1, 0, 7 ), 1 );

    EXPECT_CALL( callback, Call( RequestFailTypeOpt() ) ).Times( 1 );
    EXPECT_FALSE( cc->hasInterviewDone() );

    node->getRequestsManager().onRespond( BarrierOperatorSignalRequest( 1, 0, 51 ), 1 );

    EXPECT_TRUE( cc->hasInterviewDone() );
    EXPECT_THAT( cc->getSupportedSignals(), ElementsAre( 4, 7, 51 ) );
}

TEST_F( ZCcBarrierOperatorTestSuite, interview_fail ) {
    getNodesProvider().nodeCreate( 1 );
    auto node = getNodesProvider().onFindNode( 1 );

    node->addCommandClass( 0, ZNET_COMMAND_CLASS_BARRIER_OPERATOR, 1 );
    auto cc = node->getCommandClass<ZCc0x66BarrierOperator>( 0 );

    uint8_t bitmask[32] = {
        0,
    };
    znet::utils::setBit( bitmask, 3 );
    znet::utils::setBit( bitmask, 6 );
    znet::utils::setBit( bitmask, 50 );

    // reported fail
    {
        MockFunction<void( RequestFailTypeOpt )> callback;
        EXPECT_CALL( callback, Call( _ ) ).Times( 0 );

        EXPECT_TRUE( cc->interview( callback.AsStdFunction(), {} ) );

        node->getRequestsManager().onRespond( BarrierOperatorSupportedSignalsRequest( 1, 0 ), bitmask, 32 );

        node->getRequestsManager().onRespond( BarrierOperatorSignalRequest( 1, 0, 4 ), 1 );
        node->getRequestsManager().onRespond( BarrierOperatorSignalRequest( 1, 0, 7 ), 1 );

        EXPECT_CALL( callback, Call( RequestFailTypeOpt( RequestFailType::ReportedFail ) ) ).Times( 1 );
        node->getRequestsManager().onFail( BarrierOperatorSignalRequest( 1, 0, 51 ) );

        EXPECT_FALSE( cc->hasInterviewDone() );
    }

    // sending fail
    {
        MockFunction<void( RequestFailTypeOpt )> callback;
        EXPECT_CALL( callback, Call( _ ) ).Times( 0 );

        getRequestsManager().addFailedRequest( BarrierOperatorSignalRequest( 1, 0, 4 ) );

        EXPECT_TRUE( cc->interview( callback.AsStdFunction(), {} ) );

        node->getRequestsManager().onRespond( BarrierOperatorSupportedSignalsRequest( 1, 0 ), bitmask, 32 );

        EXPECT_CALL( callback, Call( RequestFailTypeOpt( RequestFailType::SendingError ) ) ).Times( 1 );

        getTimerProvider().addTime( DefaultPostponedRequestDelay );
        node->getRequestsManager().onRespond( BarrierOperatorSignalRequest( 1, 0, 4 ), 1 );
        EXPECT_FALSE( cc->hasInterviewDone() );
    }

    // first sending fail
    {
        MockFunction<void( RequestFailTypeOpt )> callback;
        EXPECT_CALL( callback, Call( _ ) ).Times( 0 );

        getRequestsManager().addFailedRequest( BarrierOperatorSupportedSignalsRequest( 1, 0 ) );

        EXPECT_FALSE( cc->interview( callback.AsStdFunction(), {} ) );
        EXPECT_FALSE( cc->hasInterviewDone() );
    }
}

}  // namespace ut
}  // namespace znet
