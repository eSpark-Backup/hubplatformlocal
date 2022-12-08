#include <znet/znet_node.hpp>
#include <znet/cc_0x85_association.hpp>
#include <znet/cc_0x8e_multi_channel_association.hpp>

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

class ZCcMultiChannelAssociationSuite : public ::testing::Test, public NodeRequestsManagingFixture {};

TEST_F( ZCcMultiChannelAssociationSuite, interview ) {
    getNodesProvider().nodeCreate( 1 );
    auto node = getNodesProvider().onFindNode( 1 );

    MockFunction<void( RequestFailTypeOpt )> callback;

    node->addCommandClass( 0, ZNET_COMMAND_CLASS_MULTI_CHANNEL_ASSOCIATION, 3 );
    node->addCommandClass( 0, ZNET_COMMAND_CLASS_ASSOCIATION, 3 );
    node->createAdditionalChannels( 1 );
    node->addCommandClass( 1, ZNET_COMMAND_CLASS_MULTI_CHANNEL_ASSOCIATION, 3 );
    node->addCommandClass( 1, ZNET_COMMAND_CLASS_ASSOCIATION, 3 );
    auto cc = node->getCommandClass<ZCc0x8eMultiChannelAssociation>( 0 );

    EXPECT_CALL( callback, Call( _ ) ).Times( 0 );

    EXPECT_TRUE( cc->interview( callback.AsStdFunction(), {} ) );

    uint8_t groupsCount = 2;

    node->getRequestsManager().onRespond( MultiChannelAssociationSupportedGroupingRequest( 1, 0 ), groupsCount );

    // Start groups interview
    EXPECT_FALSE( cc->hasInterviewDone() );

    znet_multichannel_association_report_t associationReport;
    memset( &associationReport, 0, sizeof( associationReport ) );
    associationReport.grouping_id = 1;
    associationReport.max_end_pints_supported = 1;

    node->getRequestsManager().onRespond( MultiChannelAssociationRequest( 1, 0, 1 ), &associationReport );

    associationReport.grouping_id = 2;
    associationReport.max_end_pints_supported = 2;

    node->getRequestsManager().onRespond( MultiChannelAssociationRequest( 1, 0, 2 ), &associationReport );

    // Start lifeline group set
    node->getRequestsManager().onRespond( MultiChannelAssociationSet( 1, 0, 1, nullptr, 1 ) );

    // Get Lifeline group
    EXPECT_FALSE( cc->hasInterviewDone() );
    EXPECT_CALL( callback, Call( RequestFailTypeOpt{} ) ).Times( 1 );

    associationReport.grouping_id = 1;
    associationReport.max_end_pints_supported = 1;
    associationReport.end_pints_count = 1;
    associationReport.end_pints[0].node_id = 1;
    associationReport.end_pints[0].channel_id = 0;

    node->getRequestsManager().onRespond( MultiChannelAssociationRequest( 1, 0, 1 ), &associationReport );

    EXPECT_TRUE( cc->hasInterviewDone() );

    EXPECT_EQ( cc->getAssociationGroupsCount(), 2 );
    EXPECT_NE( cc->getGroupInfo( 1 ), nullptr );
    EXPECT_EQ( cc->getGroupInfo( 1 )->size(), 1 );
    EXPECT_EQ( cc->getGroupInfo( 1 )->at( 0 ).node_id, 1 );
    EXPECT_EQ( cc->getGroupInfo( 1 )->at( 0 ).channel_id, 0 );
    EXPECT_EQ( cc->getGroupInfo( 2 ), nullptr );

    auto ccFirstChannelAssociations = node->getCommandClass<ZCc0x85Association>( 0 );
    EXPECT_NE( ccFirstChannelAssociations.get(), nullptr );
    EXPECT_TRUE( ccFirstChannelAssociations->hasInterviewDone() );

    auto ccSecondChannelAssociations = node->getCommandClass<ZCc0x85Association>( 1 );
    EXPECT_NE( ccSecondChannelAssociations.get(), nullptr );
    EXPECT_FALSE( ccSecondChannelAssociations->hasInterviewDone() );

    auto ccSecondChannelMultiAssociations = node->getCommandClass<ZCc0x8eMultiChannelAssociation>( 1 );
    EXPECT_NE( ccSecondChannelMultiAssociations.get(), nullptr );
    EXPECT_FALSE( ccSecondChannelMultiAssociations->hasInterviewDone() );
}

TEST_F( ZCcMultiChannelAssociationSuite, interview_second_channel ) {
    getNodesProvider().nodeCreate( 1 );
    auto node = getNodesProvider().onFindNode( 1 );

    MockFunction<void( RequestFailTypeOpt )> callback;

    node->addCommandClass( 0, ZNET_COMMAND_CLASS_MULTI_CHANNEL_ASSOCIATION, 3 );
    node->addCommandClass( 0, ZNET_COMMAND_CLASS_ASSOCIATION, 3 );
    node->createAdditionalChannels( 1 );
    node->addCommandClass( 1, ZNET_COMMAND_CLASS_MULTI_CHANNEL_ASSOCIATION, 3 );
    node->addCommandClass( 1, ZNET_COMMAND_CLASS_ASSOCIATION, 3 );
    auto cc = node->getCommandClass<ZCc0x8eMultiChannelAssociation>( 1 );

    EXPECT_CALL( callback, Call( _ ) ).Times( 0 );

    EXPECT_TRUE( cc->interview( callback.AsStdFunction(), {} ) );

    uint8_t groupsCount = 2;

    node->getRequestsManager().onRespond( MultiChannelAssociationSupportedGroupingRequest( 1, 1 ), groupsCount );

    // Start groups interview
    EXPECT_FALSE( cc->hasInterviewDone() );

    znet_multichannel_association_report_t associationReport;
    memset( &associationReport, 0, sizeof( associationReport ) );
    associationReport.grouping_id = 1;
    associationReport.max_end_pints_supported = 1;

    node->getRequestsManager().onRespond( MultiChannelAssociationRequest( 1, 1, 1 ), &associationReport );

    associationReport.grouping_id = 2;
    associationReport.max_end_pints_supported = 2;

    EXPECT_CALL( callback, Call( RequestFailTypeOpt{} ) ).Times( 1 );
    node->getRequestsManager().onRespond( MultiChannelAssociationRequest( 1, 1, 2 ), &associationReport );

    EXPECT_TRUE( cc->hasInterviewDone() );

    EXPECT_EQ( cc->getAssociationGroupsCount(), 2 );
    EXPECT_EQ( cc->getGroupInfo( 1 ), nullptr );
    EXPECT_EQ( cc->getGroupInfo( 2 ), nullptr );

    auto ccSecondChannelAssociations = node->getCommandClass<ZCc0x85Association>( 1 );
    EXPECT_NE( ccSecondChannelAssociations.get(), nullptr );
    EXPECT_TRUE( ccSecondChannelAssociations->hasInterviewDone() );
}

TEST_F( ZCcMultiChannelAssociationSuite, request_value ) {
    getNodesProvider().nodeCreate( 1 );
    auto node = getNodesProvider().onFindNode( 1 );

    MockFunction<void( RequestFailTypeOpt )> callback;

    node->addCommandClass( 0, ZNET_COMMAND_CLASS_MULTI_CHANNEL_ASSOCIATION, 3 );
    node->addCommandClass( 0, ZNET_COMMAND_CLASS_ASSOCIATION, 3 );
    auto cc = node->getCommandClass<ZCc0x8eMultiChannelAssociation>( 0 );

    // request value for empty cc
    {
        EXPECT_CALL( callback, Call( _ ) ).Times( 0 );
        EXPECT_FALSE( cc->requestValue( createRequestValue( callback ) ) );
    }

    // interview
    {
        cc->interview( {}, {} );
        uint8_t groupsCount = 2;

        node->getRequestsManager().onRespond( MultiChannelAssociationSupportedGroupingRequest( 1, 0 ), groupsCount );

        // Start groups interview
        znet_multichannel_association_report_t associationReport;
        memset( &associationReport, 0, sizeof( associationReport ) );
        associationReport.grouping_id = 1;
        associationReport.max_end_pints_supported = 1;

        node->getRequestsManager().onRespond( MultiChannelAssociationRequest( 1, 0, 1 ), &associationReport );

        associationReport.grouping_id = 2;
        associationReport.max_end_pints_supported = 2;

        node->getRequestsManager().onRespond( MultiChannelAssociationRequest( 1, 0, 2 ), &associationReport );

        // Start lifeline group set
        node->getRequestsManager().onRespond( MultiChannelAssociationSet( 1, 0, 1, nullptr, 1 ) );

        // Get Lifeline group
        associationReport.grouping_id = 1;
        associationReport.max_end_pints_supported = 1;
        associationReport.end_pints_count = 1;
        associationReport.end_pints[0].node_id = 1;
        associationReport.end_pints[0].channel_id = 0;

        node->getRequestsManager().onRespond( MultiChannelAssociationRequest( 1, 0, 1 ), &associationReport );

        EXPECT_TRUE( cc->hasInterviewDone() );
    }

    // request value
    {
        EXPECT_CALL( callback, Call( _ ) ).Times( 0 );
        EXPECT_TRUE( cc->requestValue( createRequestValue( callback ) ) );

        znet_multichannel_association_report_t associationReport;
        memset( &associationReport, 0, sizeof( associationReport ) );
        associationReport.grouping_id = 1;
        associationReport.max_end_pints_supported = 1;
        associationReport.end_pints_count = 1;
        associationReport.end_pints[0].node_id = 2;
        associationReport.end_pints[0].channel_id = 0;

        node->getRequestsManager().onRespond( MultiChannelAssociationRequest( 1, 0, 1 ), &associationReport );

        associationReport.grouping_id = 2;
        associationReport.max_end_pints_supported = 2;
        associationReport.end_pints_count = 1;
        associationReport.end_pints[0].node_id = 3;
        associationReport.end_pints[0].channel_id = 2;

        EXPECT_CALL( callback, Call( RequestFailTypeOpt{} ) ).Times( 1 );
        node->getRequestsManager().onRespond( MultiChannelAssociationRequest( 1, 0, 2 ), &associationReport );

        EXPECT_EQ( cc->getAssociationGroupsCount(), 2 );
        EXPECT_NE( cc->getGroupInfo( 1 ), nullptr );
        EXPECT_EQ( cc->getGroupInfo( 1 )->size(), 1 );
        EXPECT_EQ( cc->getGroupInfo( 1 )->at( 0 ).node_id, 2 );
        EXPECT_EQ( cc->getGroupInfo( 1 )->at( 0 ).channel_id, 0 );
        EXPECT_NE( cc->getGroupInfo( 2 ), nullptr );
        EXPECT_EQ( cc->getGroupInfo( 2 )->size(), 1 );
        EXPECT_EQ( cc->getGroupInfo( 2 )->at( 0 ).node_id, 3 );
        EXPECT_EQ( cc->getGroupInfo( 2 )->at( 0 ).channel_id, 2 );
    }
}

TEST_F( ZCcMultiChannelAssociationSuite, interview_Failed ) {
    getNodesProvider().nodeCreate( 1 );
    auto node = getNodesProvider().onFindNode( 1 );

    MockFunction<void( RequestFailTypeOpt )> callback;

    node->addCommandClass( 0, ZNET_COMMAND_CLASS_MULTI_CHANNEL_ASSOCIATION, 3 );
    auto cc = node->getCommandClass<ZCc0x8eMultiChannelAssociation>( 0 );

    // First command sending failed
    {
        MockFunction<void( RequestFailTypeOpt )> callback;
        EXPECT_CALL( callback, Call( _ ) ).Times( 0 );

        getRequestsManager().addFailedRequest( MultiChannelAssociationSupportedGroupingRequest( 1, 0 ) );

        EXPECT_FALSE( cc->interview( callback.AsStdFunction(), {} ) );
        EXPECT_FALSE( cc->hasInterviewDone() );
    }

    // First command report failed
    {
        MockFunction<void( RequestFailTypeOpt )> callback;
        EXPECT_CALL( callback, Call( _ ) ).Times( 0 );

        EXPECT_CALL( callback, Call( RequestFailTypeOpt( RequestFailType::ReportedFail ) ) ).Times( 1 );

        EXPECT_TRUE( cc->interview( callback.AsStdFunction(), {} ) );

        node->getRequestsManager().onFail( MultiChannelAssociationSupportedGroupingRequest( 1, 0 ) );

        EXPECT_FALSE( cc->hasInterviewDone() );
    }

    uint8_t groupsCount = 2;

    znet_multichannel_association_report_t associationReport;
    memset( &associationReport, 0, sizeof( associationReport ) );
    associationReport.grouping_id = 1;
    associationReport.max_end_pints_supported = 1;

    // Association command sending failed
    {
        MockFunction<void( RequestFailTypeOpt )> callback;
        EXPECT_CALL( callback, Call( _ ) ).Times( 0 );

        getRequestsManager().addFailedRequest( MultiChannelAssociationRequest( 1, 0, 2 ) );

        EXPECT_TRUE( cc->interview( callback.AsStdFunction(), {} ) );

        EXPECT_CALL( callback, Call( RequestFailTypeOpt( RequestFailType::SendingError ) ) ).Times( 1 );

        node->getRequestsManager().onRespond( MultiChannelAssociationSupportedGroupingRequest( 1, 0 ), groupsCount );
        node->getRequestsManager().onRespond( MultiChannelAssociationRequest( 1, 0, 1 ), &associationReport );
        getTimerProvider().addTime( DefaultPostponedRequestDelay );

        EXPECT_FALSE( cc->hasInterviewDone() );
    }

    // Association command report failed
    {
        MockFunction<void( RequestFailTypeOpt )> callback;
        EXPECT_CALL( callback, Call( _ ) ).Times( 0 );

        EXPECT_TRUE( cc->interview( callback.AsStdFunction(), {} ) );

        node->getRequestsManager().onRespond( MultiChannelAssociationSupportedGroupingRequest( 1, 0 ), groupsCount );
        node->getRequestsManager().onRespond( MultiChannelAssociationRequest( 1, 0, 1 ), &associationReport );

        EXPECT_CALL( callback, Call( RequestFailTypeOpt( RequestFailType::ReportedFail ) ) ).Times( 1 );

        node->getRequestsManager().onFail( MultiChannelAssociationRequest( 1, 0, 2 ) );

        EXPECT_FALSE( cc->hasInterviewDone() );
    }

    // Association set command sending failed
    {
        MockFunction<void( RequestFailTypeOpt )> callback;
        EXPECT_CALL( callback, Call( _ ) ).Times( 0 );

        getRequestsManager().addFailedRequest( MultiChannelAssociationSet( 1, 0, 1, nullptr, 1 ) );

        EXPECT_TRUE( cc->interview( callback.AsStdFunction(), {} ) );

        EXPECT_CALL( callback, Call( RequestFailTypeOpt( RequestFailType::SendingError ) ) ).Times( 1 );

        node->getRequestsManager().onRespond( MultiChannelAssociationSupportedGroupingRequest( 1, 0 ), groupsCount );
        node->getRequestsManager().onRespond( MultiChannelAssociationRequest( 1, 0, 1 ), &associationReport );
        associationReport.grouping_id = 2;
        associationReport.max_end_pints_supported = 2;
        node->getRequestsManager().onRespond( MultiChannelAssociationRequest( 1, 0, 2 ), &associationReport );

        getTimerProvider().addTime( DefaultPostponedRequestDelay );

        EXPECT_FALSE( cc->hasInterviewDone() );
    }

    // Final Association command sending failed
    {
        MockFunction<void( RequestFailTypeOpt )> callback;
        EXPECT_CALL( callback, Call( _ ) ).Times( 0 );

        //        getRequestsManager().addFailedRequest( AssociationRequest( 1, 0, 2 ) );

        EXPECT_TRUE( cc->interview( callback.AsStdFunction(), {} ) );

        EXPECT_CALL( callback, Call( RequestFailTypeOpt( RequestFailType::SendingError ) ) ).Times( 1 );

        node->getRequestsManager().onRespond( MultiChannelAssociationSupportedGroupingRequest( 1, 0 ), groupsCount );
        associationReport.grouping_id = 1;
        associationReport.max_end_pints_supported = 1;
        node->getRequestsManager().onRespond( MultiChannelAssociationRequest( 1, 0, 1 ), &associationReport );
        associationReport.grouping_id = 2;
        associationReport.max_end_pints_supported = 2;
        node->getRequestsManager().onRespond( MultiChannelAssociationRequest( 1, 0, 2 ), &associationReport );
        getRequestsManager().addFailedRequest( MultiChannelAssociationRequest( 1, 0, 1 ) );
        node->getRequestsManager().onRespond( MultiChannelAssociationSet( 1, 0, 1, nullptr, 1 ) );
        getTimerProvider().addTime( DefaultPostponedRequestDelay );

        EXPECT_FALSE( cc->hasInterviewDone() );
    }

    // Final Association command report failed
    {
        MockFunction<void( RequestFailTypeOpt )> callback;
        EXPECT_CALL( callback, Call( _ ) ).Times( 0 );

        EXPECT_TRUE( cc->interview( callback.AsStdFunction(), {} ) );

        node->getRequestsManager().onRespond( MultiChannelAssociationSupportedGroupingRequest( 1, 0 ), groupsCount );
        associationReport.grouping_id = 1;
        associationReport.max_end_pints_supported = 1;
        node->getRequestsManager().onRespond( MultiChannelAssociationRequest( 1, 0, 1 ), &associationReport );
        associationReport.grouping_id = 2;
        associationReport.max_end_pints_supported = 2;
        node->getRequestsManager().onRespond( MultiChannelAssociationRequest( 1, 0, 2 ), &associationReport );
        node->getRequestsManager().onRespond( MultiChannelAssociationSet( 1, 0, 1, nullptr, 1 ) );

        EXPECT_CALL( callback, Call( RequestFailTypeOpt( RequestFailType::ReportedFail ) ) ).Times( 1 );

        node->getRequestsManager().onFail( MultiChannelAssociationRequest( 1, 0, 1 ) );

        EXPECT_FALSE( cc->hasInterviewDone() );
    }
}

}  // namespace ut
}  // namespace znet
