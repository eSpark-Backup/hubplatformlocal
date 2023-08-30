#include <znet/znet_node.hpp>
#include <znet/cc_0x60_multi_channel.hpp>

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

class ZCcConfigurationSuite : public ::testing::Test, public NodeRequestsManagingFixture {
public:
};

TEST_F( ZCcConfigurationSuite, buildChannels ) {
    getNodesProvider().nodeCreate( 1 );
    auto node = getNodesProvider().onFindNode( 1 );

    MockFunction<void( RequestFailTypeOpt )> callback;

    node->addCommandClass( 0, ZNET_COMMAND_CLASS_MULTI_CHANNEL, 1 );
    auto cc = node->getCommandClass<ZCc0x60MultiChannel>( 0 );

    EXPECT_CALL( callback, Call( _ ) ).Times( 0 );

    EXPECT_TRUE( cc->buildChannels( callback.AsStdFunction(), {} ) );

    znet_multichannel_endpoint_report_t mainReport{};
    mainReport.channel_ids_count = 3;
    mainReport.agregated_channel_ids_count = 5;
    getRequestsManager().onRespond( MultichannelEndpointRequest( 1 ), mainReport );

    znet_multichannel_endpoint_find_report_t findReport{};
    findReport.reports_to_follow = 0;
    findReport.generic = 0xFF;
    findReport.specific = 0xFF;
    findReport.channel_ids_count = 0;
    getRequestsManager().onRespond( MultichannelEndpointFindRequest( 1, 0xFF, 0xFF, true ), findReport );

    const znet_command_class_t classes1[] = { ZNET_COMMAND_CLASS_METER };
    znet_multichannel_capability_report_t capabilityReport1{};
    capabilityReport1.dynamic = true;
    capabilityReport1.channel_id = 1;
    capabilityReport1.commands_count = 2;
    capabilityReport1.commands = classes1;
    getRequestsManager().onRespond( MultichannelEndpointCapabilityRequest( 1, 1 ), capabilityReport1 );

    const znet_command_class_t classes2[] = { ZNET_COMMAND_CLASS_METER, ZNET_COMMAND_CLASS_NOTIFICATION };
    znet_multichannel_capability_report_t capabilityReport2{};
    capabilityReport2.dynamic = false;
    capabilityReport2.channel_id = 2;
    capabilityReport2.commands_count = 2;
    capabilityReport2.commands = classes2;
    getRequestsManager().onRespond( MultichannelEndpointCapabilityRequest( 1, 2 ), capabilityReport2 );

    const znet_command_class_t classes3[] = {
        ZNET_COMMAND_CLASS_SOUND_SWITCH, ZNET_COMMAND_CLASS_MARK, ZNET_COMMAND_CLASS_NOTIFICATION
    };
    znet_multichannel_capability_report_t capabilityReport3{};
    capabilityReport3.dynamic = false;
    capabilityReport3.channel_id = 3;
    capabilityReport3.commands_count = 3;
    capabilityReport3.commands = classes3;

    EXPECT_CALL( callback, Call( RequestFailTypeOpt{} ) ).Times( 1 );
    getRequestsManager().onRespond( MultichannelEndpointCapabilityRequest( 1, 3 ), capabilityReport3 );

    ASSERT_EQ( node->getChannelsCount(), 4 );
    ASSERT_EQ( node->getAggregatedChannelsCount(), 0 );
    ASSERT_EQ( node->getAllChannelsCount(), 4 );

    EXPECT_THAT( getNodesProvider().getNodeCommandClassesIds( 1, 0 ), ElementsAre( ZNET_COMMAND_CLASS_MULTI_CHANNEL ) );
    EXPECT_THAT( getNodesProvider().getNodeCommandClassesIds( 1, 1 ), ElementsAre() );
    EXPECT_THAT( getNodesProvider().getNodeCommandClassesIds( 1, 2 ),
                 ElementsAre( ZNET_COMMAND_CLASS_BASIC, ZNET_COMMAND_CLASS_METER, ZNET_COMMAND_CLASS_NOTIFICATION ) );
    EXPECT_THAT( getNodesProvider().getNodeCommandClassesIds( 1, 3 ),
                 ElementsAre( ZNET_COMMAND_CLASS_BASIC, ZNET_COMMAND_CLASS_SOUND_SWITCH ) );
}

TEST_F( ZCcConfigurationSuite, buildChannelsSeveralFinds ) {
    getNodesProvider().nodeCreate( 1 );
    auto node = getNodesProvider().onFindNode( 1 );

    MockFunction<void( RequestFailTypeOpt )> callback;

    node->addCommandClass( 0, ZNET_COMMAND_CLASS_MULTI_CHANNEL, 1 );
    auto cc = node->getCommandClass<ZCc0x60MultiChannel>( 0 );

    EXPECT_CALL( callback, Call( _ ) ).Times( 0 );

    EXPECT_TRUE( cc->buildChannels( callback.AsStdFunction(), {} ) );

    znet_multichannel_endpoint_report_t mainReport{};
    mainReport.channel_ids_count = 1;
    mainReport.agregated_channel_ids_count = 5;
    getRequestsManager().onRespond( MultichannelEndpointRequest( 1 ), mainReport );

    znet_multichannel_endpoint_find_report_t findReport{};
    findReport.reports_to_follow = 2;
    findReport.generic = 0xFF;
    findReport.specific = 0xFF;
    findReport.channel_ids_count = 0;
    getRequestsManager().onRespond( MultichannelEndpointFindRequest( 1, 0xFF, 0xFF, true ), findReport );

    findReport.reports_to_follow = 1;
    getRequestsManager().onRespond( MultichannelEndpointFindRequest( 1, 0xFF, 0xFF, true ), findReport );

    findReport.reports_to_follow = 0;
    getRequestsManager().onRespond( MultichannelEndpointFindRequest( 1, 0xFF, 0xFF, true ), findReport );

    const znet_command_class_t classes1[] = { ZNET_COMMAND_CLASS_METER };
    znet_multichannel_capability_report_t capabilityReport1{};
    capabilityReport1.dynamic = true;
    capabilityReport1.channel_id = 1;
    capabilityReport1.commands_count = 2;
    capabilityReport1.commands = classes1;

    EXPECT_CALL( callback, Call( RequestFailTypeOpt{} ) ).Times( 1 );
    getRequestsManager().onRespond( MultichannelEndpointCapabilityRequest( 1, 1 ), capabilityReport1 );

    ASSERT_EQ( node->getChannelsCount(), 2 );
    ASSERT_EQ( node->getAggregatedChannelsCount(), 0 );
    ASSERT_EQ( node->getAllChannelsCount(), 2 );

    EXPECT_THAT( getNodesProvider().getNodeCommandClassesIds( 1, 0 ), ElementsAre( ZNET_COMMAND_CLASS_MULTI_CHANNEL ) );
    EXPECT_THAT( getNodesProvider().getNodeCommandClassesIds( 1, 1 ), ElementsAre() );
}

TEST_F( ZCcConfigurationSuite, buildChannelsWithSecurity ) {
    getNodesProvider().nodeCreate( 1 );
    auto node = getNodesProvider().onFindNode( 1 );

    MockFunction<void( RequestFailTypeOpt )> callback;

    node->setSecurity( SecurityMode::Class::S0 );
    node->addCommandClass( 0, ZNET_COMMAND_CLASS_MULTI_CHANNEL, 1 );
    auto cc = node->getCommandClass<ZCc0x60MultiChannel>( 0 );

    EXPECT_CALL( callback, Call( _ ) ).Times( 0 );

    EXPECT_TRUE( cc->buildChannels( callback.AsStdFunction(), {} ) );

    znet_multichannel_endpoint_report_t mainReport{};
    mainReport.channel_ids_count = 2;
    getRequestsManager().onRespond( MultichannelEndpointRequest( 1 ), mainReport );

    znet_multichannel_endpoint_find_report_t findReport{};
    findReport.reports_to_follow = 0;
    findReport.generic = 0xFF;
    findReport.specific = 0xFF;
    findReport.channel_ids_count = 0;
    getRequestsManager().onRespond( MultichannelEndpointFindRequest( 1, 0xFF, 0xFF, true ), findReport );

    {
        const uint8_t securityBuffer[3] = { 2, ZNET_COMMAND_CLASS_METER, ZNET_COMMAND_CLASS_NOTIFICATION };

        getRequestsManager().onRespond( SecuritySupportedRequest( 1, 1 ),
                                        reinterpret_cast<const znet_security_supported_report_t*>( securityBuffer ) );

        const znet_command_class_t classes[] = { ZNET_COMMAND_CLASS_BATTERY };
        znet_multichannel_capability_report_t capabilityReport{};
        capabilityReport.dynamic = false;
        capabilityReport.channel_id = 1;
        capabilityReport.commands_count = 1;
        capabilityReport.commands = classes;
        getRequestsManager().onRespond( MultichannelEndpointCapabilityRequest( 1, 1 ), capabilityReport );
    }

    {
        const uint8_t securityBuffer[2] = { 1, ZNET_COMMAND_CLASS_METER };

        getRequestsManager().onRespond( SecuritySupportedRequest( 1, 2 ),
                                        reinterpret_cast<const znet_security_supported_report_t*>( securityBuffer ) );

        EXPECT_CALL( callback, Call( RequestFailTypeOpt{} ) ).Times( 1 );

        const znet_command_class_t classes[] = { ZNET_COMMAND_CLASS_BATTERY };
        znet_multichannel_capability_report_t capabilityReport{};
        capabilityReport.dynamic = true;
        capabilityReport.channel_id = 1;
        capabilityReport.commands_count = 1;
        capabilityReport.commands = classes;
        getRequestsManager().onRespond( MultichannelEndpointCapabilityRequest( 1, 2 ), capabilityReport );
    }

    ASSERT_EQ( node->getChannelsCount(), 3 );
    ASSERT_EQ( node->getAggregatedChannelsCount(), 0 );
    ASSERT_EQ( node->getAllChannelsCount(), 3 );

    EXPECT_THAT( getNodesProvider().getNodeCommandClassesIds( 1, 0 ), ElementsAre( ZNET_COMMAND_CLASS_MULTI_CHANNEL ) );
    EXPECT_THAT( getNodesProvider().getNodeCommandClassesIds( 1, 1 ),
                 ElementsAre( ZNET_COMMAND_CLASS_BASIC,
                              ZNET_COMMAND_CLASS_METER,
                              ZNET_COMMAND_CLASS_NOTIFICATION,
                              ZNET_COMMAND_CLASS_BATTERY ) );
    EXPECT_THAT( getNodesProvider().getNodeCommandClassesIds( 1, 2 ),
                 ElementsAre( ZNET_COMMAND_CLASS_BASIC, ZNET_COMMAND_CLASS_METER ) );
}

TEST_F( ZCcConfigurationSuite, buildChannelsV4NoAggregatedChannels ) {
    getNodesProvider().nodeCreate( 1 );
    auto node = getNodesProvider().onFindNode( 1 );

    MockFunction<void( RequestFailTypeOpt )> callback;

    node->addCommandClass( 0, ZNET_COMMAND_CLASS_MULTI_CHANNEL, 4 );
    auto cc = node->getCommandClass<ZCc0x60MultiChannel>( 0 );

    EXPECT_CALL( callback, Call( _ ) ).Times( 0 );

    EXPECT_TRUE( cc->buildChannels( callback.AsStdFunction(), {} ) );

    znet_multichannel_endpoint_report_t mainReport{};
    mainReport.channel_ids_count = 3;
    mainReport.agregated_channel_ids_count = 0;
    getRequestsManager().onRespond( MultichannelEndpointRequest( 1 ), mainReport );

    znet_multichannel_endpoint_find_report_t findReport{};
    findReport.reports_to_follow = 0;
    findReport.generic = 0xFF;
    findReport.specific = 0xFF;
    findReport.channel_ids_count = 0;
    getRequestsManager().onRespond( MultichannelEndpointFindRequest( 1, 0xFF, 0xFF, true ), findReport );

    const znet_command_class_t classes1[] = { ZNET_COMMAND_CLASS_METER };
    znet_multichannel_capability_report_t capabilityReport1{};
    capabilityReport1.dynamic = true;
    capabilityReport1.channel_id = 1;
    capabilityReport1.commands_count = 2;
    capabilityReport1.commands = classes1;
    getRequestsManager().onRespond( MultichannelEndpointCapabilityRequest( 1, 1 ), capabilityReport1 );

    const znet_command_class_t classes2[] = { ZNET_COMMAND_CLASS_METER, ZNET_COMMAND_CLASS_NOTIFICATION };
    znet_multichannel_capability_report_t capabilityReport2{};
    capabilityReport2.dynamic = false;
    capabilityReport2.channel_id = 2;
    capabilityReport2.commands_count = 2;
    capabilityReport2.commands = classes2;
    getRequestsManager().onRespond( MultichannelEndpointCapabilityRequest( 1, 2 ), capabilityReport2 );

    const znet_command_class_t classes3[] = {
        ZNET_COMMAND_CLASS_SOUND_SWITCH, ZNET_COMMAND_CLASS_MARK, ZNET_COMMAND_CLASS_NOTIFICATION
    };
    znet_multichannel_capability_report_t capabilityReport3{};
    capabilityReport3.dynamic = false;
    capabilityReport3.channel_id = 3;
    capabilityReport3.commands_count = 3;
    capabilityReport3.commands = classes3;

    EXPECT_CALL( callback, Call( RequestFailTypeOpt{} ) ).Times( 1 );
    getRequestsManager().onRespond( MultichannelEndpointCapabilityRequest( 1, 3 ), capabilityReport3 );

    ASSERT_EQ( node->getChannelsCount(), 4 );
    ASSERT_EQ( node->getAggregatedChannelsCount(), 0 );
    ASSERT_EQ( node->getAllChannelsCount(), 4 );

    EXPECT_THAT( getNodesProvider().getNodeCommandClassesIds( 1, 0 ), ElementsAre( ZNET_COMMAND_CLASS_MULTI_CHANNEL ) );
    EXPECT_THAT( getNodesProvider().getNodeCommandClassesIds( 1, 1 ), ElementsAre() );
    EXPECT_THAT( getNodesProvider().getNodeCommandClassesIds( 1, 2 ),
                 ElementsAre( ZNET_COMMAND_CLASS_BASIC, ZNET_COMMAND_CLASS_METER, ZNET_COMMAND_CLASS_NOTIFICATION ) );
    EXPECT_THAT( getNodesProvider().getNodeCommandClassesIds( 1, 3 ),
                 ElementsAre( ZNET_COMMAND_CLASS_BASIC, ZNET_COMMAND_CLASS_SOUND_SWITCH ) );
}

TEST_F( ZCcConfigurationSuite, buildChannelsV4AggregatedChannels ) {
    getNodesProvider().nodeCreate( 1 );
    auto node = getNodesProvider().onFindNode( 1 );

    MockFunction<void( RequestFailTypeOpt )> callback;

    node->addCommandClass( 0, ZNET_COMMAND_CLASS_MULTI_CHANNEL, 4 );
    auto cc = node->getCommandClass<ZCc0x60MultiChannel>( 0 );

    EXPECT_CALL( callback, Call( _ ) ).Times( 0 );

    EXPECT_TRUE( cc->buildChannels( callback.AsStdFunction(), {} ) );

    znet_multichannel_endpoint_report_t mainReport{};
    mainReport.channel_ids_count = 3;
    mainReport.agregated_channel_ids_count = 2;
    getRequestsManager().onRespond( MultichannelEndpointRequest( 1 ), mainReport );

    znet_multichannel_endpoint_find_report_t findReport{};
    findReport.reports_to_follow = 0;
    findReport.generic = 0xFF;
    findReport.specific = 0xFF;
    findReport.channel_ids_count = 0;
    getRequestsManager().onRespond( MultichannelEndpointFindRequest( 1, 0xFF, 0xFF, true ), findReport );

    const znet_command_class_t classes1[] = { ZNET_COMMAND_CLASS_METER };
    znet_multichannel_capability_report_t capabilityReport1{};
    capabilityReport1.dynamic = true;
    capabilityReport1.channel_id = 1;
    capabilityReport1.commands_count = 2;
    capabilityReport1.commands = classes1;
    getRequestsManager().onRespond( MultichannelEndpointCapabilityRequest( 1, 1 ), capabilityReport1 );

    const znet_command_class_t classes2[] = { ZNET_COMMAND_CLASS_METER, ZNET_COMMAND_CLASS_NOTIFICATION };
    znet_multichannel_capability_report_t capabilityReport2{};
    capabilityReport2.dynamic = false;
    capabilityReport2.channel_id = 2;
    capabilityReport2.commands_count = 2;
    capabilityReport2.commands = classes2;
    getRequestsManager().onRespond( MultichannelEndpointCapabilityRequest( 1, 2 ), capabilityReport2 );

    const znet_command_class_t classes3[] = {
        ZNET_COMMAND_CLASS_SOUND_SWITCH, ZNET_COMMAND_CLASS_MARK, ZNET_COMMAND_CLASS_NOTIFICATION
    };
    znet_multichannel_capability_report_t capabilityReport3{};
    capabilityReport3.dynamic = false;
    capabilityReport3.channel_id = 3;
    capabilityReport3.commands_count = 3;
    capabilityReport3.commands = classes3;
    getRequestsManager().onRespond( MultichannelEndpointCapabilityRequest( 1, 3 ), capabilityReport3 );

    znet_multichannel_aggregated_members_report_t aggregatedReport{};
    aggregatedReport.channel_id = 4;
    aggregatedReport.agregated_channel_ids_count = 0;
    getRequestsManager().onRespond( MultichannelAggregatedMemberRequest( 1, 4 ), aggregatedReport );

    aggregatedReport.channel_id = 5;
    EXPECT_CALL( callback, Call( RequestFailTypeOpt{} ) ).Times( 1 );
    getRequestsManager().onRespond( MultichannelAggregatedMemberRequest( 1, 5 ), aggregatedReport );

    ASSERT_EQ( node->getChannelsCount(), 4 );
    ASSERT_EQ( node->getAggregatedChannelsCount(), 2 );
    ASSERT_EQ( node->getAllChannelsCount(), 6 );

    EXPECT_THAT( getNodesProvider().getNodeCommandClassesIds( 1, 0 ), ElementsAre( ZNET_COMMAND_CLASS_MULTI_CHANNEL ) );
    EXPECT_THAT( getNodesProvider().getNodeCommandClassesIds( 1, 1 ), ElementsAre() );
    EXPECT_THAT( getNodesProvider().getNodeCommandClassesIds( 1, 2 ),
                 ElementsAre( ZNET_COMMAND_CLASS_BASIC, ZNET_COMMAND_CLASS_METER, ZNET_COMMAND_CLASS_NOTIFICATION ) );
    EXPECT_THAT( getNodesProvider().getNodeCommandClassesIds( 1, 3 ),
                 ElementsAre( ZNET_COMMAND_CLASS_BASIC, ZNET_COMMAND_CLASS_SOUND_SWITCH ) );
}

TEST_F( ZCcConfigurationSuite, buildChannelsV4AggregatedChannelsWithSecurity ) {
    getNodesProvider().nodeCreate( 1 );
    auto node = getNodesProvider().onFindNode( 1 );

    MockFunction<void( RequestFailTypeOpt )> callback;

    node->setSecurity( SecurityMode::Class::S0 );
    node->addCommandClass( 0, ZNET_COMMAND_CLASS_MULTI_CHANNEL, 4 );
    auto cc = node->getCommandClass<ZCc0x60MultiChannel>( 0 );

    EXPECT_CALL( callback, Call( _ ) ).Times( 0 );

    EXPECT_TRUE( cc->buildChannels( callback.AsStdFunction(), {} ) );

    znet_multichannel_endpoint_report_t mainReport{};
    mainReport.channel_ids_count = 2;
    mainReport.agregated_channel_ids_count = 2;
    getRequestsManager().onRespond( MultichannelEndpointRequest( 1 ), mainReport );

    znet_multichannel_endpoint_find_report_t findReport{};
    findReport.reports_to_follow = 0;
    findReport.generic = 0xFF;
    findReport.specific = 0xFF;
    findReport.channel_ids_count = 0;
    getRequestsManager().onRespond( MultichannelEndpointFindRequest( 1, 0xFF, 0xFF, true ), findReport );

    {
        const uint8_t securityBuffer[3] = { 2, ZNET_COMMAND_CLASS_METER, ZNET_COMMAND_CLASS_NOTIFICATION };

        getRequestsManager().onRespond( SecuritySupportedRequest( 1, 1 ),
                                        reinterpret_cast<const znet_security_supported_report_t*>( securityBuffer ) );

        const znet_command_class_t classes[] = { ZNET_COMMAND_CLASS_BATTERY };
        znet_multichannel_capability_report_t capabilityReport{};
        capabilityReport.dynamic = false;
        capabilityReport.channel_id = 1;
        capabilityReport.commands_count = 1;
        capabilityReport.commands = classes;
        getRequestsManager().onRespond( MultichannelEndpointCapabilityRequest( 1, 1 ), capabilityReport );
    }

    {
        const uint8_t securityBuffer[2] = { 1, ZNET_COMMAND_CLASS_METER };

        getRequestsManager().onRespond( SecuritySupportedRequest( 1, 2 ),
                                        reinterpret_cast<const znet_security_supported_report_t*>( securityBuffer ) );

        const znet_command_class_t classes[] = { ZNET_COMMAND_CLASS_BATTERY };
        znet_multichannel_capability_report_t capabilityReport{};
        capabilityReport.dynamic = true;
        capabilityReport.channel_id = 2;
        capabilityReport.commands_count = 1;
        capabilityReport.commands = classes;
        getRequestsManager().onRespond( MultichannelEndpointCapabilityRequest( 1, 2 ), capabilityReport );
    }

    znet_multichannel_aggregated_members_report_t aggregatedReport{};
    aggregatedReport.channel_id = 3;
    aggregatedReport.agregated_channel_ids_count = 0;
    getRequestsManager().onRespond( MultichannelAggregatedMemberRequest( 1, 3 ), aggregatedReport );

    aggregatedReport.channel_id = 4;
    EXPECT_CALL( callback, Call( RequestFailTypeOpt{} ) ).Times( 1 );
    getRequestsManager().onRespond( MultichannelAggregatedMemberRequest( 1, 4 ), aggregatedReport );

    ASSERT_EQ( node->getChannelsCount(), 3 );
    ASSERT_EQ( node->getAggregatedChannelsCount(), 2 );
    ASSERT_EQ( node->getAllChannelsCount(), 5 );

    EXPECT_THAT( getNodesProvider().getNodeCommandClassesIds( 1, 0 ), ElementsAre( ZNET_COMMAND_CLASS_MULTI_CHANNEL ) );
    EXPECT_THAT( getNodesProvider().getNodeCommandClassesIds( 1, 1 ),
                 ElementsAre( ZNET_COMMAND_CLASS_BASIC,
                              ZNET_COMMAND_CLASS_METER,
                              ZNET_COMMAND_CLASS_NOTIFICATION,
                              ZNET_COMMAND_CLASS_BATTERY ) );
    EXPECT_THAT( getNodesProvider().getNodeCommandClassesIds( 1, 2 ),
                 ElementsAre( ZNET_COMMAND_CLASS_BASIC, ZNET_COMMAND_CLASS_METER ) );
}

}  // namespace ut
}  // namespace znet
