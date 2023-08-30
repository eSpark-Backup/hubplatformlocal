#include <znet/znet_node.hpp>
#include <znet/cc_0x30_binary_sensor.hpp>

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

class ZCcBinarySensorTestSuite : public ::testing::Test, public NodeRequestsManagingFixture {
public:
};

TEST_F( ZCcBinarySensorTestSuite, interview_v1 ) {
    MockFunction<void( RequestFailTypeOpt )> callback;

    auto cc = ensureNodeAndCc<ZCc0x30BinarySensor>( 1, 0, 1 );

    EXPECT_CALL( callback, Call( _ ) ).Times( 0 );

    EXPECT_FALSE( cc->interview( callback.AsStdFunction(), {} ) );

    EXPECT_FALSE( cc->hasInterviewDone() );
    EXPECT_THAT( cc->getSensorTypes(), ElementsAre() );
}

TEST_F( ZCcBinarySensorTestSuite, interview ) {
    MockFunction<void( RequestFailTypeOpt )> callback;

    auto cc = ensureNodeAndCc<ZCc0x30BinarySensor>( 1, 0, 2 );

    EXPECT_CALL( callback, Call( _ ) ).Times( 0 );

    EXPECT_TRUE( cc->interview( callback.AsStdFunction(), {} ) );

    EXPECT_FALSE( cc->hasInterviewDone() );
    EXPECT_CALL( callback, Call( RequestFailTypeOpt{} ) ).Times( 1 );

    uint8_t bitmask = 10;
    getRequestsManager().onRespond( BinarySensorSupportedTypesRequest( 1, 0, 0 ), &bitmask, 1 );

    EXPECT_TRUE( cc->hasInterviewDone() );

    EXPECT_THAT( cc->getSensorTypes(),
                 ElementsAre( ZCc0x30BinarySensor::SensorType::GeneralPurpose, ZCc0x30BinarySensor::SensorType::CO ) );
}

TEST_F( ZCcBinarySensorTestSuite, requestValue_v1 ) {
    auto cc = ensureNodeAndCc<ZCc0x30BinarySensor>( 1, 0, 1 );
    cc->interview( {}, {} );

    MockFunction<void( RequestFailTypeOpt )> callback;
    EXPECT_CALL( callback, Call( _ ) ).Times( 0 );

    EXPECT_TRUE( cc->requestValue( createRequestValue( callback ) ) );

    EXPECT_CALL( callback, Call( _ ) ).Times( 1 );
    getRequestsManager().onRespond( BinarySensorRequestV1( 1, 0 ), 1 );
}

TEST_F( ZCcBinarySensorTestSuite, requestValue ) {
    auto cc = ensureNodeAndCc<ZCc0x30BinarySensor>( 1, 0, 2 );
    cc->interview( {}, {} );

    uint8_t bitmask = 10;
    getRequestsManager().onRespond( BinarySensorSupportedTypesRequest( 1, 0, 0 ), &bitmask, 1 );

    MockFunction<void( RequestFailTypeOpt )> callback;
    EXPECT_CALL( callback, Call( _ ) ).Times( 0 );

    EXPECT_TRUE( cc->requestValue( createRequestValue( callback ) ) );

    getRequestsManager().onRespond( BinarySensorRequest( 1, 0, 1 ), 1 );

    EXPECT_CALL( callback, Call( RequestFailTypeOpt{} ) ).Times( 1 );
    getRequestsManager().onRespond( BinarySensorRequest( 1, 0, 3 ), 1 );
}

TEST_F( ZCcBinarySensorTestSuite, requestValue_fail ) {
    auto cc = ensureNodeAndCc<ZCc0x30BinarySensor>( 1, 0, 2 );

    // empty types
    {
        MockFunction<void( RequestFailTypeOpt )> callback;
        EXPECT_CALL( callback, Call( _ ) ).Times( 0 );

        EXPECT_FALSE( cc->requestValue( createRequestValue( callback ) ) );
    }

    cc->interview( {}, {} );

    uint8_t bitmask = 10;
    getRequestsManager().onRespond( BinarySensorSupportedTypesRequest( 1, 0, 0 ), &bitmask, 1 );

    // reported fail
    {
        MockFunction<void( RequestFailTypeOpt )> callback;
        EXPECT_CALL( callback, Call( _ ) ).Times( 0 );

        EXPECT_TRUE( cc->requestValue( createRequestValue( callback ) ) );

        EXPECT_CALL( callback, Call( RequestFailTypeOpt( RequestFailType::ReportedFail ) ) ).Times( 1 );
        getRequestsManager().onFail( BinarySensorRequest( 1, 0, 1 ) );
    }

    // sending fail
    {
        MockFunction<void( RequestFailTypeOpt )> callback;
        EXPECT_CALL( callback, Call( _ ) ).Times( 0 );

        getRequestsManager().addFailedRequest( BinarySensorRequest( 1, 0, 3 ) );

        EXPECT_TRUE( cc->requestValue( createRequestValue( callback ) ) );

        EXPECT_CALL( callback, Call( RequestFailTypeOpt( RequestFailType::SendingError ) ) ).Times( 1 );
        getRequestsManager().onRespond( BinarySensorRequest( 1, 0, 1 ), 1 );
    }

    // first sending fail
    {
        MockFunction<void( RequestFailTypeOpt )> callback;
        EXPECT_CALL( callback, Call( _ ) ).Times( 0 );

        getRequestsManager().addFailedRequest( BinarySensorRequest( 1, 0, 1 ) );

        EXPECT_FALSE( cc->requestValue( createRequestValue( callback ) ) );
    }
}

}  // namespace ut
}  // namespace znet
