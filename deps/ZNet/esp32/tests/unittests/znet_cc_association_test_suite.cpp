#include <znet/znet_node.hpp>
#include <znet/cc_0x85_association.hpp>

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

class ZCcAssociationSuite : public ::testing::Test, public NodeRequestsManagingFixture {};

TEST_F( ZCcAssociationSuite, interview ) {
    getNodesProvider().nodeCreate( 1 );
    auto node = getNodesProvider().onFindNode( 1 );

    MockFunction<void( RequestFailTypeOpt )> callback;

    node->addCommandClass( 0, ZNET_COMMAND_CLASS_ASSOCIATION, 3 );
    node->createAdditionalChannels( 1 );
    node->addCommandClass( 1, ZNET_COMMAND_CLASS_ASSOCIATION, 3 );
    auto cc = node->getCommandClass<ZCc0x85Association>( 0 );

    EXPECT_CALL( callback, Call( _ ) ).Times( 0 );

    EXPECT_TRUE( cc->interview( callback.AsStdFunction(), {} ) );

    uint8_t groupsCount = 2;

    node->getRequestsManager().onRespond( AssociationSupportedGroupingRequest( 1, 0 ), groupsCount );

    // Start groups interview
    EXPECT_FALSE( cc->hasInterviewDone() );

    znet_association_report_t associationReport;
    memset( &associationReport, 0, sizeof( associationReport ) );
    associationReport.grouping_id = 1;
    associationReport.max_nodes_supported = 1;

    node->getRequestsManager().onRespond( AssociationRequest( 1, 0, 1 ), &associationReport );

    associationReport.grouping_id = 2;
    associationReport.max_nodes_supported = 2;

    node->getRequestsManager().onRespond( AssociationRequest( 1, 0, 2 ), &associationReport );

    // Start lifeline group set
    node->getRequestsManager().onRespond( AssociationSet( 1, 0, 1, nullptr, 1 ) );

    // Get Lifeline group
    EXPECT_FALSE( cc->hasInterviewDone() );
    EXPECT_CALL( callback, Call( RequestFailTypeOpt{} ) ).Times( 1 );

    associationReport.grouping_id = 1;
    associationReport.max_nodes_supported = 1;
    associationReport.node_id_count = 1;
    associationReport.node_ids[0] = 1;

    node->getRequestsManager().onRespond( AssociationRequest( 1, 0, 1 ), &associationReport );

    EXPECT_TRUE( cc->hasInterviewDone() );

    EXPECT_EQ( cc->getAssociationGroupsCount(), 2 );
    EXPECT_NE( cc->getGroupNodes( 1 ), nullptr );
    EXPECT_EQ( cc->getGroupNodes( 1 )->size(), 1 );
    EXPECT_EQ( cc->getGroupNodes( 1 )->at( 0 ), 1 );
    EXPECT_EQ( cc->getGroupNodes( 2 ), nullptr );

    auto ccSecondChannel = node->getCommandClass<ZCc0x85Association>( 1 );
    EXPECT_NE( ccSecondChannel.get(), nullptr );
    EXPECT_FALSE( ccSecondChannel->hasInterviewDone() );
}

TEST_F( ZCcAssociationSuite, interview_second_channel ) {
    getNodesProvider().nodeCreate( 1 );
    auto node = getNodesProvider().onFindNode( 1 );

    MockFunction<void( RequestFailTypeOpt )> callback;

    node->addCommandClass( 0, ZNET_COMMAND_CLASS_ASSOCIATION, 3 );
    node->createAdditionalChannels( 1 );
    node->addCommandClass( 1, ZNET_COMMAND_CLASS_ASSOCIATION, 3 );
    auto cc = node->getCommandClass<ZCc0x85Association>( 1 );

    EXPECT_CALL( callback, Call( _ ) ).Times( 0 );

    EXPECT_TRUE( cc->interview( callback.AsStdFunction(), {} ) );

    uint8_t groupsCount = 2;

    node->getRequestsManager().onRespond( AssociationSupportedGroupingRequest( 1, 1 ), groupsCount );

    // Start groups interview
    EXPECT_FALSE( cc->hasInterviewDone() );

    znet_association_report_t associationReport;
    memset( &associationReport, 0, sizeof( associationReport ) );
    associationReport.grouping_id = 1;
    associationReport.max_nodes_supported = 1;

    node->getRequestsManager().onRespond( AssociationRequest( 1, 1, 1 ), &associationReport );

    associationReport.grouping_id = 2;
    associationReport.max_nodes_supported = 2;

    EXPECT_CALL( callback, Call( RequestFailTypeOpt{} ) ).Times( 1 );
    node->getRequestsManager().onRespond( AssociationRequest( 1, 1, 2 ), &associationReport );

    EXPECT_TRUE( cc->hasInterviewDone() );

    EXPECT_EQ( cc->getAssociationGroupsCount(), 2 );
    EXPECT_EQ( cc->getGroupNodes( 1 ), nullptr );
    EXPECT_EQ( cc->getGroupNodes( 2 ), nullptr );
}

TEST_F( ZCcAssociationSuite, request_value ) {
    getNodesProvider().nodeCreate( 1 );
    auto node = getNodesProvider().onFindNode( 1 );

    MockFunction<void( RequestFailTypeOpt )> callback;

    node->addCommandClass( 0, ZNET_COMMAND_CLASS_ASSOCIATION, 3 );
    auto cc = node->getCommandClass<ZCc0x85Association>( 0 );

    // request value for empty cc
    {
        EXPECT_CALL( callback, Call( _ ) ).Times( 0 );
        EXPECT_FALSE( cc->requestValue( createRequestValue( callback ) ) );
    }

    // interview
    {
        cc->interview( {}, {} );

        uint8_t groupsCount = 2;

        node->getRequestsManager().onRespond( AssociationSupportedGroupingRequest( 1, 0 ), groupsCount );

        // Start groups interview
        znet_association_report_t associationReport;
        memset( &associationReport, 0, sizeof( associationReport ) );
        associationReport.grouping_id = 1;
        associationReport.max_nodes_supported = 1;

        node->getRequestsManager().onRespond( AssociationRequest( 1, 0, 1 ), &associationReport );

        associationReport.grouping_id = 2;
        associationReport.max_nodes_supported = 2;

        node->getRequestsManager().onRespond( AssociationRequest( 1, 0, 2 ), &associationReport );

        // Start lifeline group set
        node->getRequestsManager().onRespond( AssociationSet( 1, 0, 1, nullptr, 1 ) );

        // Get Lifeline group
        associationReport.grouping_id = 1;
        associationReport.max_nodes_supported = 1;
        associationReport.node_id_count = 1;
        associationReport.node_ids[0] = 1;

        node->getRequestsManager().onRespond( AssociationRequest( 1, 0, 1 ), &associationReport );

        EXPECT_TRUE( cc->hasInterviewDone() );
    }

    // request value
    {
        EXPECT_CALL( callback, Call( _ ) ).Times( 0 );
        EXPECT_TRUE( cc->requestValue( createRequestValue( callback ) ) );

        znet_association_report_t associationReport;
        memset( &associationReport, 0, sizeof( associationReport ) );
        associationReport.grouping_id = 1;
        associationReport.max_nodes_supported = 1;
        associationReport.node_id_count = 1;
        associationReport.node_ids[0] = 2;

        node->getRequestsManager().onRespond( AssociationRequest( 1, 0, 1 ), &associationReport );

        associationReport.grouping_id = 2;
        associationReport.max_nodes_supported = 2;
        associationReport.node_ids[0] = 3;

        EXPECT_CALL( callback, Call( RequestFailTypeOpt{} ) ).Times( 1 );
        node->getRequestsManager().onRespond( AssociationRequest( 1, 0, 2 ), &associationReport );

        EXPECT_EQ( cc->getAssociationGroupsCount(), 2 );
        EXPECT_NE( cc->getGroupNodes( 1 ), nullptr );
        EXPECT_EQ( cc->getGroupNodes( 1 )->size(), 1 );
        EXPECT_EQ( cc->getGroupNodes( 1 )->at( 0 ), 2 );
        EXPECT_NE( cc->getGroupNodes( 2 ), nullptr );
        EXPECT_EQ( cc->getGroupNodes( 2 )->size(), 1 );
        EXPECT_EQ( cc->getGroupNodes( 2 )->at( 0 ), 3 );
    }
}

TEST_F( ZCcAssociationSuite, interview_failed ) {
    getNodesProvider().nodeCreate( 1 );
    auto node = getNodesProvider().onFindNode( 1 );

    MockFunction<void( RequestFailTypeOpt )> callback;

    node->addCommandClass( 0, ZNET_COMMAND_CLASS_ASSOCIATION, 3 );
    auto cc = node->getCommandClass<ZCc0x85Association>( 0 );

    // First command sending failed
    {
        MockFunction<void( RequestFailTypeOpt )> callback;
        EXPECT_CALL( callback, Call( _ ) ).Times( 0 );

        getRequestsManager().addFailedRequest( AssociationSupportedGroupingRequest( 1, 0 ) );

        EXPECT_FALSE( cc->interview( callback.AsStdFunction(), {} ) );
        EXPECT_FALSE( cc->hasInterviewDone() );
    }

    // First command report failed
    {
        MockFunction<void( RequestFailTypeOpt )> callback;
        EXPECT_CALL( callback, Call( _ ) ).Times( 0 );

        EXPECT_CALL( callback, Call( RequestFailTypeOpt( RequestFailType::ReportedFail ) ) ).Times( 1 );

        EXPECT_TRUE( cc->interview( callback.AsStdFunction(), {} ) );

        node->getRequestsManager().onFail( AssociationSupportedGroupingRequest( 1, 0 ) );

        EXPECT_FALSE( cc->hasInterviewDone() );
    }

    uint8_t groupsCount = 2;

    znet_association_report_t associationReport;
    memset( &associationReport, 0, sizeof( associationReport ) );
    associationReport.grouping_id = 1;
    associationReport.max_nodes_supported = 1;

    // Association command sending failed
    {
        MockFunction<void( RequestFailTypeOpt )> callback;
        EXPECT_CALL( callback, Call( _ ) ).Times( 0 );

        getRequestsManager().addFailedRequest( AssociationRequest( 1, 0, 2 ) );

        EXPECT_TRUE( cc->interview( callback.AsStdFunction(), {} ) );

        EXPECT_CALL( callback, Call( RequestFailTypeOpt( RequestFailType::SendingError ) ) ).Times( 1 );

        node->getRequestsManager().onRespond( AssociationSupportedGroupingRequest( 1, 0 ), groupsCount );
        node->getRequestsManager().onRespond( AssociationRequest( 1, 0, 1 ), &associationReport );
        getTimerProvider().addTime( DefaultPostponedRequestDelay );

        EXPECT_FALSE( cc->hasInterviewDone() );
    }

    // Association command report failed
    {
        MockFunction<void( RequestFailTypeOpt )> callback;
        EXPECT_CALL( callback, Call( _ ) ).Times( 0 );

        EXPECT_TRUE( cc->interview( callback.AsStdFunction(), {} ) );

        node->getRequestsManager().onRespond( AssociationSupportedGroupingRequest( 1, 0 ), groupsCount );
        node->getRequestsManager().onRespond( AssociationRequest( 1, 0, 1 ), &associationReport );

        EXPECT_CALL( callback, Call( RequestFailTypeOpt( RequestFailType::ReportedFail ) ) ).Times( 1 );

        node->getRequestsManager().onFail( AssociationRequest( 1, 0, 2 ) );

        EXPECT_FALSE( cc->hasInterviewDone() );
    }

    // Association set command sending failed
    {
        MockFunction<void( RequestFailTypeOpt )> callback;
        EXPECT_CALL( callback, Call( _ ) ).Times( 0 );

        getRequestsManager().addFailedRequest( AssociationSet( 1, 0, 1, nullptr, 1 ) );

        EXPECT_TRUE( cc->interview( callback.AsStdFunction(), {} ) );

        EXPECT_CALL( callback, Call( RequestFailTypeOpt( RequestFailType::SendingError ) ) ).Times( 1 );

        node->getRequestsManager().onRespond( AssociationSupportedGroupingRequest( 1, 0 ), groupsCount );
        node->getRequestsManager().onRespond( AssociationRequest( 1, 0, 1 ), &associationReport );
        associationReport.grouping_id = 2;
        associationReport.max_nodes_supported = 2;
        node->getRequestsManager().onRespond( AssociationRequest( 1, 0, 2 ), &associationReport );

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

        node->getRequestsManager().onRespond( AssociationSupportedGroupingRequest( 1, 0 ), groupsCount );
        associationReport.grouping_id = 1;
        associationReport.max_nodes_supported = 1;
        node->getRequestsManager().onRespond( AssociationRequest( 1, 0, 1 ), &associationReport );
        associationReport.grouping_id = 2;
        associationReport.max_nodes_supported = 2;
        node->getRequestsManager().onRespond( AssociationRequest( 1, 0, 2 ), &associationReport );
        getRequestsManager().addFailedRequest( AssociationRequest( 1, 0, 1 ) );
        node->getRequestsManager().onRespond( AssociationSet( 1, 0, 1, nullptr, 1 ) );
        getTimerProvider().addTime( DefaultPostponedRequestDelay );

        EXPECT_FALSE( cc->hasInterviewDone() );
    }

    // Final Association command report failed
    {
        MockFunction<void( RequestFailTypeOpt )> callback;
        EXPECT_CALL( callback, Call( _ ) ).Times( 0 );

        EXPECT_TRUE( cc->interview( callback.AsStdFunction(), {} ) );

        node->getRequestsManager().onRespond( AssociationSupportedGroupingRequest( 1, 0 ), groupsCount );
        associationReport.grouping_id = 1;
        associationReport.max_nodes_supported = 1;
        node->getRequestsManager().onRespond( AssociationRequest( 1, 0, 1 ), &associationReport );
        associationReport.grouping_id = 2;
        associationReport.max_nodes_supported = 2;
        node->getRequestsManager().onRespond( AssociationRequest( 1, 0, 2 ), &associationReport );
        node->getRequestsManager().onRespond( AssociationSet( 1, 0, 1, nullptr, 1 ) );

        EXPECT_CALL( callback, Call( RequestFailTypeOpt( RequestFailType::ReportedFail ) ) ).Times( 1 );

        node->getRequestsManager().onFail( AssociationRequest( 1, 0, 1 ) );

        EXPECT_FALSE( cc->hasInterviewDone() );
    }
}

}  // namespace ut
}  // namespace znet
