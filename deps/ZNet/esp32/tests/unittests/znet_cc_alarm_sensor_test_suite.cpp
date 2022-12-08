#include <znet/znet_node.hpp>
#include <znet/cc_0x9c_alarm_sensor.hpp>

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

class ZCcAlarmSensorTestSuite : public ::testing::Test, public NodeRequestsManagingFixture {
public:
};

TEST_F( ZCcAlarmSensorTestSuite, interview ) {
    MockFunction<void( RequestFailTypeOpt )> callback;

    auto cc = ensureNodeAndCc<ZCc0x9cAlarmSensor>( 1, 0, 1 );

    EXPECT_CALL( callback, Call( _ ) ).Times( 0 );

    EXPECT_TRUE( cc->interview( callback.AsStdFunction(), {} ) );

    EXPECT_FALSE( cc->hasInterviewDone() );
    EXPECT_CALL( callback, Call( RequestFailTypeOpt{} ) ).Times( 1 );

    uint8_t bitmask = 10;
    getRequestsManager().onRespond( AlarmSensorSupportedTypesRequest( 1, 0 ), &bitmask, 1 );

    EXPECT_TRUE( cc->hasInterviewDone() );

    EXPECT_THAT( cc->getSupportedTypes(), ElementsAre( AlarmSensorType::SmokeAlarm, AlarmSensorType::CO2Alarm ) );
}

TEST_F( ZCcAlarmSensorTestSuite, requestValue ) {
    auto cc = ensureNodeAndCc<ZCc0x9cAlarmSensor>( 1, 0, 1 );
    cc->interview( {}, {} );

    uint8_t bitmask = 10;
    getRequestsManager().onRespond( AlarmSensorSupportedTypesRequest( 1, 0 ), &bitmask, 1 );

    MockFunction<void( RequestFailTypeOpt )> callback;
    EXPECT_CALL( callback, Call( _ ) ).Times( 0 );

    EXPECT_TRUE( cc->requestValue( createRequestValue( callback ) ) );

    getRequestsManager().onRespond( AlarmSensorRequest( 1, 0, 1 ), 1, 0 );

    EXPECT_CALL( callback, Call( RequestFailTypeOpt{} ) ).Times( 1 );
    getRequestsManager().onRespond( AlarmSensorRequest( 1, 0, 3 ), 1, 0 );
}

TEST_F( ZCcAlarmSensorTestSuite, requestValueForOneType ) {
    auto cc = ensureNodeAndCc<ZCc0x9cAlarmSensor>( 1, 0, 1 );
    cc->interview( {}, {} );

    uint8_t bitmask = 10;
    getRequestsManager().onRespond( AlarmSensorSupportedTypesRequest( 1, 0 ), &bitmask, 1 );

    MockFunction<void( RequestFailTypeOpt )> callback;
    EXPECT_CALL( callback, Call( _ ) ).Times( 0 );

    RequestValueAlarmSensor request;
    request.sensorType = 1;
    request.resultCallback = callback.AsStdFunction();

    EXPECT_TRUE( cc->requestValue( request ) );

    EXPECT_CALL( callback, Call( RequestFailTypeOpt{} ) ).Times( 1 );

    getRequestsManager().onRespond( AlarmSensorRequest( 1, 0, 1 ), 1, 0 );

    EXPECT_FALSE( getRequestsManager().isWaitingForRespond( AlarmSensorRequest( 1, 0, 3 ) ) );
}

TEST_F( ZCcAlarmSensorTestSuite, requestValue_fail ) {
    auto cc = ensureNodeAndCc<ZCc0x9cAlarmSensor>( 1, 0, 1 );

    // empty types
    {
        MockFunction<void( RequestFailTypeOpt )> callback;
        EXPECT_CALL( callback, Call( _ ) ).Times( 0 );

        EXPECT_FALSE( cc->requestValue( createRequestValue( callback ) ) );
    }

    cc->interview( {}, {} );

    uint8_t bitmask = 10;
    getRequestsManager().onRespond( AlarmSensorSupportedTypesRequest( 1, 0 ), &bitmask, 1 );

    // reported fail
    {
        MockFunction<void( RequestFailTypeOpt )> callback;
        EXPECT_CALL( callback, Call( _ ) ).Times( 0 );

        EXPECT_TRUE( cc->requestValue( createRequestValue( callback ) ) );

        EXPECT_CALL( callback, Call( RequestFailTypeOpt( RequestFailType::ReportedFail ) ) ).Times( 1 );
        getRequestsManager().onFail( AlarmSensorRequest( 1, 0, 1 ) );
    }

    // sending fail
    {
        MockFunction<void( RequestFailTypeOpt )> callback;
        EXPECT_CALL( callback, Call( _ ) ).Times( 0 );

        getRequestsManager().addFailedRequest( AlarmSensorRequest( 1, 0, 3 ) );

        EXPECT_TRUE( cc->requestValue( createRequestValue( callback ) ) );

        EXPECT_CALL( callback, Call( RequestFailTypeOpt( RequestFailType::SendingError ) ) ).Times( 1 );
        getRequestsManager().onRespond( AlarmSensorRequest( 1, 0, 1 ), 1, 0 );
    }

    // first sending fail
    {
        MockFunction<void( RequestFailTypeOpt )> callback;
        EXPECT_CALL( callback, Call( _ ) ).Times( 0 );

        getRequestsManager().addFailedRequest( AlarmSensorRequest( 1, 0, 1 ) );

        EXPECT_FALSE( cc->requestValue( createRequestValue( callback ) ) );
    }
}

}  // namespace ut
}  // namespace znet
