#include <znet/znet_node.hpp>
#include <znet/cc_0x32_meter.hpp>

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "znet_node_requests_managing_fixture.h"
#include "fake_file.h"
#include "MainFixture.h"
#include "znet_fc_send_data_mock.h"

namespace znet {

namespace ut {

using ::testing::_;
using ::testing::ElementsAre;
using ::testing::Invoke;
using ::testing::MockFunction;

class ZCcMeterTestSuite : public MainFixture, public NodeRequestsManagingFixture, public FcSendDataMock {
public:
};

TEST_F( ZCcMeterTestSuite, interview_v1 ) {
    getNodesProvider().nodeCreate( 1 );
    auto node = getNodesProvider().onFindNode( 1 );

    MockFunction<void( RequestFailTypeOpt )> callback;

    node->addCommandClass( 0, ZNET_COMMAND_CLASS_METER, 1 );
    auto cc = node->getCommandClass<ZCc0x32Meter>( 0 );

    EXPECT_CALL( callback, Call( _ ) ).Times( 0 );

    EXPECT_TRUE( cc->interview( callback.AsStdFunction(), {} ) );

    EXPECT_FALSE( cc->hasInterviewDone() );
    EXPECT_CALL( callback, Call( RequestFailTypeOpt{} ) ).Times( 1 );

    znet_meter_report_t report{};
    report.type = static_cast<uint8_t>( meter::MeterType::Gas );
    report.scale = 2;

    node->getRequestsManager().onRespond( MeterUnknownScaleRequest( 1, 0 ), report );

    EXPECT_TRUE( cc->hasInterviewDone() );

    EXPECT_EQ( cc->getType(), meter::MeterType::Gas );
    EXPECT_THAT( cc->getScales(), ElementsAre( 2 ) );
}

TEST_F( ZCcMeterTestSuite, interview_v2 ) {
    getNodesProvider().nodeCreate( 1 );
    auto node = getNodesProvider().onFindNode( 1 );

    {
        MockFunction<void( RequestFailTypeOpt )> callback;

        node->addCommandClass( 0, ZNET_COMMAND_CLASS_METER, 2 );
        auto cc = node->getCommandClass<ZCc0x32Meter>( 0 );

        EXPECT_CALL( callback, Call( _ ) ).Times( 0 );

        EXPECT_TRUE( cc->interview( callback.AsStdFunction(), {} ) );

        EXPECT_FALSE( cc->hasInterviewDone() );
        EXPECT_CALL( callback, Call( RequestFailTypeOpt{} ) ).Times( 1 );

        uint8_t bitmask[10] = {
            0,
        };
        utils::setBit( bitmask, 1 );
        utils::setBit( bitmask, 3 );
        utils::setBit( bitmask, 9 );

        znet_meter_supported_report_t report{};
        report.type = static_cast<uint8_t>( meter::MeterType::Water );
        report.scales_bitmask_size = 10;
        report.scales_bitmask = bitmask;
        report.reset = 1;

        node->getRequestsManager().onRespond( MeterSupportedTypesRequest( 1, 0 ), report );

        EXPECT_TRUE( cc->hasInterviewDone() );

        EXPECT_TRUE( cc->isResetSupported() );
        EXPECT_EQ( cc->getType(), meter::MeterType::Water );
        EXPECT_THAT( cc->getScales(), ElementsAre( 1, 3 ) );

        // Ensure that an unsupported scale wasn't requested.
        EXPECT_FALSE( node->getRequestsManager().isWaitingForRespond( MeterRequest( 1, 0, 9, 0 ) ) );

        eraseCommandClass( cc );
    }

    {
        MockFunction<void( RequestFailTypeOpt )> callback;

        node->addCommandClass( 0, ZNET_COMMAND_CLASS_METER, 2 );
        auto cc = node->getCommandClass<ZCc0x32Meter>( 0 );

        EXPECT_CALL( callback, Call( _ ) ).Times( 0 );

        EXPECT_TRUE( cc->interview( callback.AsStdFunction(), {} ) );

        EXPECT_FALSE( cc->hasInterviewDone() );
        EXPECT_CALL( callback, Call( RequestFailTypeOpt{} ) ).Times( 1 );

        uint8_t bitmask[10] = {
            0,
        };
        utils::setBit( bitmask, 9 );

        znet_meter_supported_report_t report{};
        report.type = static_cast<uint8_t>( meter::MeterType::Electric );
        report.scales_bitmask_size = 10;
        report.scales_bitmask = bitmask;
        report.reset = 0;

        node->getRequestsManager().onRespond( MeterSupportedTypesRequest( 1, 0 ), report );

        EXPECT_TRUE( cc->hasInterviewDone() );

        EXPECT_FALSE( cc->isResetSupported() );
        EXPECT_EQ( cc->getType(), meter::MeterType::Electric );
        EXPECT_THAT( cc->getScales(), ElementsAre( 9 ) );

        eraseCommandClass( cc );
    }
}

TEST_F( ZCcMeterTestSuite, requestValue ) {
    getNodesProvider().nodeCreate( 1 );
    auto node = getNodesProvider().onFindNode( 1 );

    node->addCommandClass( 0, ZNET_COMMAND_CLASS_METER, 2 );
    auto cc = node->getCommandClass<ZCc0x32Meter>( 0 );

    cc->interview( {}, {} );

    uint8_t bitmask[10] = {
        0,
    };
    utils::setBit( bitmask, 4 );
    utils::setBit( bitmask, 9 );

    znet_meter_supported_report_t report{};
    report.type = static_cast<uint8_t>( meter::MeterType::Electric );
    report.scales_bitmask_size = 10;
    report.scales_bitmask = bitmask;

    node->getRequestsManager().onRespond( MeterSupportedTypesRequest( 1, 0 ), report );

    MockFunction<void( RequestFailTypeOpt )> callback;
    EXPECT_CALL( callback, Call( _ ) ).Times( 0 );

    EXPECT_TRUE( cc->requestValue( createRequestValue( callback ) ) );

    node->getRequestsManager().onRespond( MeterRequest( 1, 0, 4, 0 ), znet_meter_report_t{} );

    EXPECT_CALL( callback, Call( RequestFailTypeOpt{} ) ).Times( 1 );
    node->getRequestsManager().onRespond( MeterRequest( 1, 0, 9, 0 ), znet_meter_report_t{} );
}

TEST_F( ZCcMeterTestSuite, requestValue_fail ) {
    getNodesProvider().nodeCreate( 1 );
    auto node = getNodesProvider().onFindNode( 1 );

    node->addCommandClass( 0, ZNET_COMMAND_CLASS_METER, 2 );
    auto cc = node->getCommandClass<ZCc0x32Meter>( 0 );

    // empty scales
    {
        MockFunction<void( RequestFailTypeOpt )> callback;
        EXPECT_CALL( callback, Call( _ ) ).Times( 0 );

        EXPECT_FALSE( cc->requestValue( createRequestValue( callback ) ) );
    }

    cc->interview( {}, {} );

    uint8_t bitmask[10] = {
        0,
    };
    utils::setBit( bitmask, 4 );
    utils::setBit( bitmask, 9 );

    znet_meter_supported_report_t report{};
    report.type = static_cast<uint8_t>( meter::MeterType::Electric );
    report.scales_bitmask_size = 10;
    report.scales_bitmask = bitmask;

    node->getRequestsManager().onRespond( MeterSupportedTypesRequest( 1, 0 ), report );

    // reported fail
    {
        MockFunction<void( RequestFailTypeOpt )> callback;
        EXPECT_CALL( callback, Call( _ ) ).Times( 0 );

        EXPECT_TRUE( cc->requestValue( createRequestValue( callback ) ) );

        node->getRequestsManager().onRespond( MeterRequest( 1, 0, 4, 0 ), znet_meter_report_t{} );

        EXPECT_CALL( callback, Call( RequestFailTypeOpt( RequestFailType::ReportedFail ) ) ).Times( 1 );
        node->getRequestsManager().onFail( MeterRequest( 1, 0, 9, 0 ) );
    }

    // sending fail
    {
        MockFunction<void( RequestFailTypeOpt )> callback;
        EXPECT_CALL( callback, Call( _ ) ).Times( 0 );

        getRequestsManager().addFailedRequest( MeterRequest( 1, 0, 9, 0 ) );

        EXPECT_TRUE( cc->requestValue( createRequestValue( callback ) ) );

        EXPECT_CALL( callback, Call( RequestFailTypeOpt( RequestFailType::SendingError ) ) ).Times( 1 );

        node->getRequestsManager().onRespond( MeterRequest( 1, 0, 4, 0 ), znet_meter_report_t{} );
    }

    // first sending fail
    {
        MockFunction<void( RequestFailTypeOpt )> callback;
        EXPECT_CALL( callback, Call( _ ) ).Times( 0 );

        getRequestsManager().addFailedRequest( MeterRequest( 1, 0, 4, 0 ) );

        EXPECT_FALSE( cc->requestValue( createRequestValue( callback ) ) );
    }
}

TEST_F( ZCcMeterTestSuite, znet_node_cmd_meter_get_test ) {
    ensureNodeAndCc<ZCc0x32Meter>( 3, 0, 6 );

    // v1
    {
        EXPECT_CALL(
            *this,
            fcSendData(
                3, 0, std::vector<uint8_t>{ ZNET_COMMAND_CLASS_METER, METER_GET }, true, true, Encapsulation_None ) );
        EXPECT_EQ( znet_node_cmd_meter_get( 3, 0, 2, 1 ), ZNET_ERR_OK );
    }

    // v2
    {
        EXPECT_CALL( *this,
                     fcSendData( 3,
                                 0,
                                 std::vector<uint8_t>{ ZNET_COMMAND_CLASS_METER, METER_GET, 0x18 },
                                 true,
                                 true,
                                 Encapsulation_None ) );
        EXPECT_EQ( znet_node_cmd_meter_get( 3, 0, 3, 2 ), ZNET_ERR_OK );
    }

    // v3
    {
        EXPECT_CALL( *this,
                     fcSendData( 3,
                                 0,
                                 std::vector<uint8_t>{ ZNET_COMMAND_CLASS_METER, METER_GET, 0x30 },
                                 true,
                                 true,
                                 Encapsulation_None ) );
        EXPECT_EQ( znet_node_cmd_meter_get( 3, 0, 6, 3 ), ZNET_ERR_OK );

        EXPECT_CALL( *this, fcSendData( _, _, _, _, _, _ ) ).Times( 0 );
        EXPECT_EQ( znet_node_cmd_meter_get( 3, 0, 7, 3 ), ZNET_ERR_INVAL );
    }

    // v4
    {
        EXPECT_CALL( *this,
                     fcSendData( 3,
                                 0,
                                 std::vector<uint8_t>{ ZNET_COMMAND_CLASS_METER, METER_GET, 0x30 },
                                 true,
                                 true,
                                 Encapsulation_None ) );
        EXPECT_EQ( znet_node_cmd_meter_get( 3, 0, 6, 4 ), ZNET_ERR_OK );

        EXPECT_CALL( *this,
                     fcSendData( 3,
                                 0,
                                 std::vector<uint8_t>{ ZNET_COMMAND_CLASS_METER, METER_GET, 0x38, 0x00 },
                                 true,
                                 true,
                                 Encapsulation_None ) );
        EXPECT_EQ( znet_node_cmd_meter_get( 3, 0, 8, 4 ), ZNET_ERR_OK );

        EXPECT_CALL( *this,
                     fcSendData( 3,
                                 0,
                                 std::vector<uint8_t>{ ZNET_COMMAND_CLASS_METER, METER_GET, 0x38, 0x04 },
                                 true,
                                 true,
                                 Encapsulation_None ) );
        EXPECT_EQ( znet_node_cmd_meter_get( 3, 0, 12, 4 ), ZNET_ERR_OK );

        EXPECT_CALL( *this, fcSendData( _, _, _, _, _, _ ) ).Times( 0 );
        EXPECT_EQ( znet_node_cmd_meter_get( 3, 0, 7, 4 ), ZNET_ERR_INVAL );
    }
}

}  // namespace ut
}  // namespace znet
