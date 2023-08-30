#include "fake_owner_fixture.h"
#include "znet_state_accessor.h"
#include "MainFixture.h"
#include <znet_handler_interview.hpp>
#include <znet/cc_0x84_wakeup.hpp>
#include <gtest/gtest.h>
#include <gmock/gmock.h>

namespace znet {

namespace ut {

using ::testing::_;
using ::testing::ElementsAre;

class ZNetInterviewHandlerTestSuite : public MainFixture, public FakeOwnerFixture, public ZNetStateAccessor {};

TEST_F( ZNetInterviewHandlerTestSuite, fullInterviewFlow ) {
    setSisNodeId( 1 );
    setSelfNodeId( 1 );

    void* callbackArg = &getImpl();
    using Step = InterviewHandlerBase::Step;

    getImpl().activateMode( IZNet::Mode::IncludeStarting );
    const auto& callbacks = getImpl().getCallbacks();
    auto& handler = dynamic_cast<InterviewHandler&>( getImpl().getHandler( 0 ) );

    // add node
    ZNode* node = nullptr;
    {
        znet_nodeinfo_t nodeInfoSource{};
        nodeInfoSource.commands_count = 9;
        nodeInfoSource.node_id = 10;
        nodeInfoSource.security = ZNET_NODEINFO_SECURITY_S0;

        uint8_t buff[sizeof( znet_nodeinfo_t ) + nodeInfoSource.commands_count];
        memcpy( buff, &nodeInfoSource, sizeof( znet_nodeinfo_t ) );
        auto commands = buff + sizeof( znet_nodeinfo_t );
        commands[0] = ZNET_COMMAND_CLASS_MULTI_CHANNEL_ASSOCIATION;
        commands[1] = ZNET_COMMAND_CLASS_ASSOCIATION_GRP_INFO;
        commands[2] = ZNET_COMMAND_CLASS_SWITCH_BINARY;
        commands[3] = ZNET_COMMAND_CLASS_MULTI_CHANNEL;
        commands[4] = ZNET_COMMAND_CLASS_CONFIGURATION;
        commands[5] = ZNET_COMMAND_CLASS_VERSION;
        commands[6] = ZNET_COMMAND_CLASS_ZWAVEPLUS_INFO;
        commands[7] = ZNET_COMMAND_CLASS_WAKE_UP;
        commands[8] = ZNET_COMMAND_CLASS_MANUFACTURER_SPECIFIC;
        auto nodeInfo = reinterpret_cast<const znet_nodeinfo_t*>( buff );

        callbacks.node_add_result( ZNET_ERR_OK, nodeInfo, callbackArg );

        EXPECT_EQ( handler.getCurrentStep(), Step::WaitSecurity );
        ASSERT_NE( getOwner().onFindNode( 10 ), nullptr );
        node = getOwner().onFindNode( 10 );

        EXPECT_EQ( node->getChannelsCount(), 1 );
        EXPECT_THAT( getOwner().getNodeCommandClassesIds( 10, 0 ),
                     ElementsAre( ZNET_COMMAND_CLASS_BASIC,
                                  ZNET_COMMAND_CLASS_CONFIGURATION,
                                  ZNET_COMMAND_CLASS_MULTI_CHANNEL_ASSOCIATION,
                                  ZNET_COMMAND_CLASS_ASSOCIATION_GRP_INFO,
                                  ZNET_COMMAND_CLASS_SWITCH_BINARY,
                                  ZNET_COMMAND_CLASS_MULTI_CHANNEL,
                                  ZNET_COMMAND_CLASS_VERSION,
                                  ZNET_COMMAND_CLASS_ZWAVEPLUS_INFO,
                                  ZNET_COMMAND_CLASS_WAKE_UP,
                                  ZNET_COMMAND_CLASS_MANUFACTURER_SPECIFIC ) );
    }

    // security
    {
        znet_security_supported_report_t sourceReport{};
        sourceReport.commands_count = 2;
        uint8_t buff[sizeof( sourceReport ) + sourceReport.commands_count];
        memcpy( buff, &sourceReport, sizeof( sourceReport ) );
        uint8_t* commands = buff + sizeof( sourceReport );
        commands[0] = ZNET_COMMAND_CLASS_DOOR_LOCK;
        commands[1] = ZNET_COMMAND_CLASS_MARK;
        auto report = reinterpret_cast<const znet_security_supported_report_t*>( buff );

        callbacks.node_cmd_security_supported_result( ZNET_ERR_OK, 10, 0, report, callbackArg );

        EXPECT_EQ( node->getChannelsCount(), 1 );
        EXPECT_THAT( getOwner().getNodeCommandClassesIds( 10, 0 ),
                     ElementsAre( ZNET_COMMAND_CLASS_BASIC,
                                  ZNET_COMMAND_CLASS_CONFIGURATION,
                                  ZNET_COMMAND_CLASS_MULTI_CHANNEL_ASSOCIATION,
                                  ZNET_COMMAND_CLASS_ASSOCIATION_GRP_INFO,
                                  ZNET_COMMAND_CLASS_SWITCH_BINARY,
                                  ZNET_COMMAND_CLASS_MULTI_CHANNEL,
                                  ZNET_COMMAND_CLASS_VERSION,
                                  ZNET_COMMAND_CLASS_ZWAVEPLUS_INFO,
                                  ZNET_COMMAND_CLASS_WAKE_UP,
                                  ZNET_COMMAND_CLASS_MANUFACTURER_SPECIFIC,
                                  ZNET_COMMAND_CLASS_DOOR_LOCK ) );
        EXPECT_EQ( handler.getCurrentStep(), Step::GetCommandClassVersion );
    }

    // versions
    {
        callbacks.node_cmd_command_version_result( ZNET_ERR_OK, 10, 0, ZNET_COMMAND_CLASS_BASIC, 2, callbackArg );
        callbacks.node_cmd_command_version_result(
            ZNET_ERR_OK, 10, 0, ZNET_COMMAND_CLASS_CONFIGURATION, 1, callbackArg );
        callbacks.node_cmd_command_version_result(
            ZNET_ERR_OK, 10, 0, ZNET_COMMAND_CLASS_MULTI_CHANNEL_ASSOCIATION, 1, callbackArg );
        callbacks.node_cmd_command_version_result(
            ZNET_ERR_OK, 10, 0, ZNET_COMMAND_CLASS_ASSOCIATION_GRP_INFO, 1, callbackArg );
        callbacks.node_cmd_command_version_result(
            ZNET_ERR_OK, 10, 0, ZNET_COMMAND_CLASS_SWITCH_BINARY, 2, callbackArg );
        callbacks.node_cmd_command_version_result(
            ZNET_ERR_OK, 10, 0, ZNET_COMMAND_CLASS_MULTI_CHANNEL, 1, callbackArg );
        callbacks.node_cmd_command_version_result( ZNET_ERR_OK, 10, 0, ZNET_COMMAND_CLASS_VERSION, 1, callbackArg );
        callbacks.node_cmd_command_version_result(
            ZNET_ERR_OK, 10, 0, ZNET_COMMAND_CLASS_ZWAVEPLUS_INFO, 1, callbackArg );
        callbacks.node_cmd_command_version_result( ZNET_ERR_OK, 10, 0, ZNET_COMMAND_CLASS_WAKE_UP, 2, callbackArg );
        callbacks.node_cmd_command_version_result(
            ZNET_ERR_OK, 10, 0, ZNET_COMMAND_CLASS_MANUFACTURER_SPECIFIC, 4, callbackArg );
        callbacks.node_cmd_command_version_result( ZNET_ERR_OK, 10, 0, ZNET_COMMAND_CLASS_DOOR_LOCK, 5, callbackArg );

        EXPECT_EQ( handler.getCurrentStep(), Step::GetZWavePlusInfo );
        EXPECT_EQ( node->getCommandClass( 0, ZNET_COMMAND_CLASS_BASIC )->getVersion(), 2 );
        EXPECT_EQ( node->getCommandClass( 0, ZNET_COMMAND_CLASS_SWITCH_BINARY )->getVersion(), 2 );
        EXPECT_EQ( node->getCommandClass( 0, ZNET_COMMAND_CLASS_MULTI_CHANNEL )->getVersion(), 1 );
        EXPECT_EQ( node->getCommandClass( 0, ZNET_COMMAND_CLASS_CONFIGURATION )->getVersion(), 1 );
        EXPECT_EQ( node->getCommandClass( 0, ZNET_COMMAND_CLASS_MULTI_CHANNEL_ASSOCIATION )->getVersion(), 1 );
        EXPECT_EQ( node->getCommandClass( 0, ZNET_COMMAND_CLASS_ASSOCIATION_GRP_INFO )->getVersion(), 1 );
        EXPECT_EQ( node->getCommandClass( 0, ZNET_COMMAND_CLASS_VERSION )->getVersion(), 1 );
        EXPECT_EQ( node->getCommandClass( 0, ZNET_COMMAND_CLASS_ZWAVEPLUS_INFO )->getVersion(), 1 );
        EXPECT_EQ( node->getCommandClass( 0, ZNET_COMMAND_CLASS_WAKE_UP )->getVersion(), 2 );
        EXPECT_EQ( node->getCommandClass( 0, ZNET_COMMAND_CLASS_MANUFACTURER_SPECIFIC )->getVersion(), 4 );
        EXPECT_EQ( node->getCommandClass( 0, ZNET_COMMAND_CLASS_DOOR_LOCK )->getVersion(), 5 );
    }

    // zwave plus
    {
        znet_zwaveplus_info_report_t report{};
        report.role_type = 1;
        report.node_type = 2;
        report.installer_icon = 3;
        report.user_icon = 4;

        callbacks.node_cmd_zwaveplus_info_result( ZNET_ERR_OK, 10, &report, callbackArg );

        EXPECT_EQ( handler.getCurrentStep(), Step::WakeUpCapabilities );
        EXPECT_EQ( node->getRoleType(), 1 );
        EXPECT_EQ( node->getZwavePlusIconType(), 3 );
    }

    // wake up
    {
        znet_wakeup_capabilities_report_t report{};
        report.step_wakeup_interval = 100;
        report.default_wakeup_interval = 200;
        report.min_wakeup_interval = 50;
        report.max_wakeup_interval = 50000;

        callbacks.node_cmd_wakeup_capabilities_result( ZNET_ERR_OK, 10, 0, &report, callbackArg );

        EXPECT_EQ( handler.getCurrentStep(), Step::MultichannelAssociation );
    }

    // multi channel associations
    {
        uint8_t groupsCount = 1;
        callbacks.node_cmd_multichannel_association_supported_grouping_result(
            ZNET_ERR_OK, 10, 0, groupsCount, callbackArg );

        znet_multichannel_association_report_t associationReport;
        memset( &associationReport, 0, sizeof( associationReport ) );
        associationReport.grouping_id = 1;
        associationReport.max_end_pints_supported = 1;

        callbacks.node_cmd_multichannel_association_result( ZNET_ERR_OK, 10, 0, 1, &associationReport, callbackArg );

        callbacks.node_cmd_multichannel_association_set_done( ZNET_ERR_OK, 10, 0, 1, callbackArg );

        associationReport.grouping_id = 1;
        associationReport.max_end_pints_supported = 1;
        associationReport.end_pints_count = 1;
        associationReport.end_pints[0].node_id = 1;
        associationReport.end_pints[0].channel_id = 0;

        callbacks.node_cmd_multichannel_association_result( ZNET_ERR_OK, 10, 0, 1, &associationReport, callbackArg );
        EXPECT_EQ( handler.getCurrentStep(), Step::GetManufactureSpecific );
    }

    // manufacturer specific
    {
        znet_manufacturer_specific_report_t report{};
        report.manufacturer_id = 1;
        report.product_id = 2;
        report.product_type = 3;

        callbacks.node_cmd_manufacturer_specific_result( ZNET_ERR_OK, 10, 0, &report, callbackArg );

        EXPECT_EQ( handler.getCurrentStep(), Step::AssociationGroupInfo );
        EXPECT_EQ( node->getManufacturerId(), 1 );
        EXPECT_EQ( node->getProductId(), 2 );
        EXPECT_EQ( node->getProductType(), 3 );
    }

    // Association group info
    {
        uint8_t buffName[sizeof( znet_association_group_name_report_t ) + 255];
        memset( buffName, 0, sizeof( znet_association_group_name_report_t ) + 255 );
        znet_association_group_name_report_t* nameReport = (znet_association_group_name_report_t*)&buffName;

        nameReport->grouping_id = 1;
        memcpy( nameReport->name, "testName1", 9 );
        nameReport->name_size = 9;

        callbacks.node_cmd_association_group_name_result( ZNET_ERR_OK, 10, 0, 1, nameReport, callbackArg );

        uint8_t buffInfo[sizeof( znet_association_group_info_report_t ) + 255];
        memset( buffInfo, 0, sizeof( znet_association_group_info_report_t ) + 255 );
        znet_association_group_info_report_t* infoReport = (znet_association_group_info_report_t*)&buffInfo;

        infoReport->group_info_count = 1;
        infoReport->group_infos[0].grouping_id = 1;
        infoReport->group_infos[0].profile = 43690;

        callbacks.node_cmd_association_group_info_result( ZNET_ERR_OK, 10, 0, 1, infoReport, callbackArg );

        uint8_t listInfo[sizeof( znet_association_group_command_list_report_t ) + 255];
        memset( listInfo, 0, sizeof( znet_association_group_command_list_report_t ) + 255 );
        znet_association_group_command_list_report_t* listReport =
            (znet_association_group_command_list_report_t*)&listInfo;

        listReport->grouping_id = 1;
        listReport->commands_count = 1;
        listReport->commands[0].command = 1;
        listReport->commands[0].sub_command = 2;

        callbacks.node_cmd_association_group_command_list_result( ZNET_ERR_OK, 10, 0, 1, listReport, callbackArg );

        EXPECT_EQ( handler.getCurrentStep(), Step::MultichannelEndpointSurvey );
    }

    // build channels
    {
        znet_multichannel_endpoint_report_t endpointReport{};
        endpointReport.channel_ids_count = 1;

        callbacks.node_cmd_multichannel_endpoint_result( ZNET_ERR_OK, 10, &endpointReport, callbackArg );

        znet_multichannel_endpoint_find_report_t findReport{};
        findReport.reports_to_follow = 0;
        findReport.generic = 0xFF;
        findReport.specific = 0xFF;
        findReport.channel_ids_count = 0;

        callbacks.node_cmd_multichannel_endpoint_find_result( ZNET_ERR_OK, 10, &findReport, callbackArg );

        znet_command_class_t classes1[] = { ZNET_COMMAND_CLASS_MULTI_CHANNEL_ASSOCIATION,
                                            ZNET_COMMAND_CLASS_ASSOCIATION_GRP_INFO,
                                            ZNET_COMMAND_CLASS_SENSOR_ALARM };
        znet_multichannel_capability_report_t capabilityReport1{};
        capabilityReport1.commands_count = 3;
        capabilityReport1.channel_id = 1;
        capabilityReport1.commands = classes1;

        znet_security_supported_report_t securitySourceReport1{};
        securitySourceReport1.commands_count = 1;
        uint8_t buff1[sizeof( securitySourceReport1 ) + securitySourceReport1.commands_count];
        memcpy( buff1, &securitySourceReport1, sizeof( securitySourceReport1 ) );
        uint8_t* securityCommands1 = buff1 + sizeof( securitySourceReport1 );
        securityCommands1[0] = ZNET_COMMAND_CLASS_DOOR_LOCK;

        auto securityReport1 = reinterpret_cast<const znet_security_supported_report_t*>( buff1 );

        callbacks.node_cmd_security_supported_result( ZNET_ERR_OK, 10, 1, securityReport1, callbackArg );
        callbacks.node_cmd_multichannel_capability_result( ZNET_ERR_OK, 10, 1, &capabilityReport1, callbackArg );

        EXPECT_EQ( handler.getCurrentStep(), Step::CommandClassesProcessing );
        EXPECT_EQ( node->getChannelsCount(), 2 );
        EXPECT_THAT( getOwner().getNodeCommandClassesIds( 10, 0 ),
                     ElementsAre( ZNET_COMMAND_CLASS_BASIC,
                                  ZNET_COMMAND_CLASS_CONFIGURATION,
                                  ZNET_COMMAND_CLASS_MULTI_CHANNEL_ASSOCIATION,
                                  ZNET_COMMAND_CLASS_ASSOCIATION_GRP_INFO,
                                  ZNET_COMMAND_CLASS_SWITCH_BINARY,
                                  ZNET_COMMAND_CLASS_MULTI_CHANNEL,
                                  ZNET_COMMAND_CLASS_VERSION,
                                  ZNET_COMMAND_CLASS_ZWAVEPLUS_INFO,
                                  ZNET_COMMAND_CLASS_WAKE_UP,
                                  ZNET_COMMAND_CLASS_MANUFACTURER_SPECIFIC,
                                  ZNET_COMMAND_CLASS_DOOR_LOCK ) );
        EXPECT_THAT( getOwner().getNodeCommandClassesIds( 10, 1 ),
                     ElementsAre( ZNET_COMMAND_CLASS_BASIC,
                                  ZNET_COMMAND_CLASS_DOOR_LOCK,
                                  ZNET_COMMAND_CLASS_MULTI_CHANNEL_ASSOCIATION,
                                  ZNET_COMMAND_CLASS_ASSOCIATION_GRP_INFO,
                                  ZNET_COMMAND_CLASS_SENSOR_ALARM ) );
    }

    // process command classes, setup node settings
    {
        EXPECT_EQ( node->getCommandClass<ZCc0x84WakeUp>( 0 )->getInterval(), 0 );

        callbacks.node_cmd_basic_result( ZNET_ERR_OK, 10, 0, 0, 0, 0, callbackArg );

        znet_version_report_t versionReport{};
        callbacks.node_cmd_version_result( ZNET_ERR_OK, 10, &versionReport, callbackArg );

        znet_door_lock_capabilities_t doorlockCapabilities{};
        callbacks.node_cmd_door_lock_capabilities_result( ZNET_ERR_OK, 10, 0, &doorlockCapabilities, callbackArg );

        callbacks.node_cmd_basic_result( ZNET_ERR_OK, 10, 1, 0, 0, 0, callbackArg );
        callbacks.node_cmd_door_lock_capabilities_result( ZNET_ERR_OK, 10, 1, &doorlockCapabilities, callbackArg );

        // multi channel associations ch1
        {
            uint8_t groupsCount = 1;
            callbacks.node_cmd_multichannel_association_supported_grouping_result(
                ZNET_ERR_OK, 10, 1, groupsCount, callbackArg );

            znet_multichannel_association_report_t associationReport;
            memset( &associationReport, 0, sizeof( associationReport ) );
            associationReport.grouping_id = 1;
            associationReport.max_end_pints_supported = 1;
            associationReport.end_pints_count = 1;
            associationReport.end_pints[0].node_id = 1;
            associationReport.end_pints[0].channel_id = 0;

            callbacks.node_cmd_multichannel_association_result(
                ZNET_ERR_OK, 10, 1, 1, &associationReport, callbackArg );
        }

        // association group info ch1
        {
            uint8_t buffName[sizeof( znet_association_group_name_report_t ) + 255];
            memset( buffName, 0, sizeof( znet_association_group_name_report_t ) + 255 );
            znet_association_group_name_report_t* nameReport = (znet_association_group_name_report_t*)&buffName;

            nameReport->grouping_id = 1;
            memcpy( nameReport->name, "testName2", 9 );
            nameReport->name_size = 9;

            callbacks.node_cmd_association_group_name_result( ZNET_ERR_OK, 10, 1, 1, nameReport, callbackArg );

            uint8_t buffInfo[sizeof( znet_association_group_info_report_t ) + 255];
            memset( buffInfo, 0, sizeof( znet_association_group_info_report_t ) + 255 );
            znet_association_group_info_report_t* infoReport = (znet_association_group_info_report_t*)&buffInfo;

            infoReport->group_info_count = 1;
            infoReport->group_infos[0].grouping_id = 1;
            infoReport->group_infos[0].profile = 43690;

            callbacks.node_cmd_association_group_info_result( ZNET_ERR_OK, 10, 1, 1, infoReport, callbackArg );

            uint8_t listInfo[sizeof( znet_association_group_command_list_report_t ) + 255];
            memset( listInfo, 0, sizeof( znet_association_group_command_list_report_t ) + 255 );
            znet_association_group_command_list_report_t* listReport =
                (znet_association_group_command_list_report_t*)&listInfo;

            listReport->grouping_id = 1;
            listReport->commands_count = 1;
            listReport->commands[0].command = 1;
            listReport->commands[0].sub_command = 2;

            callbacks.node_cmd_association_group_command_list_result( ZNET_ERR_OK, 10, 1, 1, listReport, callbackArg );
        }

        callbacks.node_cmd_command_version_result(
            ZNET_ERR_OK, 10, 0, ZNET_COMMAND_CLASS_SENSOR_ALARM, 3, callbackArg );

        uint8_t mask = 3;
        callbacks.node_cmd_alarm_sensor_get_supported_sensor_result( ZNET_ERR_OK, 10, 1, &mask, 1, callbackArg );

        EXPECT_EQ( node->getCommandClass( 1, ZNET_COMMAND_CLASS_DOOR_LOCK )->getVersion(), 5 );
        EXPECT_EQ( node->getCommandClass( 1, ZNET_COMMAND_CLASS_SENSOR_ALARM )->getVersion(), 3 );

        EXPECT_EQ( node->getCommandClass<ZCc0x84WakeUp>( 0 )->getInterval(), ZCc0x84WakeUp::DefaultWakeUpInterval );
        EXPECT_EQ( node->getWakeUpInterval(), utils::optional<uint32_t>( ZCc0x84WakeUp::DefaultWakeUpInterval ) );
    }

    EXPECT_EQ( handler.getCurrentStep(), Step::Ready );
}

TEST_F( ZNetInterviewHandlerTestSuite, interview_Associations ) {
    setSisNodeId( 1 );
    setSelfNodeId( 1 );

    void* callbackArg = &getImpl();
    using Step = InterviewHandlerBase::Step;

    getImpl().activateMode( IZNet::Mode::IncludeStarting );
    const auto& callbacks = getImpl().getCallbacks();
    auto& handler = dynamic_cast<InterviewHandler&>( getImpl().getHandler( 0 ) );

    // add node
    ZNode* node = nullptr;
    {
        znet_nodeinfo_t nodeInfoSource{};
        nodeInfoSource.commands_count = 8;
        nodeInfoSource.node_id = 10;
        nodeInfoSource.security = ZNET_NODEINFO_SECURITY_NONE;

        uint8_t buff[sizeof( znet_nodeinfo_t ) + nodeInfoSource.commands_count];
        memcpy( buff, &nodeInfoSource, sizeof( znet_nodeinfo_t ) );
        auto commands = buff + sizeof( znet_nodeinfo_t );
        commands[0] = ZNET_COMMAND_CLASS_ASSOCIATION;
        commands[1] = ZNET_COMMAND_CLASS_ASSOCIATION_GRP_INFO;
        commands[2] = ZNET_COMMAND_CLASS_SWITCH_BINARY;
        commands[3] = ZNET_COMMAND_CLASS_MULTI_CHANNEL;
        commands[4] = ZNET_COMMAND_CLASS_CONFIGURATION;
        commands[5] = ZNET_COMMAND_CLASS_VERSION;
        commands[6] = ZNET_COMMAND_CLASS_ZWAVEPLUS_INFO;
        commands[7] = ZNET_COMMAND_CLASS_MANUFACTURER_SPECIFIC;
        auto nodeInfo = reinterpret_cast<const znet_nodeinfo_t*>( buff );

        callbacks.node_add_result( ZNET_ERR_OK, nodeInfo, callbackArg );

        EXPECT_EQ( handler.getCurrentStep(), Step::GetCommandClassVersion );
        ASSERT_NE( getOwner().onFindNode( 10 ), nullptr );
        node = getOwner().onFindNode( 10 );

        EXPECT_EQ( node->getChannelsCount(), 1 );
        EXPECT_THAT( getOwner().getNodeCommandClassesIds( 10, 0 ),
                     ElementsAre( ZNET_COMMAND_CLASS_BASIC,
                                  ZNET_COMMAND_CLASS_CONFIGURATION,
                                  ZNET_COMMAND_CLASS_ASSOCIATION,
                                  ZNET_COMMAND_CLASS_ASSOCIATION_GRP_INFO,
                                  ZNET_COMMAND_CLASS_SWITCH_BINARY,
                                  ZNET_COMMAND_CLASS_MULTI_CHANNEL,
                                  ZNET_COMMAND_CLASS_VERSION,
                                  ZNET_COMMAND_CLASS_ZWAVEPLUS_INFO,
                                  ZNET_COMMAND_CLASS_MANUFACTURER_SPECIFIC ) );
    }

    // versions
    {
        callbacks.node_cmd_command_version_result( ZNET_ERR_OK, 10, 0, ZNET_COMMAND_CLASS_BASIC, 2, callbackArg );
        callbacks.node_cmd_command_version_result(
            ZNET_ERR_OK, 10, 0, ZNET_COMMAND_CLASS_CONFIGURATION, 1, callbackArg );
        callbacks.node_cmd_command_version_result( ZNET_ERR_OK, 10, 0, ZNET_COMMAND_CLASS_ASSOCIATION, 1, callbackArg );
        callbacks.node_cmd_command_version_result(
            ZNET_ERR_OK, 10, 0, ZNET_COMMAND_CLASS_ASSOCIATION_GRP_INFO, 1, callbackArg );
        callbacks.node_cmd_command_version_result(
            ZNET_ERR_OK, 10, 0, ZNET_COMMAND_CLASS_SWITCH_BINARY, 2, callbackArg );
        callbacks.node_cmd_command_version_result(
            ZNET_ERR_OK, 10, 0, ZNET_COMMAND_CLASS_MULTI_CHANNEL, 1, callbackArg );
        callbacks.node_cmd_command_version_result( ZNET_ERR_OK, 10, 0, ZNET_COMMAND_CLASS_VERSION, 1, callbackArg );
        callbacks.node_cmd_command_version_result(
            ZNET_ERR_OK, 10, 0, ZNET_COMMAND_CLASS_ZWAVEPLUS_INFO, 1, callbackArg );
        callbacks.node_cmd_command_version_result(
            ZNET_ERR_OK, 10, 0, ZNET_COMMAND_CLASS_MANUFACTURER_SPECIFIC, 4, callbackArg );

        EXPECT_EQ( handler.getCurrentStep(), Step::GetZWavePlusInfo );
    }

    // zwave plus
    {
        znet_zwaveplus_info_report_t report{};
        report.role_type = 1;
        report.node_type = 2;
        report.installer_icon = 3;
        report.user_icon = 4;

        callbacks.node_cmd_zwaveplus_info_result( ZNET_ERR_OK, 10, &report, callbackArg );

        EXPECT_EQ( handler.getCurrentStep(), Step::Association );
    }

    // Associations
    {
        uint8_t groupsCount = 1;
        callbacks.node_cmd_association_supported_grouping_result( ZNET_ERR_OK, 10, 0, groupsCount, callbackArg );

        znet_association_report_t associationReport;
        memset( &associationReport, 0, sizeof( associationReport ) );
        associationReport.grouping_id = 1;
        associationReport.max_nodes_supported = 1;
        callbacks.node_cmd_association_result( ZNET_ERR_OK, 10, 0, 1, &associationReport, callbackArg );

        callbacks.node_cmd_association_set_done( ZNET_ERR_OK, 10, 0, 1, callbackArg );

        associationReport.grouping_id = 1;
        associationReport.max_nodes_supported = 1;
        associationReport.node_id_count = 1;
        associationReport.node_ids[0] = 1;

        callbacks.node_cmd_association_result( ZNET_ERR_OK, 10, 0, 1, &associationReport, callbackArg );

        EXPECT_EQ( handler.getCurrentStep(), Step::GetManufactureSpecific );
    }

    // manufacturer specific
    {
        znet_manufacturer_specific_report_t report{};
        report.manufacturer_id = 1;
        report.product_id = 2;
        report.product_type = 3;

        callbacks.node_cmd_manufacturer_specific_result( ZNET_ERR_OK, 10, 0, &report, callbackArg );

        EXPECT_EQ( handler.getCurrentStep(), Step::AssociationGroupInfo );
    }

    // Association group info
    {
        uint8_t buffName[sizeof( znet_association_group_name_report_t ) + 255];
        memset( buffName, 0, sizeof( znet_association_group_name_report_t ) + 255 );
        znet_association_group_name_report_t* nameReport = (znet_association_group_name_report_t*)&buffName;

        nameReport->grouping_id = 1;
        memcpy( nameReport->name, "testName1", 9 );
        nameReport->name_size = 9;

        callbacks.node_cmd_association_group_name_result( ZNET_ERR_OK, 10, 0, 1, nameReport, callbackArg );

        uint8_t buffInfo[sizeof( znet_association_group_info_report_t ) + 255];
        memset( buffInfo, 0, sizeof( znet_association_group_info_report_t ) + 255 );
        znet_association_group_info_report_t* infoReport = (znet_association_group_info_report_t*)&buffInfo;

        infoReport->group_info_count = 1;
        infoReport->group_infos[0].grouping_id = 1;
        infoReport->group_infos[0].profile = 43690;

        callbacks.node_cmd_association_group_info_result( ZNET_ERR_OK, 10, 0, 1, infoReport, callbackArg );

        uint8_t listInfo[sizeof( znet_association_group_command_list_report_t ) + 255];
        memset( listInfo, 0, sizeof( znet_association_group_command_list_report_t ) + 255 );
        znet_association_group_command_list_report_t* listReport =
            (znet_association_group_command_list_report_t*)&listInfo;

        listReport->grouping_id = 1;
        listReport->commands_count = 1;
        listReport->commands[0].command = 1;
        listReport->commands[0].sub_command = 2;

        callbacks.node_cmd_association_group_command_list_result( ZNET_ERR_OK, 10, 0, 1, listReport, callbackArg );

        EXPECT_EQ( handler.getCurrentStep(), Step::MultichannelEndpointSurvey );
    }

    // build channels
    {
        znet_multichannel_endpoint_report_t endpointReport{};
        endpointReport.channel_ids_count = 1;

        callbacks.node_cmd_multichannel_endpoint_result( ZNET_ERR_OK, 10, &endpointReport, callbackArg );

        znet_multichannel_endpoint_find_report_t findReport{};
        findReport.reports_to_follow = 0;
        findReport.generic = 0xFF;
        findReport.specific = 0xFF;
        findReport.channel_ids_count = 0;

        callbacks.node_cmd_multichannel_endpoint_find_result( ZNET_ERR_OK, 10, &findReport, callbackArg );

        znet_command_class_t classes1[] = {
            ZNET_COMMAND_CLASS_ASSOCIATION, ZNET_COMMAND_CLASS_ASSOCIATION_GRP_INFO, ZNET_COMMAND_CLASS_SENSOR_ALARM
        };
        znet_multichannel_capability_report_t capabilityReport1{};
        capabilityReport1.commands_count = 3;
        capabilityReport1.channel_id = 1;
        capabilityReport1.commands = classes1;

        callbacks.node_cmd_multichannel_capability_result( ZNET_ERR_OK, 10, 1, &capabilityReport1, callbackArg );

        EXPECT_EQ( handler.getCurrentStep(), Step::CommandClassesProcessing );
        EXPECT_EQ( node->getChannelsCount(), 2 );

        EXPECT_THAT( getOwner().getNodeCommandClassesIds( 10, 1 ),
                     ElementsAre( ZNET_COMMAND_CLASS_BASIC,
                                  ZNET_COMMAND_CLASS_ASSOCIATION,
                                  ZNET_COMMAND_CLASS_ASSOCIATION_GRP_INFO,
                                  ZNET_COMMAND_CLASS_SENSOR_ALARM ) );
    }

    // process command classes, setup node settings
    {
        callbacks.node_cmd_basic_result( ZNET_ERR_OK, 10, 0, 0, 0, 0, callbackArg );

        znet_version_report_t versionReport{};
        callbacks.node_cmd_version_result( ZNET_ERR_OK, 10, &versionReport, callbackArg );

        callbacks.node_cmd_basic_result( ZNET_ERR_OK, 10, 1, 0, 0, 0, callbackArg );

        // Associations ch1
        {
            uint8_t groupsCount = 1;
            callbacks.node_cmd_association_supported_grouping_result( ZNET_ERR_OK, 10, 1, groupsCount, callbackArg );

            znet_association_report_t associationReport;
            memset( &associationReport, 0, sizeof( associationReport ) );
            associationReport.grouping_id = 1;
            associationReport.max_nodes_supported = 1;
            callbacks.node_cmd_association_result( ZNET_ERR_OK, 10, 1, 1, &associationReport, callbackArg );
        }

        // Association group info ch1
        {
            uint8_t buffName[sizeof( znet_association_group_name_report_t ) + 255];
            memset( buffName, 0, sizeof( znet_association_group_name_report_t ) + 255 );
            znet_association_group_name_report_t* nameReport = (znet_association_group_name_report_t*)&buffName;

            nameReport->grouping_id = 1;
            memcpy( nameReport->name, "testName2", 9 );
            nameReport->name_size = 9;

            callbacks.node_cmd_association_group_name_result( ZNET_ERR_OK, 10, 1, 1, nameReport, callbackArg );

            uint8_t buffInfo[sizeof( znet_association_group_info_report_t ) + 255];
            memset( buffInfo, 0, sizeof( znet_association_group_info_report_t ) + 255 );
            znet_association_group_info_report_t* infoReport = (znet_association_group_info_report_t*)&buffInfo;

            infoReport->group_info_count = 1;
            infoReport->group_infos[0].grouping_id = 1;
            infoReport->group_infos[0].profile = 43690;

            callbacks.node_cmd_association_group_info_result( ZNET_ERR_OK, 10, 1, 1, infoReport, callbackArg );

            uint8_t listInfo[sizeof( znet_association_group_command_list_report_t ) + 255];
            memset( listInfo, 0, sizeof( znet_association_group_command_list_report_t ) + 255 );
            znet_association_group_command_list_report_t* listReport =
                (znet_association_group_command_list_report_t*)&listInfo;

            listReport->grouping_id = 1;
            listReport->commands_count = 1;
            listReport->commands[0].command = 1;
            listReport->commands[0].sub_command = 2;

            callbacks.node_cmd_association_group_command_list_result( ZNET_ERR_OK, 10, 1, 1, listReport, callbackArg );
        }

        callbacks.node_cmd_command_version_result(
            ZNET_ERR_OK, 10, 0, ZNET_COMMAND_CLASS_SENSOR_ALARM, 3, callbackArg );

        uint8_t mask = 3;
        callbacks.node_cmd_alarm_sensor_get_supported_sensor_result( ZNET_ERR_OK, 10, 1, &mask, 1, callbackArg );
    }

    EXPECT_EQ( handler.getCurrentStep(), Step::Ready );
}

TEST_F( ZNetInterviewHandlerTestSuite, wrongNodeInfo ) {
    setSisNodeId( 1 );
    setSelfNodeId( 1 );

    void* callbackArg = &getImpl();
    using Step = InterviewHandlerBase::Step;

    getImpl().activateMode( IZNet::Mode::IncludeStarting );
    const auto& callbacks = getImpl().getCallbacks();
    auto& handler = dynamic_cast<InterviewHandler&>( getImpl().getHandler( 0 ) );

    const znet_node_id_t expectedNodeId = 10;

    auto sendNodeInfo = [&]( znet_node_id_t nodeId ) {
        znet_nodeinfo_t nodeInfoSource{};
        nodeInfoSource.commands_count = 4;
        nodeInfoSource.node_id = nodeId;
        nodeInfoSource.security = ZNET_NODEINFO_SECURITY_S2_ACCESS;

        uint8_t buff[sizeof( znet_nodeinfo_t ) + nodeInfoSource.commands_count];
        memcpy( buff, &nodeInfoSource, sizeof( znet_nodeinfo_t ) );
        auto commands = buff + sizeof( znet_nodeinfo_t );
        commands[0] = expectedNodeId == nodeId ? ZNET_COMMAND_CLASS_SWITCH_BINARY : ZNET_COMMAND_CLASS_SWITCH_COLOR;
        commands[1] = ZNET_COMMAND_CLASS_CONFIGURATION;
        commands[2] = ZNET_COMMAND_CLASS_VERSION;
        commands[3] = ZNET_COMMAND_CLASS_MANUFACTURER_SPECIFIC;
        auto nodeInfo = reinterpret_cast<const znet_nodeinfo_t*>( buff );

        callbacks.node_add_result( ZNET_ERR_OK, nodeInfo, callbackArg );
    };

    auto sendSecuritySupported = [&]( znet_node_id_t nodeId ) {
        znet_security_supported_report_t sourceReport{};
        sourceReport.commands_count = 2;
        uint8_t buff[sizeof( sourceReport ) + sourceReport.commands_count];
        memcpy( buff, &sourceReport, sizeof( sourceReport ) );
        uint8_t* commands = buff + sizeof( sourceReport );
        commands[0] = nodeId == expectedNodeId ? ZNET_COMMAND_CLASS_DOOR_LOCK : ZNET_COMMAND_CLASS_THERMOSTAT_MODE;
        commands[1] = ZNET_COMMAND_CLASS_MARK;
        auto report = reinterpret_cast<const znet_security_supported_report_t*>( buff );

        callbacks.node_cmd_security_supported_result( ZNET_ERR_OK, nodeId, 0, report, callbackArg );
    };

    // add node
    ZNode* node = nullptr;
    {
        sendNodeInfo( expectedNodeId );

        sendNodeInfo( expectedNodeId );  // once again

        sendNodeInfo( 11 );  // wrong

        EXPECT_EQ( handler.getCurrentStep(), Step::WaitSecurity );
        ASSERT_NE( getOwner().onFindNode( expectedNodeId ), nullptr );
        node = getOwner().onFindNode( expectedNodeId );

        EXPECT_EQ( node->getChannelsCount(), 1 );
        EXPECT_THAT( getOwner().getNodeCommandClassesIds( expectedNodeId, 0 ),
                     ElementsAre( ZNET_COMMAND_CLASS_BASIC,
                                  ZNET_COMMAND_CLASS_CONFIGURATION,
                                  ZNET_COMMAND_CLASS_SWITCH_BINARY,
                                  ZNET_COMMAND_CLASS_VERSION,
                                  ZNET_COMMAND_CLASS_MANUFACTURER_SPECIFIC ) );
    }

    // security
    {
        sendSecuritySupported( 11 );  // wrong

        sendNodeInfo( 11 );  // wrong

        sendSecuritySupported( expectedNodeId );

        sendNodeInfo( 11 );  // wrong

        sendSecuritySupported( 11 );  // wrong

        EXPECT_EQ( node->getChannelsCount(), 1 );
        EXPECT_THAT( getOwner().getNodeCommandClassesIds( expectedNodeId, 0 ),
                     ElementsAre( ZNET_COMMAND_CLASS_BASIC,
                                  ZNET_COMMAND_CLASS_CONFIGURATION,
                                  ZNET_COMMAND_CLASS_SWITCH_BINARY,
                                  ZNET_COMMAND_CLASS_VERSION,
                                  ZNET_COMMAND_CLASS_MANUFACTURER_SPECIFIC,
                                  ZNET_COMMAND_CLASS_DOOR_LOCK ) );
        EXPECT_EQ( handler.getCurrentStep(), Step::GetCommandClassVersion );
    }
}

struct WakeUpIntervalParams {
    znet_node_id_t selfNodeId;
    znet_node_id_t sisNodeId;
    utils::optional<uint32_t> expectedInterval;
    bool isWokenUp;
};

class ZNetInterviewHandlerCheckWakeUpIntervalTestSuite : public ZNetInterviewHandlerTestSuite,
                                                         public ::testing::WithParamInterface<WakeUpIntervalParams> {};

TEST_P( ZNetInterviewHandlerCheckWakeUpIntervalTestSuite, checkWakeUpinterval ) {
    const WakeUpIntervalParams& param = GetParam();

    setSisNodeId( param.sisNodeId );
    setSelfNodeId( param.selfNodeId );

    void* callbackArg = &getImpl();
    getImpl().activateMode( IZNet::Mode::IncludeStarting );
    const auto& callbacks = getImpl().getCallbacks();

    // add node
    ZNode* node = nullptr;
    {
        znet_nodeinfo_t nodeInfoSource{};
        nodeInfoSource.commands_count = 5;
        nodeInfoSource.node_id = 10;
        nodeInfoSource.security = ZNET_NODEINFO_SECURITY_NONE;

        uint8_t buff[sizeof( znet_nodeinfo_t ) + nodeInfoSource.commands_count];
        memcpy( buff, &nodeInfoSource, sizeof( znet_nodeinfo_t ) );
        auto commands = buff + sizeof( znet_nodeinfo_t );
        commands[0] = ZNET_COMMAND_CLASS_SWITCH_BINARY;
        commands[1] = ZNET_COMMAND_CLASS_CONFIGURATION;
        commands[2] = ZNET_COMMAND_CLASS_VERSION;
        commands[3] = ZNET_COMMAND_CLASS_WAKE_UP;
        commands[4] = ZNET_COMMAND_CLASS_MANUFACTURER_SPECIFIC;
        auto nodeInfo = reinterpret_cast<const znet_nodeinfo_t*>( buff );

        callbacks.node_add_result( ZNET_ERR_OK, nodeInfo, callbackArg );

        ASSERT_NE( getOwner().onFindNode( 10 ), nullptr );
        node = getOwner().onFindNode( 10 );
    }

    // versions
    {
        callbacks.node_cmd_command_version_result( ZNET_ERR_OK, 10, 0, ZNET_COMMAND_CLASS_BASIC, 2, callbackArg );
        callbacks.node_cmd_command_version_result(
            ZNET_ERR_OK, 10, 0, ZNET_COMMAND_CLASS_CONFIGURATION, 1, callbackArg );
        callbacks.node_cmd_command_version_result(
            ZNET_ERR_OK, 10, 0, ZNET_COMMAND_CLASS_SWITCH_BINARY, 2, callbackArg );
        callbacks.node_cmd_command_version_result( ZNET_ERR_OK, 10, 0, ZNET_COMMAND_CLASS_VERSION, 1, callbackArg );
        callbacks.node_cmd_command_version_result( ZNET_ERR_OK, 10, 0, ZNET_COMMAND_CLASS_WAKE_UP, 2, callbackArg );
        callbacks.node_cmd_command_version_result(
            ZNET_ERR_OK, 10, 0, ZNET_COMMAND_CLASS_MANUFACTURER_SPECIFIC, 4, callbackArg );
    }

    // wake up
    {
        znet_wakeup_capabilities_report_t report{};
        report.step_wakeup_interval = 100;
        report.default_wakeup_interval = 200;
        report.min_wakeup_interval = 50;
        report.max_wakeup_interval = 50000;

        callbacks.node_cmd_wakeup_capabilities_result( ZNET_ERR_OK, 10, 0, &report, callbackArg );
    }

    // manufacturer specific
    {
        znet_manufacturer_specific_report_t report{};
        callbacks.node_cmd_manufacturer_specific_result( ZNET_ERR_OK, 10, 0, &report, callbackArg );
    }

    // process command classes, setup node settings
    {
        EXPECT_EQ( node->getCommandClass<ZCc0x84WakeUp>( 0 )->getInterval(), 0 );
        EXPECT_EQ( node->getWakeUpInterval(), utils::optional<uint32_t>() );

        callbacks.node_cmd_basic_result( ZNET_ERR_OK, 10, 0, 0, 0, 0, callbackArg );

        znet_version_report_t versionReport{};
        callbacks.node_cmd_version_result( ZNET_ERR_OK, 10, &versionReport, callbackArg );
    }

    EXPECT_EQ( node->getCommandClass<ZCc0x84WakeUp>( 0 )->getInterval(),
               param.expectedInterval ? *param.expectedInterval : 0 );
    EXPECT_EQ( node->getWakeUpInterval(), param.expectedInterval );
    EXPECT_EQ( node->isWokenUp(), param.isWokenUp );
}

INSTANTIATE_TEST_CASE_P(
    WakeUp,
    ZNetInterviewHandlerCheckWakeUpIntervalTestSuite,
    ::testing::Values(
        WakeUpIntervalParams{ 1, 1, utils::optional<uint32_t>( ZCc0x84WakeUp::DefaultWakeUpInterval ), true },
        WakeUpIntervalParams{ 1, 0, utils::optional<uint32_t>( ZCc0x84WakeUp::DefaultWakeUpInterval ), true },
        WakeUpIntervalParams{ 2, 1, utils::optional<uint32_t>(), false } ) );

}  // namespace ut

}  // namespace znet
