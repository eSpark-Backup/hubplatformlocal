#include <znet/znet_node.hpp>
#include <znet/cc_0x85_association.hpp>
#include <znet/cc_0x59_association_grp_info.hpp>
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

class ZCcAssociationGrpInfoSuite : public ::testing::Test, public NodeRequestsManagingFixture {
protected:
    void fillAssociations() {
        auto node = getNodesProvider().onFindNode( 1 );
        node->addCommandClass( 0, ZNET_COMMAND_CLASS_ASSOCIATION, 3 );
        auto cc = node->getCommandClass<ZCc0x85Association>( 0 );

        EXPECT_TRUE( cc->interview( {}, {} ) );

        uint8_t groupsCount = 2;

        node->getRequestsManager().onRespond( AssociationSupportedGroupingRequest( 1, 0 ), groupsCount );

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

        associationReport.grouping_id = 1;
        associationReport.max_nodes_supported = 1;
        associationReport.node_id_count = 1;
        associationReport.node_ids[0] = 1;

        node->getRequestsManager().onRespond( AssociationRequest( 1, 0, 1 ), &associationReport );

        EXPECT_TRUE( cc->hasInterviewDone() );
    }

    void fillMultiChannelAssociations() {
        auto node = getNodesProvider().onFindNode( 1 );
        node->addCommandClass( 0, ZNET_COMMAND_CLASS_MULTI_CHANNEL_ASSOCIATION, 3 );
        auto cc = node->getCommandClass<ZCc0x8eMultiChannelAssociation>( 0 );

        EXPECT_TRUE( cc->interview( {}, {} ) );

        uint8_t groupsCount = 2;

        node->getRequestsManager().onRespond( MultiChannelAssociationSupportedGroupingRequest( 1, 0 ), groupsCount );

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

        associationReport.grouping_id = 1;
        associationReport.max_end_pints_supported = 1;
        associationReport.end_pints_count = 1;
        associationReport.end_pints[0].node_id = 1;
        associationReport.end_pints[0].channel_id = 0;

        node->getRequestsManager().onRespond( MultiChannelAssociationRequest( 1, 0, 1 ), &associationReport );

        EXPECT_TRUE( cc->hasInterviewDone() );
    }

    void runCorrectInterviewTest( std::function<void()> associationsInitializer ) {
        getNodesProvider().nodeCreate( 1 );
        auto node = getNodesProvider().onFindNode( 1 );

        MockFunction<void( RequestFailTypeOpt )> callback;

        node->addCommandClass( 0, ZNET_COMMAND_CLASS_ASSOCIATION_GRP_INFO, 3 );
        auto cc = node->getCommandClass<ZCc0x59AssociationGrpInfo>( 0 );

        associationsInitializer();

        EXPECT_CALL( callback, Call( _ ) ).Times( 0 );

        EXPECT_TRUE( cc->interview( callback.AsStdFunction(), {} ) );

        // First group interview
        uint8_t buffName[sizeof( znet_association_group_name_report_t ) + 255];
        memset( buffName, 0, sizeof( znet_association_group_name_report_t ) + 255 );
        znet_association_group_name_report_t* nameReport = (znet_association_group_name_report_t*)&buffName;

        nameReport->grouping_id = 1;
        memcpy( nameReport->name, "testName1", 9 );
        nameReport->name_size = 9;

        node->getRequestsManager().onRespond( AssociationGrpNameRequest( 1, 0, 1 ), nameReport );
        EXPECT_FALSE( cc->hasInterviewDone() );

        uint8_t buffInfo[sizeof( znet_association_group_info_report_t ) + 255];
        memset( buffInfo, 0, sizeof( znet_association_group_info_report_t ) + 255 );
        znet_association_group_info_report_t* infoReport = (znet_association_group_info_report_t*)&buffInfo;

        infoReport->group_info_count = 1;
        infoReport->group_infos[0].grouping_id = 1;
        infoReport->group_infos[0].profile = 43690;

        node->getRequestsManager().onRespond( AssociationGrpInfoRequest( 1, 0, 1, 0, 1 ), infoReport );
        EXPECT_FALSE( cc->hasInterviewDone() );

        uint8_t listInfo[sizeof( znet_association_group_command_list_report_t ) + 255];
        memset( listInfo, 0, sizeof( znet_association_group_command_list_report_t ) + 255 );
        znet_association_group_command_list_report_t* listReport =
            (znet_association_group_command_list_report_t*)&listInfo;

        listReport->grouping_id = 1;
        listReport->commands_count = 1;
        listReport->commands[0].command = 1;
        listReport->commands[0].sub_command = 2;

        node->getRequestsManager().onRespond( AssociationGrpCommandListRequest( 1, 0, 1, 0 ), listReport );
        EXPECT_FALSE( cc->hasInterviewDone() );

        // Second group interview
        nameReport->grouping_id = 2;
        memcpy( nameReport->name, "testName2", 9 );
        nameReport->name_size = 9;

        node->getRequestsManager().onRespond( AssociationGrpNameRequest( 1, 0, 2 ), nameReport );
        EXPECT_FALSE( cc->hasInterviewDone() );

        infoReport->group_info_count = 1;
        infoReport->group_infos[0].grouping_id = 2;
        infoReport->group_infos[0].profile = 52428;

        node->getRequestsManager().onRespond( AssociationGrpInfoRequest( 1, 0, 2, 0, 1 ), infoReport );
        EXPECT_FALSE( cc->hasInterviewDone() );

        listReport->grouping_id = 2;
        listReport->commands_count = 1;
        listReport->commands[0].command = 2;
        listReport->commands[0].sub_command = 1;

        EXPECT_CALL( callback, Call( RequestFailTypeOpt{} ) ).Times( 1 );
        node->getRequestsManager().onRespond( AssociationGrpCommandListRequest( 1, 0, 2, 0 ), listReport );
        EXPECT_TRUE( cc->hasInterviewDone() );

        EXPECT_EQ( cc->getAssociationGroupsCount(), 2 );

        EXPECT_NE( cc->getAssociationGroupName( 1 ), nullptr );
        EXPECT_EQ( *cc->getAssociationGroupName( 1 ), "testName1" );
        EXPECT_NE( cc->getAssociationGroupProfile( 1 ), nullptr );
        EXPECT_EQ( cc->getAssociationGroupProfile( 1 )->first, 170 );
        EXPECT_EQ( cc->getAssociationGroupProfile( 1 )->second, 170 );
        EXPECT_NE( cc->getAssociationGroupCommands( 1 ), nullptr );
        EXPECT_EQ( cc->getAssociationGroupCommands( 1 )->size(), 1 );
        EXPECT_EQ( cc->getAssociationGroupCommands( 1 )->at( 0 ).first, 1 );
        EXPECT_EQ( cc->getAssociationGroupCommands( 1 )->at( 0 ).second, 2 );

        EXPECT_NE( cc->getAssociationGroupName( 2 ), nullptr );
        EXPECT_EQ( *cc->getAssociationGroupName( 2 ), "testName2" );
        EXPECT_NE( cc->getAssociationGroupProfile( 2 ), nullptr );
        EXPECT_EQ( cc->getAssociationGroupProfile( 2 )->first, 204 );
        EXPECT_EQ( cc->getAssociationGroupProfile( 2 )->second, 204 );
        EXPECT_NE( cc->getAssociationGroupCommands( 2 ), nullptr );
        EXPECT_EQ( cc->getAssociationGroupCommands( 2 )->size(), 1 );
        EXPECT_EQ( cc->getAssociationGroupCommands( 2 )->at( 0 ).first, 2 );
        EXPECT_EQ( cc->getAssociationGroupCommands( 2 )->at( 0 ).second, 1 );
    }
};

TEST_F( ZCcAssociationGrpInfoSuite, interview ) {
    runCorrectInterviewTest( [this]() { fillAssociations(); } );
}

TEST_F( ZCcAssociationGrpInfoSuite, interviewWithMultiChannelAssociations ) {
    runCorrectInterviewTest( [this]() { fillMultiChannelAssociations(); } );
}

TEST_F( ZCcAssociationGrpInfoSuite, interview_Failed ) {
    getNodesProvider().nodeCreate( 1 );
    auto node = getNodesProvider().onFindNode( 1 );

    MockFunction<void( RequestFailTypeOpt )> callback;

    node->addCommandClass( 0, ZNET_COMMAND_CLASS_ASSOCIATION_GRP_INFO, 3 );
    auto cc = node->getCommandClass<ZCc0x59AssociationGrpInfo>( 0 );

    fillAssociations();

    // Name command sending failed
    {
        MockFunction<void( RequestFailTypeOpt )> callback;
        EXPECT_CALL( callback, Call( _ ) ).Times( 0 );
        EXPECT_CALL( callback, Call( RequestFailTypeOpt( RequestFailType::SendingError ) ) ).Times( 1 );

        getRequestsManager().addFailedRequest( AssociationGrpNameRequest( 1, 0, 1 ) );

        EXPECT_TRUE( cc->interview( callback.AsStdFunction(), {} ) );
        getTimerProvider().addTime( DefaultPostponedRequestDelay );

        EXPECT_FALSE( cc->hasInterviewDone() );
    }

    // Name command report failed
    {
        MockFunction<void( RequestFailTypeOpt )> callback;
        EXPECT_CALL( callback, Call( _ ) ).Times( 0 );

        EXPECT_CALL( callback, Call( RequestFailTypeOpt( RequestFailType::ReportedFail ) ) ).Times( 1 );

        EXPECT_TRUE( cc->interview( callback.AsStdFunction(), {} ) );

        node->getRequestsManager().onFail( AssociationGrpNameRequest( 1, 0, 1 ) );

        EXPECT_FALSE( cc->hasInterviewDone() );
    }

    uint8_t buffName[sizeof( znet_association_group_name_report_t ) + 255];
    memset( buffName, 0, sizeof( znet_association_group_name_report_t ) + 255 );
    znet_association_group_name_report_t* nameReport = (znet_association_group_name_report_t*)&buffName;

    nameReport->grouping_id = 1;
    memcpy( nameReport->name, "testName1", 10 );
    nameReport->name_size = 9;

    // Info command sending failed
    {
        MockFunction<void( RequestFailTypeOpt )> callback;
        EXPECT_CALL( callback, Call( _ ) ).Times( 0 );
        EXPECT_CALL( callback, Call( RequestFailTypeOpt( RequestFailType::SendingError ) ) ).Times( 1 );

        getRequestsManager().addFailedRequest( AssociationGrpInfoRequest( 1, 0, 1, 0, 1 ) );

        EXPECT_TRUE( cc->interview( callback.AsStdFunction(), {} ) );
        node->getRequestsManager().onRespond( AssociationGrpNameRequest( 1, 0, 1 ), nameReport );
        getTimerProvider().addTime( DefaultPostponedRequestDelay );

        EXPECT_FALSE( cc->hasInterviewDone() );
    }

    // Info command report failed
    {
        MockFunction<void( RequestFailTypeOpt )> callback;
        EXPECT_CALL( callback, Call( _ ) ).Times( 0 );

        EXPECT_TRUE( cc->interview( callback.AsStdFunction(), {} ) );
        node->getRequestsManager().onRespond( AssociationGrpNameRequest( 1, 0, 1 ), nameReport );

        EXPECT_CALL( callback, Call( RequestFailTypeOpt( RequestFailType::ReportedFail ) ) ).Times( 1 );
        node->getRequestsManager().onFail( AssociationGrpInfoRequest( 1, 0, 1, 0, 1 ) );

        EXPECT_FALSE( cc->hasInterviewDone() );
    }

    uint8_t buffInfo[sizeof( znet_association_group_info_report_t ) + 255];
    memset( buffInfo, 0, sizeof( znet_association_group_info_report_t ) + 255 );
    znet_association_group_info_report_t* infoReport = (znet_association_group_info_report_t*)&buffInfo;

    infoReport->group_info_count = 1;
    infoReport->group_infos[0].grouping_id = 1;
    infoReport->group_infos[0].profile = 43690;

    // List command sending failed
    {
        MockFunction<void( RequestFailTypeOpt )> callback;
        EXPECT_CALL( callback, Call( _ ) ).Times( 0 );
        EXPECT_CALL( callback, Call( RequestFailTypeOpt( RequestFailType::SendingError ) ) ).Times( 1 );

        getRequestsManager().addFailedRequest( AssociationGrpCommandListRequest( 1, 0, 1, 0 ) );

        EXPECT_TRUE( cc->interview( callback.AsStdFunction(), {} ) );
        node->getRequestsManager().onRespond( AssociationGrpNameRequest( 1, 0, 1 ), nameReport );
        node->getRequestsManager().onRespond( AssociationGrpInfoRequest( 1, 0, 1, 0, 1 ), infoReport );
        getTimerProvider().addTime( DefaultPostponedRequestDelay );

        EXPECT_FALSE( cc->hasInterviewDone() );
    }

    // List command report failed
    {
        MockFunction<void( RequestFailTypeOpt )> callback;
        EXPECT_CALL( callback, Call( _ ) ).Times( 0 );

        EXPECT_TRUE( cc->interview( callback.AsStdFunction(), {} ) );
        node->getRequestsManager().onRespond( AssociationGrpNameRequest( 1, 0, 1 ), nameReport );
        node->getRequestsManager().onRespond( AssociationGrpInfoRequest( 1, 0, 1, 0, 1 ), infoReport );

        EXPECT_CALL( callback, Call( RequestFailTypeOpt( RequestFailType::ReportedFail ) ) ).Times( 1 );
        node->getRequestsManager().onFail( AssociationGrpCommandListRequest( 1, 0, 1, 0 ) );

        EXPECT_FALSE( cc->hasInterviewDone() );
    }
}

}  // namespace ut
}  // namespace znet
