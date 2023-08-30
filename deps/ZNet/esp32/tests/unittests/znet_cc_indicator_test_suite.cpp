#include <znet/znet_node.hpp>
#include <znet/cc_0x87_indicator.hpp>

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

class ZCcIndicatorTestSuite : public ::testing::Test, public NodeRequestsManagingFixture {
public:
};

TEST_F( ZCcIndicatorTestSuite, interview_v1 ) {
    getNodesProvider().nodeCreate( 1 );
    auto node = getNodesProvider().onFindNode( 1 );

    MockFunction<void( RequestFailTypeOpt )> callback;

    node->addCommandClass( 0, ZNET_COMMAND_CLASS_INDICATOR, 1 );
    auto cc = node->getCommandClass<ZCc0x87Indicator>( 0 );

    EXPECT_CALL( callback, Call( _ ) ).Times( 0 );

    EXPECT_TRUE( cc->interview( callback.AsStdFunction(), {} ) );

    EXPECT_FALSE( cc->hasInterviewDone() );
    EXPECT_CALL( callback, Call( RequestFailTypeOpt{} ) ).Times( 1 );

    znet_indicator_report_t report{};
    report.indicator_0_value = 0;

    node->getRequestsManager().onRespond( IndicatorRequestV1( 1, 0 ), report );

    EXPECT_TRUE( cc->hasInterviewDone() );
}

TEST_F( ZCcIndicatorTestSuite, interview_v3 ) {
    getNodesProvider().nodeCreate( 1 );
    auto node = getNodesProvider().onFindNode( 1 );

    MockFunction<void( RequestFailTypeOpt )> callback;

    node->addCommandClass( 0, ZNET_COMMAND_CLASS_INDICATOR, 3 );
    auto cc = node->getCommandClass<ZCc0x87Indicator>( 0 );

    EXPECT_CALL( callback, Call( _ ) ).Times( 0 );

    EXPECT_TRUE( cc->interview( callback.AsStdFunction(), {} ) );

    uint8_t buffSupported[sizeof( znet_indicator_supported_report_t ) + 1];
    memset( buffSupported, 0, sizeof( znet_indicator_supported_report_t ) + 1 );

    znet_indicator_supported_report_t* supportedReport = (znet_indicator_supported_report_t*)&buffSupported;
    supportedReport->indicator_id = 0x50;
    supportedReport->next_indicator_id = 0x00;
    supportedReport->supported_mask_len = 0x01;
    supportedReport->bit_mask[0] = 0x38;

    node->getRequestsManager().onRespond( IndicatorSupportedRequestWithUnspecifiedId( 1, 0, 0 ), *supportedReport );

    EXPECT_FALSE( cc->hasInterviewDone() );
    EXPECT_CALL( callback, Call( RequestFailTypeOpt{} ) ).Times( 1 );

    uint8_t buff[sizeof( znet_indicator_report_t ) + sizeof( znet_indicator_property_t ) * 3];
    memset( buff, 0, sizeof( znet_indicator_report_t ) + sizeof( znet_indicator_property_t ) * 3 );

    znet_indicator_report_t* report = (znet_indicator_report_t*)&buff;
    report->indicator_0_value = 0x00;
    report->indicator_objects_count = 0x03;
    report->properties[0] = { 0x50, 0x03, 0x00 };
    report->properties[1] = { 0x50, 0x04, 0x00 };
    report->properties[2] = { 0x50, 0x05, 0x00 };

    node->getRequestsManager().onRespond( IndicatorRequest( 1, 0, 0x50 ), *report );

    EXPECT_TRUE( cc->hasInterviewDone() );
}

TEST_F( ZCcIndicatorTestSuite, interview_v3_several_indicators ) {
    getNodesProvider().nodeCreate( 1 );
    auto node = getNodesProvider().onFindNode( 1 );

    MockFunction<void( RequestFailTypeOpt )> callback;

    node->addCommandClass( 0, ZNET_COMMAND_CLASS_INDICATOR, 3 );
    auto cc = node->getCommandClass<ZCc0x87Indicator>( 0 );

    EXPECT_CALL( callback, Call( _ ) ).Times( 0 );

    EXPECT_TRUE( cc->interview( callback.AsStdFunction(), {} ) );

    uint8_t buffSupported[sizeof( znet_indicator_supported_report_t ) + 1];
    memset( buffSupported, 0, sizeof( znet_indicator_supported_report_t ) + 1 );

    znet_indicator_supported_report_t* supportedReport = (znet_indicator_supported_report_t*)&buffSupported;
    supportedReport->indicator_id = 0x01;
    supportedReport->next_indicator_id = 0x50;
    supportedReport->supported_mask_len = 0x01;
    supportedReport->bit_mask[0] = 0x38;

    node->getRequestsManager().onRespond( IndicatorSupportedRequestWithUnspecifiedId( 1, 0, 0 ), *supportedReport );

    EXPECT_FALSE( cc->hasInterviewDone() );

    supportedReport->indicator_id = 0x50;
    supportedReport->next_indicator_id = 0x00;

    node->getRequestsManager().onRespond( IndicatorSupportedRequest( 1, 0, 0x50 ), *supportedReport );

    uint8_t buff[sizeof( znet_indicator_report_t ) + sizeof( znet_indicator_property_t ) * 3];
    memset( buff, 0, sizeof( znet_indicator_report_t ) + sizeof( znet_indicator_property_t ) * 3 );

    znet_indicator_report_t* report = (znet_indicator_report_t*)&buff;
    report->indicator_0_value = 0x00;
    report->indicator_objects_count = 0x03;
    report->properties[0] = { 0x01, 0x03, 0x00 };
    report->properties[1] = { 0x01, 0x04, 0x00 };
    report->properties[2] = { 0x01, 0x05, 0x00 };

    node->getRequestsManager().onRespond( IndicatorRequest( 1, 0, 0x01 ), *report );

    EXPECT_CALL( callback, Call( RequestFailTypeOpt{} ) ).Times( 1 );
    report->properties[0] = { 0x50, 0x03, 0x00 };
    report->properties[1] = { 0x50, 0x04, 0x00 };
    report->properties[2] = { 0x50, 0x05, 0x00 };

    node->getRequestsManager().onRespond( IndicatorRequest( 1, 0, 0x50 ), *report );

    EXPECT_TRUE( cc->hasInterviewDone() );
}

TEST_F( ZCcIndicatorTestSuite, interview_v3_fail ) {
    getNodesProvider().nodeCreate( 1 );
    auto node = getNodesProvider().onFindNode( 1 );

    node->addCommandClass( 0, ZNET_COMMAND_CLASS_INDICATOR, 3 );
    auto cc = node->getCommandClass<ZCc0x87Indicator>( 0 );

    uint8_t buffSupported[sizeof( znet_indicator_supported_report_t ) + 1];
    memset( buffSupported, 0, sizeof( znet_indicator_supported_report_t ) + 1 );

    znet_indicator_supported_report_t* supportedReport = (znet_indicator_supported_report_t*)&buffSupported;
    supportedReport->indicator_id = 0x50;
    supportedReport->next_indicator_id = 0x00;
    supportedReport->supported_mask_len = 0x01;
    supportedReport->bit_mask[0] = 0x38;

    // reported fail
    {
        MockFunction<void( RequestFailTypeOpt )> callback;
        EXPECT_CALL( callback, Call( _ ) ).Times( 0 );

        EXPECT_TRUE( cc->interview( callback.AsStdFunction(), {} ) );

        node->getRequestsManager().onRespond( IndicatorSupportedRequestWithUnspecifiedId( 1, 0, 0 ), *supportedReport );

        EXPECT_CALL( callback, Call( RequestFailTypeOpt( RequestFailType::ReportedFail ) ) ).Times( 1 );

        node->getRequestsManager().onFail( IndicatorRequest( 1, 0, 0x50 ) );

        EXPECT_FALSE( cc->hasInterviewDone() );
    }

    // sending fail
    {
        MockFunction<void( RequestFailTypeOpt )> callback;
        EXPECT_CALL( callback, Call( _ ) ).Times( 0 );

        getRequestsManager().addFailedRequest( IndicatorRequest( 1, 0, 0x50 ) );

        EXPECT_TRUE( cc->interview( callback.AsStdFunction(), {} ) );

        EXPECT_CALL( callback, Call( RequestFailTypeOpt( RequestFailType::SendingError ) ) ).Times( 1 );

        node->getRequestsManager().onRespond( IndicatorSupportedRequestWithUnspecifiedId( 1, 0, 0 ), *supportedReport );
        getTimerProvider().addTime( DefaultPostponedRequestDelay );

        EXPECT_FALSE( cc->hasInterviewDone() );
    }

    // first sending fail
    {
        MockFunction<void( RequestFailTypeOpt )> callback;
        EXPECT_CALL( callback, Call( _ ) ).Times( 0 );

        getRequestsManager().addFailedRequest( IndicatorSupportedRequestWithUnspecifiedId( 1, 0, 0 ) );

        EXPECT_FALSE( cc->interview( callback.AsStdFunction(), {} ) );
        EXPECT_FALSE( cc->hasInterviewDone() );
    }
}

}  // namespace ut
}  // namespace znet
