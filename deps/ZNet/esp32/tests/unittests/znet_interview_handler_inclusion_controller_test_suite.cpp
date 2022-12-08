#include "fake_owner_fixture.h"
#include "MainFixture.h"
#include <znet_handler_interview.hpp>
#include <gtest/gtest.h>
#include <gmock/gmock.h>

static const char* logtag = "znet_interviewHandlerIC";
#define LOG_LOCAL_LEVEL P_LOG_DEBUG

namespace znet {

namespace ut {

using ::testing::_;
using ::testing::ElementsAre;

class ZNetInterviewHandlerInclusionControllerTestSuite : public MainFixture, public FakeOwnerFixture {
protected:
    void SetUp() override {
        loginit();

        m_hub = std::make_shared<ZwaveHub>( 0xd1ac1b2d, 0x77, 0x01 );
        m_store = std::make_shared<Storage>( Storage::Version::v0_1 );

        znet_init( &m_callbacks );
        for( int i = 0; i < m_procIteration; ++i ) {
            znet_proc();
        }
    }
};

static void commonInterviewPart( void* callbackArg,
                                 const znet_callbacks_t& callbacks,
                                 const ZNode* node,
                                 InterviewHandler& handler ) {
    P_LOGD( logtag, "CommonInterviewPart" );
    using Step = InterviewHandlerBase::Step;
    auto nodeId = node->getId();

    // versions
    {
        callbacks.node_cmd_command_version_result( ZNET_ERR_OK, nodeId, 0, ZNET_COMMAND_CLASS_BASIC, 1, callbackArg );
        callbacks.node_cmd_command_version_result(
            ZNET_ERR_OK, nodeId, 0, ZNET_COMMAND_CLASS_CONFIGURATION, 1, callbackArg );
        callbacks.node_cmd_command_version_result(
            ZNET_ERR_OK, nodeId, 0, ZNET_COMMAND_CLASS_ZWAVEPLUS_INFO, 1, callbackArg );
        callbacks.node_cmd_command_version_result( ZNET_ERR_OK, nodeId, 0, ZNET_COMMAND_CLASS_VERSION, 1, callbackArg );
        callbacks.node_cmd_command_version_result(
            ZNET_ERR_OK, nodeId, 0, ZNET_COMMAND_CLASS_MANUFACTURER_SPECIFIC, 4, callbackArg );
        callbacks.node_cmd_command_version_result(
            ZNET_ERR_OK, nodeId, 0, ZNET_COMMAND_CLASS_SWITCH_BINARY, 2, callbackArg );
        callbacks.node_cmd_command_version_result(
            ZNET_ERR_OK, nodeId, 0, ZNET_COMMAND_CLASS_SECURITY, 1, callbackArg );

        EXPECT_EQ( node->getCommandClass( 0, ZNET_COMMAND_CLASS_ZWAVEPLUS_INFO )->getVersion(), 1 );
        EXPECT_EQ( node->getCommandClass( 0, ZNET_COMMAND_CLASS_VERSION )->getVersion(), 1 );
        EXPECT_EQ( node->getCommandClass( 0, ZNET_COMMAND_CLASS_MANUFACTURER_SPECIFIC )->getVersion(), 4 );
        EXPECT_EQ( node->getCommandClass( 0, ZNET_COMMAND_CLASS_SWITCH_BINARY )->getVersion(), 2 );
        EXPECT_EQ( node->getCommandClass( 0, ZNET_COMMAND_CLASS_CONFIGURATION )->getVersion(), 1 );
    }

    // zwave plus
    {
        znet_zwaveplus_info_report_t report{};
        report.role_type = 1;
        report.node_type = 2;
        report.installer_icon = 3;
        report.user_icon = 4;

        callbacks.node_cmd_zwaveplus_info_result( ZNET_ERR_OK, nodeId, &report, callbackArg );

        EXPECT_EQ( node->getRoleType(), 1 );
        EXPECT_EQ( node->getZwavePlusIconType(), 3 );
    }

    // manufacturer specific
    {
        znet_manufacturer_specific_report_t report{};
        report.manufacturer_id = 1;
        report.product_id = 2;
        report.product_type = 3;

        callbacks.node_cmd_manufacturer_specific_result( ZNET_ERR_OK, nodeId, 0, &report, callbackArg );

        EXPECT_EQ( handler.getCurrentStep(), Step::CommandClassesProcessing );
        EXPECT_EQ( node->getManufacturerId(), 1 );
        EXPECT_EQ( node->getProductId(), 2 );
        EXPECT_EQ( node->getProductType(), 3 );
    }

    // process command classes, setup node settings
    {
        callbacks.node_cmd_basic_result( ZNET_ERR_OK, nodeId, 0, 0, 0, 0, callbackArg );

        znet_version_report_t versionReport{};
        callbacks.node_cmd_version_result( ZNET_ERR_OK, nodeId, &versionReport, callbackArg );

        callbacks.node_cmd_binary_switch_result( ZNET_ERR_OK, nodeId, 0, 1, 0xFE, 0, callbackArg );
    }

    EXPECT_EQ( handler.getCurrentStep(), Step::Ready );
}

TEST_F( ZNetInterviewHandlerInclusionControllerTestSuite, fullInterviewFlow ) {
    void* callbackArg = &getImpl();
    using Step = InterviewHandlerBase::Step;

    getImpl().activateMode( IZNet::Mode::IncludeStarting );
    const auto& callbacks = getImpl().getCallbacks();
    auto& handler = dynamic_cast<InterviewHandler&>( getImpl().getHandler( 0 ) );

    P_LOGD( logtag, "Test non security node" );
    // add non security node
    ZNode* node = nullptr;
    {
        znet_nodeinfo_t nodeInfoSource{};
        nodeInfoSource.commands_count = 5;
        nodeInfoSource.node_id = 10;
        nodeInfoSource.security = ZNET_NODEINFO_SECURITY_NONE;

        uint8_t buff[sizeof( znet_nodeinfo_t ) + nodeInfoSource.commands_count];
        memcpy( buff, &nodeInfoSource, sizeof( znet_nodeinfo_t ) );
        auto commands = buff + sizeof( znet_nodeinfo_t );
        commands[0] = ZNET_COMMAND_CLASS_ZWAVEPLUS_INFO;
        commands[1] = ZNET_COMMAND_CLASS_VERSION;
        commands[2] = ZNET_COMMAND_CLASS_MANUFACTURER_SPECIFIC;
        commands[3] = ZNET_COMMAND_CLASS_SWITCH_BINARY;
        commands[4] = ZNET_COMMAND_CLASS_CONFIGURATION;
        auto nodeInfo = reinterpret_cast<const znet_nodeinfo_t*>( buff );

        callbacks.node_add_result( ZNET_ERR_OK, nodeInfo, callbackArg );

        EXPECT_EQ( handler.getCurrentStep(), Step::WaitPrimaryController );
        ASSERT_NE( getOwner().onFindNode( 10 ), nullptr );
        node = getOwner().onFindNode( 10 );

        EXPECT_EQ( node->getChannelsCount(), 1 );
        EXPECT_THAT( getOwner().getNodeCommandClassesIds( 10, 0 ),
                     ElementsAre( ZNET_COMMAND_CLASS_BASIC,
                                  ZNET_COMMAND_CLASS_CONFIGURATION,
                                  ZNET_COMMAND_CLASS_ZWAVEPLUS_INFO,
                                  ZNET_COMMAND_CLASS_VERSION,
                                  ZNET_COMMAND_CLASS_MANUFACTURER_SPECIFIC,
                                  ZNET_COMMAND_CLASS_SWITCH_BINARY ) );
    }

    callbacks.node_cmd_inclusion_controller_complete(
        ZNET_ERR_OK, 10, ZNET_INCLUSION_CONTROLLER_PROXY_INCLUSION, ZNET_INCLUSION_CONTROLLER_STEP_OK, callbackArg );
    EXPECT_EQ( handler.getCurrentStep(), Step::GetCommandClassVersion );

    commonInterviewPart( callbackArg, callbacks, node, handler );

    P_LOGD( logtag, "Test security node" );
    getImpl().activateMode( IZNet::Mode::IncludeStarting );

    // add S0 security node
    {
        znet_nodeinfo_t nodeInfoSource{};
        nodeInfoSource.commands_count = 6;
        nodeInfoSource.node_id = 11;
        nodeInfoSource.security = ZNET_NODEINFO_SECURITY_S0;

        uint8_t buff[sizeof( znet_nodeinfo_t ) + nodeInfoSource.commands_count];
        memcpy( buff, &nodeInfoSource, sizeof( znet_nodeinfo_t ) );
        auto commands = buff + sizeof( znet_nodeinfo_t );
        commands[0] = ZNET_COMMAND_CLASS_ZWAVEPLUS_INFO;
        commands[1] = ZNET_COMMAND_CLASS_VERSION;
        commands[2] = ZNET_COMMAND_CLASS_MANUFACTURER_SPECIFIC;
        commands[3] = ZNET_COMMAND_CLASS_SWITCH_BINARY;
        commands[4] = ZNET_COMMAND_CLASS_CONFIGURATION;
        commands[5] = ZNET_COMMAND_CLASS_SECURITY;
        auto nodeInfo = reinterpret_cast<const znet_nodeinfo_t*>( buff );

        callbacks.node_add_result( ZNET_ERR_OK, nodeInfo, callbackArg );

        EXPECT_EQ( handler.getCurrentStep(), Step::WaitPrimaryController );
        ASSERT_NE( getOwner().onFindNode( 11 ), nullptr );
        node = getOwner().onFindNode( 11 );

        EXPECT_EQ( node->getChannelsCount(), 1 );
        EXPECT_THAT( getOwner().getNodeCommandClassesIds( 11, 0 ),
                     ElementsAre( ZNET_COMMAND_CLASS_BASIC,
                                  ZNET_COMMAND_CLASS_CONFIGURATION,
                                  ZNET_COMMAND_CLASS_ZWAVEPLUS_INFO,
                                  ZNET_COMMAND_CLASS_VERSION,
                                  ZNET_COMMAND_CLASS_MANUFACTURER_SPECIFIC,
                                  ZNET_COMMAND_CLASS_SWITCH_BINARY,
                                  ZNET_COMMAND_CLASS_SECURITY ) );
    }

    znet_set_ic_info( 1, Encapsulation_Sec_2 );
    callbacks.node_cmd_inclusion_controller_initiate(
        ZNET_ERR_OK, 1, 11, ZNET_INCLUSION_CONTROLLER_S0_INCLUSION, callbackArg );
    EXPECT_EQ( handler.getCurrentStep(), Step::WaitPrimaryController );

    callbacks.node_cmd_inclusion_controller_complete(
        ZNET_ERR_OK, 11, ZNET_INCLUSION_CONTROLLER_PROXY_INCLUSION, ZNET_INCLUSION_CONTROLLER_STEP_OK, callbackArg );
    EXPECT_EQ( handler.getCurrentStep(), Step::WaitSecurity );

    // security
    {
        znet_security_supported_report_t sourceReport{};
        sourceReport.commands_count = 2;
        uint8_t buff[sizeof( sourceReport ) + sourceReport.commands_count];
        memcpy( buff, &sourceReport, sizeof( sourceReport ) );
        uint8_t* commands = buff + sizeof( sourceReport );
        commands[0] = ZNET_COMMAND_CLASS_SWITCH_BINARY;
        commands[1] = ZNET_COMMAND_CLASS_MARK;
        auto report = reinterpret_cast<const znet_security_supported_report_t*>( buff );

        callbacks.node_cmd_security_supported_result( ZNET_ERR_OK, 11, 0, report, callbackArg );
        EXPECT_EQ( handler.getCurrentStep(), Step::GetCommandClassVersion );

        EXPECT_EQ( node->getChannelsCount(), 1 );
        EXPECT_THAT( getOwner().getNodeCommandClassesIds( 11, 0 ),
                     ElementsAre( ZNET_COMMAND_CLASS_BASIC,
                                  ZNET_COMMAND_CLASS_CONFIGURATION,
                                  ZNET_COMMAND_CLASS_ZWAVEPLUS_INFO,
                                  ZNET_COMMAND_CLASS_VERSION,
                                  ZNET_COMMAND_CLASS_MANUFACTURER_SPECIFIC,
                                  ZNET_COMMAND_CLASS_SWITCH_BINARY,
                                  ZNET_COMMAND_CLASS_SECURITY ) );
    }

    commonInterviewPart( callbackArg, callbacks, node, handler );
}

}  // namespace ut

}  // namespace znet
