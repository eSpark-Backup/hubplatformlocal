#include <znet/znet_requests_manager.hpp>
#include <znet_requests_manager_impl.ipp>

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <queue>

#include "fake_timer_provider.h"

namespace znet {
namespace ut {

using ::testing::_;
using ::testing::ElementsAre;
using ::testing::Invoke;
using ::testing::MockFunction;

namespace {

struct NativeFunctionsMock {
    MOCK_METHOD( void, funcCalled, (int));

    MOCK_METHOD( void, func1Called, (int, float));

    MOCK_METHOD( void, func2Called, ( int a, float b, uint8_t c ) );
};

NativeFunctionsMock* mock = nullptr;

}  // namespace

static int func1( int a, float b ) {
    if( mock ) {
        mock->funcCalled( 1 );
        mock->func1Called( a, b );
    }
    return 0;
}

static int func2( int a, float b, uint8_t c ) {
    if( mock ) {
        mock->funcCalled( 2 );
        mock->func2Called( a, b, c );
    }
    return 0;
}

using Result1 = std::pair<float, int>;
using Request1 = RequestInfo<RequestParams<int, float>, ZNET_FUNCTION_WRAPPER( func1 ), RespondParams<const Result1&> >;
using Request2 =
    RequestInfo<RequestParams<int, float, uint8_t>, ZNET_FUNCTION_WRAPPER( func2 ), RespondParams<int, int> >;

using TestRequestManager = RequestsManagerImpl<Request1, Request2>;

class RequestsManagerTestSuite : public ::testing::Test, public FakeTimerProvider {
protected:
    void SetUp() override {
        ASSERT_EQ( mock, nullptr );
        mock = &m_functionsMock;
    }

    void TearDown() override {
        ASSERT_EQ( mock, &m_functionsMock );
        mock = nullptr;
    }

    NativeFunctionsMock& getFunctionsMock() {
        return m_functionsMock;
    }

private:
    NativeFunctionsMock m_functionsMock;
};

TEST_F( RequestsManagerTestSuite, initialTest ) {
    // TODO
}

TEST_F( RequestsManagerTestSuite, criticalFail ) {
    TestRequestManager manager( *this );

    MockFunction<void( RequestFailType )> failCallback;
    MockFunction<void( const Result1& )> successCallback;

    Request1 request( 1, 1.1f );

    EXPECT_CALL( failCallback, Call( _ ) ).Times( 0 );
    EXPECT_CALL( successCallback, Call( _ ) ).Times( 0 );

    EXPECT_CALL( getFunctionsMock(), funcCalled( 1 ) );

    EXPECT_TRUE( manager.doRequest( request, {}, successCallback.AsStdFunction(), failCallback.AsStdFunction() ) );

    EXPECT_CALL( getFunctionsMock(), funcCalled( _ ) ).Times( 0 );
    EXPECT_CALL( failCallback, Call( RequestFailType::ReportedFail ) ).Times( 1 );
    EXPECT_EQ( manager.onFail( request ), RequestFailHandlingResult::RequestStopped );

    EXPECT_FALSE( manager.isWaitingForRespond( request ) );
}

TEST_F( RequestsManagerTestSuite, notCriticalFail ) {
    TestRequestManager manager( *this );

    MockFunction<void( RequestFailType )> failCallback;
    MockFunction<void( const Result1& )> successCallback;

    EXPECT_CALL( failCallback, Call( _ ) ).Times( 0 );
    EXPECT_CALL( successCallback, Call( _ ) ).Times( 0 );

    Request1 request( 2, 4.f );

    EXPECT_CALL( getFunctionsMock(), funcCalled( 1 ) );
    EXPECT_TRUE( manager.doRequest( request, {}, successCallback.AsStdFunction(), failCallback.AsStdFunction() ) );

    for( size_t i = 1; i <= MaxFailNumber; ++i ) {
        EXPECT_EQ( manager.onFail( request, false ), RequestFailHandlingResult::RequestContinued );
        EXPECT_TRUE( manager.isWaitingForRespond( request ) );

        EXPECT_CALL( getFunctionsMock(), funcCalled( 1 ) );
        addTime( RequestCheckingPeriod );

        EXPECT_TRUE( manager.isWaitingForRespond( request ) );
    }

    EXPECT_CALL( getFunctionsMock(), funcCalled( _ ) ).Times( 0 );
    EXPECT_CALL( failCallback, Call( RequestFailType::ReportedFail ) ).Times( 1 );
    EXPECT_EQ( manager.onFail( request, false ), RequestFailHandlingResult::RequestStopped );
    EXPECT_FALSE( manager.isWaitingForRespond( request ) );
}

TEST_F( RequestsManagerTestSuite, noReport ) {
    TestRequestManager manager( *this );

    MockFunction<void( RequestFailType )> failCallback;
    MockFunction<void( const Result1& )> successCallback;

    EXPECT_CALL( failCallback, Call( _ ) ).Times( 0 );
    EXPECT_CALL( successCallback, Call( _ ) ).Times( 0 );

    Request1 request( 2, 4.f );

    EXPECT_CALL( getFunctionsMock(), funcCalled( 1 ) );
    EXPECT_TRUE( manager.doRequest( request, {}, successCallback.AsStdFunction(), failCallback.AsStdFunction() ) );

    for( size_t i = 1; i <= MaxRetryRequestNumber; ++i ) {
        EXPECT_TRUE( manager.isWaitingForRespond( request ) );

        manager.onRequestSent( request );

        EXPECT_CALL( getFunctionsMock(), funcCalled( 1 ) );
        addTime( RequestCheckingPeriod );

        EXPECT_TRUE( manager.isWaitingForRespond( request ) );
    }

    manager.onRequestSent( request );

    EXPECT_CALL( getFunctionsMock(), funcCalled( _ ) ).Times( 0 );
    EXPECT_CALL( failCallback, Call( RequestFailType::Timeout ) ).Times( 1 );
    addTime( RequestCheckingPeriod );

    EXPECT_FALSE( manager.isWaitingForRespond( request ) );
}

}  // namespace ut
}  // namespace znet
