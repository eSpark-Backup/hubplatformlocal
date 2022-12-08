#include <znet/znet_node.hpp>
#include <znet/cc_0x31_sensor_multilevel.hpp>
#include <znet/znet_utils.hpp>

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "znet_node_requests_managing_fixture.h"
#include "fake_file.h"

namespace znet {

static bool operator==( const ZCc0x31SensorMultilevel::SensorInfo& a, const ZCc0x31SensorMultilevel::SensorInfo& b ) {
    return a.type == b.type && a.scales == b.scales;
}

namespace ut {

using ::testing::_;
using ::testing::ElementsAre;
using ::testing::Invoke;
using ::testing::MockFunction;

class ZCcSensorMultilevelTestSuite : public ::testing::Test, public NodeRequestsManagingFixture {
public:
};

TEST_F( ZCcSensorMultilevelTestSuite, interview_v4 ) {
    getNodesProvider().nodeCreate( 1 );
    auto node = getNodesProvider().onFindNode( 1 );

    MockFunction<void( RequestFailTypeOpt )> callback;

    node->addCommandClass( 0, ZNET_COMMAND_CLASS_SENSOR_MULTILEVEL, 4 );
    auto cc = node->getCommandClass<ZCc0x31SensorMultilevel>( 0 );

    EXPECT_CALL( callback, Call( _ ) ).Times( 0 );

    EXPECT_TRUE( cc->interview( callback.AsStdFunction(), {} ) );

    EXPECT_FALSE( cc->hasInterviewDone() );
    EXPECT_CALL( callback, Call( RequestFailTypeOpt{} ) ).Times( 1 );

    znet_multilevel_sensor_report_t report{};
    report.type = static_cast<uint8_t>( MultilevelSensor::Type::AirFlow );
    report.scale = 2;

    node->getRequestsManager().onRespond( MultiLevelSensorUnknownTypeRequest( 1, 0 ), report );

    EXPECT_TRUE( cc->hasInterviewDone() );

    const ZCc0x31SensorMultilevel::SensorInfo sensorInfo{ MultilevelSensor::Type::AirFlow, { 2 } };
    EXPECT_THAT( cc->getSupportedSensorsInfo(), ElementsAre( sensorInfo ) );
}

TEST_F( ZCcSensorMultilevelTestSuite, interview_v5 ) {
    getNodesProvider().nodeCreate( 1 );
    auto node = getNodesProvider().onFindNode( 1 );

    MockFunction<void( RequestFailTypeOpt )> callback;

    node->addCommandClass( 0, ZNET_COMMAND_CLASS_SENSOR_MULTILEVEL, 5 );
    auto cc = node->getCommandClass<ZCc0x31SensorMultilevel>( 0 );

    EXPECT_CALL( callback, Call( _ ) ).Times( 0 );

    EXPECT_TRUE( cc->interview( callback.AsStdFunction(), {} ) );

    uint8_t bitmask[32] = {
        0,
    };
    znet::utils::setBit( bitmask, static_cast<uint8_t>( MultilevelSensor::Type::Weight ) - 1 );
    znet::utils::setBit( bitmask, static_cast<uint8_t>( MultilevelSensor::Type::CO2Level ) - 1 );

    node->getRequestsManager().onRespond( MultiLevelSensorSupportedTypesRequest( 1, 0 ), bitmask, 32 );

    EXPECT_FALSE( cc->hasInterviewDone() );

    node->getRequestsManager().onRespond(
        MultiLevelSensorSupportedScalesRequest( 1, 0, static_cast<uint8_t>( MultilevelSensor::Type::Weight ) ), 3 );

    EXPECT_FALSE( cc->hasInterviewDone() );

    znet_multilevel_sensor_report_t sensorReport;
    memset( &sensorReport, 0, sizeof( sensorReport ) );

    node->getRequestsManager().onRespond(
        MultiLevelSensorRequest( 1, 0, static_cast<uint8_t>( MultilevelSensor::Type::Weight ), 0, 5 ), sensorReport );

    EXPECT_FALSE( cc->hasInterviewDone() );

    node->getRequestsManager().onRespond(
        MultiLevelSensorSupportedScalesRequest( 1, 0, static_cast<uint8_t>( MultilevelSensor::Type::CO2Level ) ), 2 );

    EXPECT_FALSE( cc->hasInterviewDone() );
    EXPECT_CALL( callback, Call( RequestFailTypeOpt{} ) ).Times( 1 );

    node->getRequestsManager().onRespond(
        MultiLevelSensorRequest( 1, 0, static_cast<uint8_t>( MultilevelSensor::Type::CO2Level ), 0, 5 ), sensorReport );

    EXPECT_TRUE( cc->hasInterviewDone() );

    const ZCc0x31SensorMultilevel::SensorInfo weightInfo{ MultilevelSensor::Type::Weight, { 0, 1 } };
    const ZCc0x31SensorMultilevel::SensorInfo co2Info{ MultilevelSensor::Type::CO2Level, { 1 } };
    EXPECT_THAT( cc->getSupportedSensorsInfo(), ElementsAre( weightInfo, co2Info ) );
}

TEST_F( ZCcSensorMultilevelTestSuite, interview_v5_fail ) {
    getNodesProvider().nodeCreate( 1 );
    auto node = getNodesProvider().onFindNode( 1 );

    node->addCommandClass( 0, ZNET_COMMAND_CLASS_SENSOR_MULTILEVEL, 5 );
    auto cc = node->getCommandClass<ZCc0x31SensorMultilevel>( 0 );

    uint8_t bitmask[32] = {
        0,
    };
    znet::utils::setBit( bitmask, static_cast<uint8_t>( MultilevelSensor::Type::Weight ) - 1 );
    znet::utils::setBit( bitmask, static_cast<uint8_t>( MultilevelSensor::Type::WaterTemperature ) - 1 );

    // scale reported fail
    {
        MockFunction<void( RequestFailTypeOpt )> callback;
        EXPECT_CALL( callback, Call( _ ) ).Times( 0 );

        EXPECT_TRUE( cc->interview( callback.AsStdFunction(), {} ) );

        node->getRequestsManager().onRespond( MultiLevelSensorSupportedTypesRequest( 1, 0 ), bitmask, 32 );
        node->getRequestsManager().onRespond(
            MultiLevelSensorSupportedScalesRequest( 1, 0, static_cast<uint8_t>( MultilevelSensor::Type::Weight ) ), 3 );

        znet_multilevel_sensor_report_t sensorReport;
        memset( &sensorReport, 0, sizeof( sensorReport ) );

        node->getRequestsManager().onRespond(
            MultiLevelSensorRequest( 1, 0, static_cast<uint8_t>( MultilevelSensor::Type::Weight ), 0, 5 ),
            sensorReport );

        EXPECT_CALL( callback, Call( RequestFailTypeOpt( RequestFailType::ReportedFail ) ) ).Times( 1 );

        node->getRequestsManager().onFail( MultiLevelSensorSupportedScalesRequest(
            1, 0, static_cast<uint8_t>( MultilevelSensor::Type::WaterTemperature ) ) );

        EXPECT_FALSE( cc->hasInterviewDone() );
    }

    // reported fail
    {
        MockFunction<void( RequestFailTypeOpt )> callback;
        EXPECT_CALL( callback, Call( _ ) ).Times( 0 );

        EXPECT_TRUE( cc->interview( callback.AsStdFunction(), {} ) );

        node->getRequestsManager().onRespond( MultiLevelSensorSupportedTypesRequest( 1, 0 ), bitmask, 32 );
        node->getRequestsManager().onRespond(
            MultiLevelSensorSupportedScalesRequest( 1, 0, static_cast<uint8_t>( MultilevelSensor::Type::Weight ) ), 3 );

        znet_multilevel_sensor_report_t sensorReport;
        memset( &sensorReport, 0, sizeof( sensorReport ) );

        node->getRequestsManager().onRespond(
            MultiLevelSensorRequest( 1, 0, static_cast<uint8_t>( MultilevelSensor::Type::Weight ), 0, 5 ),
            sensorReport );

        node->getRequestsManager().onRespond(
            MultiLevelSensorSupportedScalesRequest(
                1, 0, static_cast<uint8_t>( MultilevelSensor::Type::WaterTemperature ) ),
            1 );

        EXPECT_CALL( callback, Call( RequestFailTypeOpt( RequestFailType::ReportedFail ) ) ).Times( 1 );

        node->getRequestsManager().onFail(
            MultiLevelSensorRequest( 1, 0, static_cast<uint8_t>( MultilevelSensor::Type::WaterTemperature ), 0, 5 ) );

        EXPECT_FALSE( cc->hasInterviewDone() );
    }

    // scales sending fail
    {
        MockFunction<void( RequestFailTypeOpt )> callback;
        EXPECT_CALL( callback, Call( _ ) ).Times( 0 );

        getRequestsManager().addFailedRequest( MultiLevelSensorSupportedScalesRequest(
            1, 0, static_cast<uint8_t>( MultilevelSensor::Type::WaterTemperature ) ) );

        EXPECT_TRUE( cc->interview( callback.AsStdFunction(), {} ) );

        node->getRequestsManager().onRespond( MultiLevelSensorSupportedTypesRequest( 1, 0 ), bitmask, 32 );

        EXPECT_CALL( callback, Call( RequestFailTypeOpt( RequestFailType::SendingError ) ) ).Times( 1 );

        node->getRequestsManager().onRespond(
            MultiLevelSensorSupportedScalesRequest( 1, 0, static_cast<uint8_t>( MultilevelSensor::Type::Weight ) ), 3 );

        znet_multilevel_sensor_report_t sensorReport;
        memset( &sensorReport, 0, sizeof( sensorReport ) );

        node->getRequestsManager().onRespond(
            MultiLevelSensorRequest( 1, 0, static_cast<uint8_t>( MultilevelSensor::Type::Weight ), 0, 5 ),
            sensorReport );

        getTimerProvider().addTime( DefaultPostponedRequestDelay );

        EXPECT_FALSE( cc->hasInterviewDone() );
    }

    // sending fail
    {
        MockFunction<void( RequestFailTypeOpt )> callback;
        EXPECT_CALL( callback, Call( _ ) ).Times( 0 );

        getRequestsManager().addFailedRequest(
            MultiLevelSensorRequest( 1, 0, static_cast<uint8_t>( MultilevelSensor::Type::WaterTemperature ), 0, 5 ) );

        EXPECT_TRUE( cc->interview( callback.AsStdFunction(), {} ) );

        node->getRequestsManager().onRespond( MultiLevelSensorSupportedTypesRequest( 1, 0 ), bitmask, 32 );

        EXPECT_CALL( callback, Call( RequestFailTypeOpt( RequestFailType::SendingError ) ) ).Times( 1 );

        node->getRequestsManager().onRespond(
            MultiLevelSensorSupportedScalesRequest( 1, 0, static_cast<uint8_t>( MultilevelSensor::Type::Weight ) ), 3 );

        znet_multilevel_sensor_report_t sensorReport;
        memset( &sensorReport, 0, sizeof( sensorReport ) );

        node->getRequestsManager().onRespond(
            MultiLevelSensorRequest( 1, 0, static_cast<uint8_t>( MultilevelSensor::Type::Weight ), 0, 5 ),
            sensorReport );

        node->getRequestsManager().onRespond(
            MultiLevelSensorSupportedScalesRequest(
                1, 0, static_cast<uint8_t>( MultilevelSensor::Type::WaterTemperature ) ),
            1 );

        getTimerProvider().addTime( DefaultPostponedRequestDelay );

        EXPECT_FALSE( cc->hasInterviewDone() );
    }

    // first sending fail
    {
        MockFunction<void( RequestFailTypeOpt )> callback;
        EXPECT_CALL( callback, Call( _ ) ).Times( 0 );

        getRequestsManager().addFailedRequest( MultiLevelSensorSupportedTypesRequest( 1, 0 ) );

        EXPECT_FALSE( cc->interview( callback.AsStdFunction(), {} ) );
        EXPECT_FALSE( cc->hasInterviewDone() );
    }
}

TEST_F( ZCcSensorMultilevelTestSuite, requestValue ) {
    getNodesProvider().nodeCreate( 1 );
    auto node = getNodesProvider().onFindNode( 1 );

    node->addCommandClass( 0, ZNET_COMMAND_CLASS_SENSOR_MULTILEVEL, 5 );
    auto cc = node->getCommandClass<ZCc0x31SensorMultilevel>( 0 );

    uint8_t bitmask[32] = {
        0,
    };
    znet::utils::setBit( bitmask, static_cast<uint8_t>( MultilevelSensor::Type::Weight ) - 1 );
    znet::utils::setBit( bitmask, static_cast<uint8_t>( MultilevelSensor::Type::CO2Level ) - 1 );

    cc->interview( {}, {} );
    node->getRequestsManager().onRespond( MultiLevelSensorSupportedTypesRequest( 1, 0 ), bitmask, 32 );
    node->getRequestsManager().onRespond(
        MultiLevelSensorSupportedScalesRequest( 1, 0, static_cast<uint8_t>( MultilevelSensor::Type::Weight ) ), 3 );
    znet_multilevel_sensor_report_t sensorReport;
    memset( &sensorReport, 0, sizeof( sensorReport ) );
    node->getRequestsManager().onRespond(
        MultiLevelSensorRequest( 1, 0, static_cast<uint8_t>( MultilevelSensor::Type::Weight ), 0, 5 ), sensorReport );
    node->getRequestsManager().onRespond(
        MultiLevelSensorSupportedScalesRequest( 1, 0, static_cast<uint8_t>( MultilevelSensor::Type::CO2Level ) ), 2 );
    node->getRequestsManager().onRespond(
        MultiLevelSensorRequest( 1, 0, static_cast<uint8_t>( MultilevelSensor::Type::CO2Level ), 0, 5 ), sensorReport );

    MockFunction<void( RequestFailTypeOpt )> callback;
    EXPECT_CALL( callback, Call( _ ) ).Times( 0 );

    EXPECT_TRUE( cc->requestValue( createRequestValue( callback ) ) );

    znet_multilevel_sensor_report_t weightReport{};
    weightReport.type = static_cast<uint8_t>( MultilevelSensor::Type::Weight );
    node->getRequestsManager().onRespond( MultiLevelSensorRequest( 1, 0, weightReport.type, 0, 0 ), weightReport );

    EXPECT_CALL( callback, Call( RequestFailTypeOpt{} ) ).Times( 1 );

    znet_multilevel_sensor_report_t co2Report{};
    co2Report.type = static_cast<uint8_t>( MultilevelSensor::Type::CO2Level );
    node->getRequestsManager().onRespond( MultiLevelSensorRequest( 1, 0, co2Report.type, 0, 0 ), co2Report );
}

TEST_F( ZCcSensorMultilevelTestSuite, requestValue_fail ) {
    getNodesProvider().nodeCreate( 1 );
    auto node = getNodesProvider().onFindNode( 1 );

    node->addCommandClass( 0, ZNET_COMMAND_CLASS_SENSOR_MULTILEVEL, 5 );
    auto cc = node->getCommandClass<ZCc0x31SensorMultilevel>( 0 );

    // no types
    {
        MockFunction<void( RequestFailTypeOpt )> callback;
        EXPECT_CALL( callback, Call( _ ) ).Times( 0 );

        EXPECT_FALSE( cc->requestValue( createRequestValue( callback ) ) );
    }

    uint8_t bitmask[32] = {
        0,
    };
    znet::utils::setBit( bitmask, static_cast<uint8_t>( MultilevelSensor::Type::Weight ) - 1 );
    znet::utils::setBit( bitmask, static_cast<uint8_t>( MultilevelSensor::Type::CO2Level ) - 1 );

    cc->interview( {}, {} );
    node->getRequestsManager().onRespond( MultiLevelSensorSupportedTypesRequest( 1, 0 ), bitmask, 32 );
    node->getRequestsManager().onRespond(
        MultiLevelSensorSupportedScalesRequest( 1, 0, static_cast<uint8_t>( MultilevelSensor::Type::Weight ) ), 3 );
    znet_multilevel_sensor_report_t sensorReport;
    memset( &sensorReport, 0, sizeof( sensorReport ) );
    node->getRequestsManager().onRespond(
        MultiLevelSensorRequest( 1, 0, static_cast<uint8_t>( MultilevelSensor::Type::Weight ), 0, 5 ), sensorReport );
    node->getRequestsManager().onRespond(
        MultiLevelSensorSupportedScalesRequest( 1, 0, static_cast<uint8_t>( MultilevelSensor::Type::CO2Level ) ), 2 );
    node->getRequestsManager().onRespond(
        MultiLevelSensorRequest( 1, 0, static_cast<uint8_t>( MultilevelSensor::Type::CO2Level ), 0, 5 ), sensorReport );

    // reported fail
    {
        MockFunction<void( RequestFailTypeOpt )> callback;
        EXPECT_CALL( callback, Call( _ ) ).Times( 0 );

        EXPECT_TRUE( cc->requestValue( createRequestValue( callback ) ) );

        znet_multilevel_sensor_report_t weightReport{};
        weightReport.type = static_cast<uint8_t>( MultilevelSensor::Type::Weight );
        node->getRequestsManager().onRespond( MultiLevelSensorRequest( 1, 0, weightReport.type, 0, 0 ), weightReport );

        EXPECT_CALL( callback, Call( RequestFailTypeOpt( RequestFailType::ReportedFail ) ) ).Times( 1 );

        node->getRequestsManager().onFail(
            MultiLevelSensorRequest( 1, 0, static_cast<uint8_t>( MultilevelSensor::Type::CO2Level ), 0, 0 ) );
    }

    // sending fail
    {
        MockFunction<void( RequestFailTypeOpt )> callback;
        EXPECT_CALL( callback, Call( _ ) ).Times( 0 );

        getRequestsManager().addFailedRequest(
            MultiLevelSensorRequest( 1, 0, static_cast<uint8_t>( MultilevelSensor::Type::CO2Level ), 0, 0 ) );

        EXPECT_TRUE( cc->requestValue( createRequestValue( callback ) ) );

        EXPECT_CALL( callback, Call( RequestFailTypeOpt( RequestFailType::SendingError ) ) ).Times( 1 );

        znet_multilevel_sensor_report_t weightReport{};
        weightReport.type = static_cast<uint8_t>( MultilevelSensor::Type::Weight );

        node->getRequestsManager().onRespond( MultiLevelSensorRequest( 1, 0, weightReport.type, 0, 0 ), weightReport );
    }

    // first sending fail
    {
        MockFunction<void( RequestFailTypeOpt )> callback;
        EXPECT_CALL( callback, Call( _ ) ).Times( 0 );

        getRequestsManager().addFailedRequest(
            MultiLevelSensorRequest( 1, 0, static_cast<uint8_t>( MultilevelSensor::Type::Weight ), 0, 0 ) );

        EXPECT_FALSE( cc->requestValue( createRequestValue( callback ) ) );
    }
}

}  // namespace ut
}  // namespace znet
