#include <znet/znet_node.hpp>
#include <znet/cc_0x20_basic.hpp>
#include <znet/znet_utils.hpp>

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "znet_node_requests_managing_fixture.h"
#include "fake_file.h"
#include "znet_fc_send_data_mock.h"
#include "MainFixture.h"

extern "C" {
#include <znet_lib_cc.h>
}

namespace znet {

namespace ut {

using ::testing::_;
using ::testing::ElementsAre;
using ::testing::Invoke;
using ::testing::MockFunction;

class ZCcBasicTestSuite : public MainFixture, public NodeRequestsManagingFixture {
public:
};

class ZCcBasicWithActuatorTestSuite : public ZCcBasicTestSuite, public WithParamInterface<znet_command_class_t> {
public:
};

TEST_F( ZCcBasicTestSuite, interview_notSupported ) {
    MockFunction<void( RequestFailTypeOpt )> callback;

    auto cc = ensureNodeAndCc<ZCc0x20Basic>( 1, 0, 1 );
    cc->getParentNode().addCommandClass( 0, ZNET_COMMAND_CLASS_USER_CODE );  // add some non-actator cc

    EXPECT_CALL( callback, Call( _ ) ).Times( 0 );

    EXPECT_TRUE( cc->interview( callback.AsStdFunction(), {} ) );

    for( int i = 0; i < 2; ++i ) {
        getRequestsManager().onRequestSent( BasicValueRequest( 1, 0 ) );
        getTimerProvider().addTime( RequestCheckingPeriod );
    }

    EXPECT_CALL( callback, Call( RequestFailTypeOpt{} ) ).Times( 0 );

    EXPECT_FALSE( cc->hasInterviewDone() );
    EXPECT_FALSE( cc->isSupported() );

    EXPECT_CALL( callback, Call( RequestFailTypeOpt{ RequestFailType::Timeout } ) ).Times( 1 );

    getRequestsManager().onRequestSent( BasicValueRequest( 1, 0 ) );
    getTimerProvider().addTime( RequestCheckingPeriod );

    EXPECT_FALSE( cc->hasInterviewDone() );
    EXPECT_FALSE( cc->isSupported() );
}

TEST_F( ZCcBasicTestSuite, interview_supported ) {
    MockFunction<void( RequestFailTypeOpt )> callback;

    auto cc = ensureNodeAndCc<ZCc0x20Basic>( 1, 0, 1 );
    cc->getParentNode().addCommandClass( 0, ZNET_COMMAND_CLASS_USER_CODE );  // add some non-actator cc

    EXPECT_CALL( callback, Call( _ ) ).Times( 0 );

    EXPECT_TRUE( cc->interview( callback.AsStdFunction(), {} ) );

    EXPECT_FALSE( cc->hasInterviewDone() );

    EXPECT_CALL( callback, Call( RequestFailTypeOpt{} ) ).Times( 1 );

    getRequestsManager().onRespond( BasicValueRequest( 1, 0 ), 0, 0, 0 );

    EXPECT_TRUE( cc->hasInterviewDone() );

    EXPECT_TRUE( cc->isSupported() );
}

TEST_F( ZCcBasicTestSuite, multiChannel_interview_rootSupported ) {
    MockFunction<void( RequestFailTypeOpt )> callback;

    auto cc2 = ensureNodeAndCc<ZCc0x20Basic>( 1, 2, 1 );
    auto cc1 = ensureNodeAndCc<ZCc0x20Basic>( 1, 1, 1 );
    auto cc0 = ensureNodeAndCc<ZCc0x20Basic>( 1, 0, 1 );

    // root channel interview
    {
        EXPECT_TRUE( cc0->interview( callback.AsStdFunction(), {} ) );
        EXPECT_CALL( callback, Call( RequestFailTypeOpt{} ) ).Times( 1 );

        getRequestsManager().onRespond( BasicValueRequest( 1, 0 ), 0, 0, 0 );

        EXPECT_TRUE( cc0->hasInterviewDone() );
        EXPECT_TRUE( cc0->isSupported() );
    }

    // 1st channel interview
    {
        EXPECT_TRUE( cc1->interview( callback.AsStdFunction(), {} ) );

        for( int i = 0; i < 2; ++i ) {
            getRequestsManager().onRequestSent( BasicValueRequest( 1, 1 ) );
            getTimerProvider().addTime( RequestCheckingPeriod );
        }

        EXPECT_CALL( callback, Call( RequestFailTypeOpt{ RequestFailType::Timeout } ) ).Times( 1 );

        getRequestsManager().onRequestSent( BasicValueRequest( 1, 1 ) );
        getTimerProvider().addTime( RequestCheckingPeriod );

        EXPECT_FALSE( cc1->hasInterviewDone() );
        EXPECT_FALSE( cc1->isSupported() );
    }

    // 2nd channel interview
    {
        EXPECT_TRUE( cc2->interview( callback.AsStdFunction(), {} ) );

        for( int i = 0; i < 2; ++i ) {
            getRequestsManager().onRequestSent( BasicValueRequest( 1, 2 ) );
            getTimerProvider().addTime( RequestCheckingPeriod );
        }

        EXPECT_CALL( callback, Call( RequestFailTypeOpt{ RequestFailType::Timeout } ) ).Times( 1 );

        getRequestsManager().onRequestSent( BasicValueRequest( 1, 2 ) );
        getTimerProvider().addTime( RequestCheckingPeriod );

        EXPECT_FALSE( cc2->hasInterviewDone() );
        EXPECT_FALSE( cc2->isSupported() );
    }
}

TEST_F( ZCcBasicTestSuite, multiChannel_interview_rootNotSupported ) {
    MockFunction<void( RequestFailTypeOpt )> callback;

    auto cc2 = ensureNodeAndCc<ZCc0x20Basic>( 1, 2, 1 );
    auto cc1 = ensureNodeAndCc<ZCc0x20Basic>( 1, 1, 1 );
    auto cc0 = ensureNodeAndCc<ZCc0x20Basic>( 1, 0, 1 );

    // root channel interview
    {
        EXPECT_TRUE( cc0->interview( callback.AsStdFunction(), {} ) );
        EXPECT_CALL( callback, Call( RequestFailTypeOpt{ RequestFailType::Timeout } ) ).Times( 1 );

        for( int i = 0; i < 3; ++i ) {
            getRequestsManager().onRequestSent( BasicValueRequest( 1, 0 ) );
            getTimerProvider().addTime( RequestCheckingPeriod );
        }

        EXPECT_FALSE( cc0->isSupported() );
    }

    // 1st channel interview
    {
        EXPECT_TRUE( cc1->interview( callback.AsStdFunction(), {} ) );

        EXPECT_CALL( callback, Call( RequestFailTypeOpt{ RequestFailType::Timeout } ) ).Times( 1 );

        getRequestsManager().onRequestSent( BasicValueRequest( 1, 1 ) );
        getTimerProvider().addTime( RequestCheckingPeriod );

        EXPECT_FALSE( cc1->hasInterviewDone() );
        EXPECT_FALSE( cc1->isSupported() );
    }

    // 2nd channel interview
    {
        EXPECT_TRUE( cc2->interview( callback.AsStdFunction(), {} ) );

        EXPECT_CALL( callback, Call( RequestFailTypeOpt{ RequestFailType::Timeout } ) ).Times( 1 );

        getRequestsManager().onRequestSent( BasicValueRequest( 1, 2 ) );
        getTimerProvider().addTime( RequestCheckingPeriod );

        EXPECT_FALSE( cc2->hasInterviewDone() );
        EXPECT_FALSE( cc2->isSupported() );
    }
}

TEST_F( ZCcBasicTestSuite, requestValue_supported ) {
    auto cc = ensureNodeAndCc<ZCc0x20Basic>( 1, 0, 1 );
    cc->interview( {}, {} );
    getRequestsManager().onRespond( BasicValueRequest( 1, 0 ), 0, 0, 0 );

    MockFunction<void( RequestFailTypeOpt )> callback;
    EXPECT_CALL( callback, Call( _ ) ).Times( 0 );

    EXPECT_TRUE( cc->requestValue( createRequestValue( callback ) ) );

    EXPECT_CALL( callback, Call( RequestFailTypeOpt{} ) ).Times( 1 );
    getRequestsManager().onRespond( BasicValueRequest( 1, 0 ), 0, 0, 0 );
}

TEST_F( ZCcBasicTestSuite, requestValue_notSupported ) {
    auto cc = ensureNodeAndCc<ZCc0x20Basic>( 1, 0, 1 );
    cc->interview( {}, {} );
    getTimerProvider().addTime( RequestCheckingPeriod * 3 );

    MockFunction<void( RequestFailTypeOpt )> callback;
    EXPECT_CALL( callback, Call( _ ) ).Times( 0 );

    EXPECT_FALSE( cc->requestValue( createRequestValue( callback ) ) );

    getTimerProvider().addTime( RequestCheckingPeriod * ( MaxRetryRequestNumber + 1 ) );
}

TEST_P( ZCcBasicWithActuatorTestSuite, interview_supported_otherActuatorCC ) {
    MockFunction<void( RequestFailTypeOpt )> callback;

    auto cc = ensureNodeAndCc<ZCc0x20Basic>( 1, 0, 1 );
    cc->getParentNode().addCommandClass( 0, GetParam() );

    EXPECT_CALL( callback, Call( _ ) ).Times( 0 );

    EXPECT_TRUE( cc->interview( callback.AsStdFunction(), {} ) );

    EXPECT_CALL( callback, Call( RequestFailTypeOpt{} ) ).Times( 1 );

    getRequestsManager().onRespond( BasicValueRequest( 1, 0 ), 0, 0, 0 );

    EXPECT_TRUE( cc->hasInterviewDone() );
    EXPECT_FALSE( cc->isSupported() );
}

TEST_P( ZCcBasicWithActuatorTestSuite, interview_notSupported_otherActuatorCC ) {
    MockFunction<void( RequestFailTypeOpt )> callback;

    auto cc = ensureNodeAndCc<ZCc0x20Basic>( 1, 0, 1 );
    cc->getParentNode().addCommandClass( 0, GetParam() );

    EXPECT_CALL( callback, Call( _ ) ).Times( 0 );

    EXPECT_TRUE( cc->interview( callback.AsStdFunction(), {} ) );

    EXPECT_CALL( callback, Call( RequestFailTypeOpt{ RequestFailType::Timeout } ) ).Times( 1 );

    getRequestsManager().onRequestSent( BasicValueRequest( 1, 0 ) );
    getTimerProvider().addTime( RequestCheckingPeriod );

    EXPECT_FALSE( cc->hasInterviewDone() );
    EXPECT_FALSE( cc->isSupported() );
}

INSTANTIATE_TEST_SUITE_P( parametric,
                          ZCcBasicWithActuatorTestSuite,
                          Values( ZNET_COMMAND_CLASS_SWITCH_BINARY,
                                  ZNET_COMMAND_CLASS_SWITCH_COLOR,
                                  ZNET_COMMAND_CLASS_DOOR_LOCK ) );

}  // namespace ut
}  // namespace znet
