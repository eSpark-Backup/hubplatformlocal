#include <znet/znet_node.hpp>
#include <znet/cc_0x70_configuration.hpp>
#include <znet/enums/configuration_format.hpp>

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
    void generateV3DefaultConfigurations( MockFunction<void( RequestFailTypeOpt )>& callback ) {
        std::vector<ZCc0x70Configuration::Parameter> configurations = {
            { .m_id = 1,
              .m_format = ConfigurationFormat::BitField,
              .m_size = 1,
              .m_readOnly = false,
              .m_minValue = 1,
              .m_maxValue = 100,
              .m_defaultValue = 50,
              .m_name = "testName1",
              .m_info = "testInfo1" },
            { .m_id = 256,
              .m_format = ConfigurationFormat::Enumerated,
              .m_size = 4,
              .m_readOnly = false,
              .m_minValue = 30,
              .m_maxValue = 50,
              .m_defaultValue = 45,
              .m_name = "testName2",
              .m_info = "testInfo2" },
            { .m_id = 2,
              .m_format = ConfigurationFormat::BitField,
              .m_size = 2,
              .m_readOnly = false,
              .m_minValue = 1,
              .m_maxValue = 100,
              .m_defaultValue = 50,
              .m_name = "testName3",
              .m_info = "testInfo3" },
        };

        auto node = getNodesProvider().onFindNode( 1 );
        auto cc = node->getCommandClass<ZCc0x70Configuration>( 0 );

        EXPECT_CALL( callback, Call( _ ) ).Times( 0 );

        // Register report frames
        znet_configuration_properties_report_t propReport;
        memset( &propReport, 0, sizeof( propReport ) );

        uint8_t buffName[sizeof( znet_configuration_name_report_t ) + 255];
        memset( buffName, 0, sizeof( znet_configuration_name_report_t ) + 255 );
        znet_configuration_name_report_t* nameReport = (znet_configuration_name_report_t*)&buffName;

        uint8_t buffInfo[sizeof( znet_configuration_info_report_t ) + 255];
        memset( buffInfo, 0, sizeof( znet_configuration_info_report_t ) + 255 );
        znet_configuration_info_report_t* infoReport = (znet_configuration_info_report_t*)&buffInfo;

        EXPECT_TRUE( cc->interview( callback.AsStdFunction(), {} ) );
        propReport.next_param_number = configurations[0].m_id;

        node->getRequestsManager().onRespond( ConfigurationUnknownPropertiesRequest( 1, 0, 0 ), propReport );

        for( int i = 0; i < configurations.size(); ++i ) {
            const auto& config = configurations.at( i );
            // report name
            nameReport->param_number = config.m_id;
            memcpy( nameReport->data, config.m_name.c_str(), config.m_name.size() );
            nameReport->name_length = config.m_name.size();

            node->getRequestsManager().onRespond( ConfigurationNameRequest( 1, 0, config.m_id, true ), *nameReport );

            // report info
            infoReport->param_number = config.m_id;
            memcpy( infoReport->data, config.m_info.c_str(), config.m_info.size() );
            infoReport->info_length = config.m_info.size();

            node->getRequestsManager().onRespond( ConfigurationInfoRequest( 1, 0, config.m_id, true ), *infoReport );

            // report properties
            propReport.param_number = config.m_id;
            propReport.param_format = static_cast<uint8_t>( config.m_format );
            propReport.param_size = config.m_size;
            propReport.param_min_value = config.m_minValue;
            propReport.param_max_value = config.m_maxValue;
            propReport.param_default_value = config.m_defaultValue;
            propReport.next_param_number = ( i < configurations.size() - 1 ) ? configurations.at( i + 1 ).m_id : 0;

            if( i + 1 == configurations.size() ) {
                EXPECT_FALSE( cc->hasInterviewDone() );
                EXPECT_CALL( callback, Call( RequestFailTypeOpt{} ) ).Times( 1 );
            }

            node->getRequestsManager().onRespond( ConfigurationPropertiesRequest( 1, 0, config.m_id ), propReport );
        }

        EXPECT_TRUE( cc->hasInterviewDone() );
    }
};

TEST_F( ZCcConfigurationSuite, interviewV1 ) {
    getNodesProvider().nodeCreate( 1 );
    auto node = getNodesProvider().onFindNode( 1 );

    MockFunction<void( RequestFailTypeOpt )> callback;

    node->addCommandClass( 0, ZNET_COMMAND_CLASS_CONFIGURATION, 1 );
    auto cc = node->getCommandClass<ZCc0x70Configuration>( 0 );

    EXPECT_CALL( callback, Call( _ ) ).Times( 0 );

    EXPECT_FALSE( cc->interview( {}, {} ) );
}

TEST_F( ZCcConfigurationSuite, interviewV3_Simple ) {
    getNodesProvider().nodeCreate( 1 );
    auto node = getNodesProvider().onFindNode( 1 );

    MockFunction<void( RequestFailTypeOpt )> callback;

    node->addCommandClass( 0, ZNET_COMMAND_CLASS_CONFIGURATION, 3 );
    auto cc = node->getCommandClass<ZCc0x70Configuration>( 0 );

    EXPECT_CALL( callback, Call( _ ) ).Times( 0 );

    EXPECT_TRUE( cc->interview( callback.AsStdFunction(), {} ) );

    znet_configuration_properties_report_t propReport;
    memset( &propReport, 0, sizeof( propReport ) );
    propReport.next_param_number = 1;

    node->getRequestsManager().onRespond( ConfigurationUnknownPropertiesRequest( 1, 0, 0 ), propReport );

    // Start first parameter interview
    EXPECT_FALSE( cc->hasInterviewDone() );

    uint8_t buffName[sizeof( znet_configuration_name_report_t ) + 255];
    memset( buffName, 0, sizeof( znet_configuration_name_report_t ) + 255 );
    znet_configuration_name_report_t* nameReport = (znet_configuration_name_report_t*)&buffName;

    nameReport->param_number = 1;
    memcpy( nameReport->data, "testName1", 9 );
    nameReport->name_length = 9;

    node->getRequestsManager().onRespond( ConfigurationNameRequest( 1, 0, 1, true ), *nameReport );

    uint8_t buffInfo[sizeof( znet_configuration_info_report_t ) + 255];
    memset( buffInfo, 0, sizeof( znet_configuration_info_report_t ) + 255 );
    znet_configuration_info_report_t* infoReport = (znet_configuration_info_report_t*)&buffInfo;

    infoReport->param_number = 1;
    memcpy( infoReport->data, "testInfo1", 9 );
    infoReport->info_length = 9;

    node->getRequestsManager().onRespond( ConfigurationInfoRequest( 1, 0, 1, true ), *infoReport );

    memset( &propReport, 0, sizeof( propReport ) );
    propReport.param_number = 1;
    propReport.param_format = 3;
    propReport.param_size = 1;
    propReport.param_min_value = 1;
    propReport.param_max_value = 100;
    propReport.param_default_value = 50;
    propReport.next_param_number = 2;

    node->getRequestsManager().onRespond( ConfigurationPropertiesRequest( 1, 0, 1 ), propReport );

    // Start second parameter interview
    EXPECT_FALSE( cc->hasInterviewDone() );

    memset( buffName, 0, sizeof( znet_configuration_name_report_t ) + 255 );

    nameReport->param_number = 2;
    memcpy( nameReport->data, "testName2", 9 );
    nameReport->name_length = 9;

    node->getRequestsManager().onRespond( ConfigurationNameRequest( 1, 0, 2, true ), *nameReport );

    memset( buffInfo, 0, sizeof( znet_configuration_info_report_t ) + 255 );

    infoReport->param_number = 2;
    memcpy( infoReport->data, "testInfo2", 9 );
    infoReport->info_length = 9;

    node->getRequestsManager().onRespond( ConfigurationInfoRequest( 1, 0, 2, true ), *infoReport );

    EXPECT_FALSE( cc->hasInterviewDone() );
    EXPECT_CALL( callback, Call( RequestFailTypeOpt{} ) ).Times( 1 );

    memset( &propReport, 0, sizeof( propReport ) );
    propReport.param_number = 2;
    propReport.param_format = 2;
    propReport.param_size = 4;
    propReport.param_min_value = 30;
    propReport.param_max_value = 50;
    propReport.param_default_value = 45;
    propReport.next_param_number = 0;

    node->getRequestsManager().onRespond( ConfigurationPropertiesRequest( 1, 0, 2 ), propReport );

    EXPECT_TRUE( cc->hasInterviewDone() );

    ZCc0x70Configuration::Parameter expectations[] = { { .m_id = 1,
                                                         .m_format = ConfigurationFormat::BitField,
                                                         .m_size = 1,
                                                         .m_readOnly = false,
                                                         .m_minValue = 1,
                                                         .m_maxValue = 100,
                                                         .m_defaultValue = 50,
                                                         .m_name = "testName1",
                                                         .m_info = "testInfo1" },
                                                       { .m_id = 2,
                                                         .m_format = ConfigurationFormat::Enumerated,
                                                         .m_size = 4,
                                                         .m_readOnly = false,
                                                         .m_minValue = 30,
                                                         .m_maxValue = 50,
                                                         .m_defaultValue = 45,
                                                         .m_name = "testName2",
                                                         .m_info = "testInfo2" } };

    int counter = 0;
    cc->forEachV3Parameter( [&expectations, &counter]( const ZCc0x70Configuration::Parameter actual ) {
        GTEST_ASSERT_LT( counter, 2 );
        const auto& expected = expectations[counter];

        EXPECT_EQ( actual.m_id, expected.m_id );
        EXPECT_EQ( actual.m_format, expected.m_format );
        EXPECT_EQ( actual.m_size, expected.m_size );
        EXPECT_EQ( actual.m_readOnly, expected.m_readOnly );
        EXPECT_EQ( actual.m_minValue, expected.m_minValue );
        EXPECT_EQ( actual.m_maxValue, expected.m_maxValue );
        EXPECT_EQ( actual.m_defaultValue, expected.m_defaultValue );
        EXPECT_EQ( actual.m_name, expected.m_name );
        EXPECT_EQ( actual.m_info, expected.m_info );

        ++counter;
    } );

    EXPECT_EQ( counter, 2 );
}

TEST_F( ZCcConfigurationSuite, interviewV3_ComplicatedName ) {
    getNodesProvider().nodeCreate( 1 );
    auto node = getNodesProvider().onFindNode( 1 );

    MockFunction<void( RequestFailTypeOpt )> callback;

    node->addCommandClass( 0, ZNET_COMMAND_CLASS_CONFIGURATION, 3 );
    auto cc = node->getCommandClass<ZCc0x70Configuration>( 0 );

    EXPECT_CALL( callback, Call( _ ) ).Times( 0 );

    EXPECT_TRUE( cc->interview( callback.AsStdFunction(), {} ) );

    znet_configuration_properties_report_t propReport;
    memset( &propReport, 0, sizeof( propReport ) );
    propReport.next_param_number = 1;

    node->getRequestsManager().onRespond( ConfigurationUnknownPropertiesRequest( 1, 0, 0 ), propReport );

    EXPECT_FALSE( cc->hasInterviewDone() );

    // Send first name part
    uint8_t buffName[sizeof( znet_configuration_name_report_t ) + 255];
    memset( buffName, 0, sizeof( znet_configuration_name_report_t ) + 255 );
    znet_configuration_name_report_t* nameReport = (znet_configuration_name_report_t*)&buffName;

    nameReport->param_number = 1;
    memcpy( nameReport->data, "part1_", 6 );
    nameReport->name_length = 6;
    nameReport->rep_to_follows = 2;

    node->getRequestsManager().onRespond( ConfigurationNameRequest( 1, 0, 1, true ), *nameReport );

    // Try to send first part one more time
    node->getRequestsManager().onRespond( ConfigurationNameRequest( 1, 0, 1, false ), *nameReport );

    // Send second name part
    memcpy( nameReport->data, "part2_", 6 );
    nameReport->name_length = 6;
    nameReport->rep_to_follows = 1;

    node->getRequestsManager().onRespond( ConfigurationNameRequest( 1, 0, 1, false ), *nameReport );

    // Try to send second part one more time
    node->getRequestsManager().onRespond( ConfigurationNameRequest( 1, 0, 1, false ), *nameReport );

    // Send third name part
    memcpy( nameReport->data, "part3", 5 );
    nameReport->name_length = 5;
    nameReport->rep_to_follows = 0;

    node->getRequestsManager().onRespond( ConfigurationNameRequest( 1, 0, 1, false ), *nameReport );

    uint8_t buffInfo[sizeof( znet_configuration_info_report_t ) + 255];
    memset( buffInfo, 0, sizeof( znet_configuration_info_report_t ) + 255 );
    znet_configuration_info_report_t* infoReport = (znet_configuration_info_report_t*)&buffInfo;

    infoReport->param_number = 1;
    memcpy( infoReport->data, "testInfo1", 9 );
    infoReport->info_length = 9;

    node->getRequestsManager().onRespond( ConfigurationInfoRequest( 1, 0, 1, true ), *infoReport );

    EXPECT_FALSE( cc->hasInterviewDone() );
    EXPECT_CALL( callback, Call( RequestFailTypeOpt{} ) ).Times( 1 );

    memset( &propReport, 0, sizeof( propReport ) );
    propReport.param_number = 1;
    propReport.param_format = 3;
    propReport.param_size = 1;
    propReport.param_min_value = 1;
    propReport.param_max_value = 100;
    propReport.param_default_value = 50;
    propReport.next_param_number = 0;

    node->getRequestsManager().onRespond( ConfigurationPropertiesRequest( 1, 0, 1 ), propReport );

    EXPECT_TRUE( cc->hasInterviewDone() );

    int counter = 0;
    cc->forEachV3Parameter( [&counter]( const ZCc0x70Configuration::Parameter actual ) {
        ASSERT_EQ( counter, 0 );
        EXPECT_EQ( actual.m_name, "part1_part2_part3" );

        ++counter;
    } );
}

TEST_F( ZCcConfigurationSuite, interviewV3_ComplicatedInfo ) {
    getNodesProvider().nodeCreate( 1 );
    auto node = getNodesProvider().onFindNode( 1 );

    MockFunction<void( RequestFailTypeOpt )> callback;

    node->addCommandClass( 0, ZNET_COMMAND_CLASS_CONFIGURATION, 3 );
    auto cc = node->getCommandClass<ZCc0x70Configuration>( 0 );

    EXPECT_CALL( callback, Call( _ ) ).Times( 0 );

    EXPECT_TRUE( cc->interview( callback.AsStdFunction(), {} ) );

    znet_configuration_properties_report_t propReport;
    memset( &propReport, 0, sizeof( propReport ) );
    propReport.next_param_number = 1;

    node->getRequestsManager().onRespond( ConfigurationUnknownPropertiesRequest( 1, 0, 0 ), propReport );

    EXPECT_FALSE( cc->hasInterviewDone() );

    uint8_t buffName[sizeof( znet_configuration_name_report_t ) + 255];
    memset( buffName, 0, sizeof( znet_configuration_name_report_t ) + 255 );
    znet_configuration_name_report_t* nameReport = (znet_configuration_name_report_t*)&buffName;

    nameReport->param_number = 1;
    memcpy( nameReport->data, "test_name", 9 );
    nameReport->name_length = 9;

    node->getRequestsManager().onRespond( ConfigurationNameRequest( 1, 0, 1, true ), *nameReport );

    // Send first info part
    uint8_t buffInfo[sizeof( znet_configuration_info_report_t ) + 255];
    memset( buffInfo, 0, sizeof( znet_configuration_info_report_t ) + 255 );
    znet_configuration_info_report_t* infoReport = (znet_configuration_info_report_t*)&buffInfo;

    infoReport->param_number = 1;
    memcpy( infoReport->data, "part1_", 6 );
    infoReport->info_length = 6;
    infoReport->rep_to_follows = 2;

    node->getRequestsManager().onRespond( ConfigurationInfoRequest( 1, 0, 1, true ), *infoReport );

    // Try to send first part one more time
    node->getRequestsManager().onRespond( ConfigurationInfoRequest( 1, 0, 1, false ), *infoReport );

    // Send second info part
    memcpy( infoReport->data, "part2_", 6 );
    infoReport->info_length = 6;
    infoReport->rep_to_follows = 1;

    node->getRequestsManager().onRespond( ConfigurationInfoRequest( 1, 0, 1, false ), *infoReport );

    // Try to send second part one more time
    node->getRequestsManager().onRespond( ConfigurationInfoRequest( 1, 0, 1, false ), *infoReport );

    // Send third info part
    memcpy( infoReport->data, "part3", 5 );
    infoReport->info_length = 5;
    infoReport->rep_to_follows = 0;

    node->getRequestsManager().onRespond( ConfigurationInfoRequest( 1, 0, 1, false ), *infoReport );

    EXPECT_FALSE( cc->hasInterviewDone() );
    EXPECT_CALL( callback, Call( RequestFailTypeOpt{} ) ).Times( 1 );

    memset( &propReport, 0, sizeof( propReport ) );
    propReport.param_number = 1;
    propReport.param_format = 3;
    propReport.param_size = 1;
    propReport.param_min_value = 1;
    propReport.param_max_value = 100;
    propReport.param_default_value = 50;
    propReport.next_param_number = 0;

    node->getRequestsManager().onRespond( ConfigurationPropertiesRequest( 1, 0, 1 ), propReport );

    EXPECT_TRUE( cc->hasInterviewDone() );

    int counter = 0;
    cc->forEachV3Parameter( [&counter]( const ZCc0x70Configuration::Parameter actual ) {
        ASSERT_EQ( counter, 0 );
        EXPECT_EQ( actual.m_info, "part1_part2_part3" );

        ++counter;
    } );
}

TEST_F( ZCcConfigurationSuite, interviewV4_Simple ) {
    getNodesProvider().nodeCreate( 1 );
    auto node = getNodesProvider().onFindNode( 1 );

    MockFunction<void( RequestFailTypeOpt )> callback;

    node->addCommandClass( 0, ZNET_COMMAND_CLASS_CONFIGURATION, 4 );
    auto cc = node->getCommandClass<ZCc0x70Configuration>( 0 );

    EXPECT_CALL( callback, Call( _ ) ).Times( 0 );

    EXPECT_TRUE( cc->interview( callback.AsStdFunction(), {} ) );

    znet_configuration_properties_report_t propReport;
    memset( &propReport, 0, sizeof( propReport ) );
    propReport.next_param_number = 1;

    node->getRequestsManager().onRespond( ConfigurationUnknownPropertiesRequest( 1, 0, 0 ), propReport );

    // Start first parameter interview
    EXPECT_FALSE( cc->hasInterviewDone() );

    uint8_t buffName[sizeof( znet_configuration_name_report_t ) + 255];
    memset( buffName, 0, sizeof( znet_configuration_name_report_t ) + 255 );
    znet_configuration_name_report_t* nameReport = (znet_configuration_name_report_t*)&buffName;

    nameReport->param_number = 1;
    memcpy( nameReport->data, "testName1", 9 );
    nameReport->name_length = 9;

    node->getRequestsManager().onRespond( ConfigurationNameRequest( 1, 0, 1, true ), *nameReport );

    uint8_t buffInfo[sizeof( znet_configuration_info_report_t ) + 255];
    memset( buffInfo, 0, sizeof( znet_configuration_info_report_t ) + 255 );
    znet_configuration_info_report_t* infoReport = (znet_configuration_info_report_t*)&buffInfo;

    infoReport->param_number = 1;
    memcpy( infoReport->data, "testInfo1", 9 );
    infoReport->info_length = 9;

    node->getRequestsManager().onRespond( ConfigurationInfoRequest( 1, 0, 1, true ), *infoReport );

    memset( &propReport, 0, sizeof( propReport ) );
    propReport.param_number = 1;
    propReport.param_format = 3;
    propReport.param_size = 1;
    propReport.read_only = false;
    propReport.param_min_value = 1;
    propReport.param_max_value = 100;
    propReport.param_default_value = 50;
    propReport.next_param_number = 2;

    node->getRequestsManager().onRespond( ConfigurationPropertiesRequest( 1, 0, 1 ), propReport );

    // Start second parameter interview
    EXPECT_FALSE( cc->hasInterviewDone() );

    memset( buffName, 0, sizeof( znet_configuration_name_report_t ) + 255 );

    nameReport->param_number = 2;
    memcpy( nameReport->data, "testName2", 9 );
    nameReport->name_length = 9;

    node->getRequestsManager().onRespond( ConfigurationNameRequest( 1, 0, 2, true ), *nameReport );

    memset( buffInfo, 0, sizeof( znet_configuration_info_report_t ) + 255 );

    infoReport->param_number = 2;
    memcpy( infoReport->data, "testInfo2", 9 );
    infoReport->info_length = 9;

    node->getRequestsManager().onRespond( ConfigurationInfoRequest( 1, 0, 2, true ), *infoReport );

    EXPECT_FALSE( cc->hasInterviewDone() );
    EXPECT_CALL( callback, Call( RequestFailTypeOpt{} ) ).Times( 1 );

    memset( &propReport, 0, sizeof( propReport ) );
    propReport.param_number = 2;
    propReport.param_format = 2;
    propReport.param_size = 4;
    propReport.read_only = true;
    propReport.param_min_value = 30;
    propReport.param_max_value = 50;
    propReport.param_default_value = 45;
    propReport.next_param_number = 0;

    node->getRequestsManager().onRespond( ConfigurationPropertiesRequest( 1, 0, 2 ), propReport );

    EXPECT_TRUE( cc->hasInterviewDone() );

    ZCc0x70Configuration::Parameter expectations[] = { { .m_id = 1,
                                                         .m_format = ConfigurationFormat::BitField,
                                                         .m_size = 1,
                                                         .m_readOnly = false,
                                                         .m_minValue = 1,
                                                         .m_maxValue = 100,
                                                         .m_defaultValue = 50,
                                                         .m_name = "testName1",
                                                         .m_info = "testInfo1" },
                                                       { .m_id = 2,
                                                         .m_format = ConfigurationFormat::Enumerated,
                                                         .m_size = 4,
                                                         .m_readOnly = true,
                                                         .m_minValue = 30,
                                                         .m_maxValue = 50,
                                                         .m_defaultValue = 45,
                                                         .m_name = "testName2",
                                                         .m_info = "testInfo2" } };

    int counter = 0;
    cc->forEachV3Parameter( [&expectations, &counter]( const ZCc0x70Configuration::Parameter actual ) {
        GTEST_ASSERT_LT( counter, 2 );
        const auto& expected = expectations[counter];

        EXPECT_EQ( actual.m_id, expected.m_id );
        EXPECT_EQ( actual.m_format, expected.m_format );
        EXPECT_EQ( actual.m_size, expected.m_size );
        EXPECT_EQ( actual.m_readOnly, expected.m_readOnly );
        EXPECT_EQ( actual.m_minValue, expected.m_minValue );
        EXPECT_EQ( actual.m_maxValue, expected.m_maxValue );
        EXPECT_EQ( actual.m_defaultValue, expected.m_defaultValue );
        EXPECT_EQ( actual.m_name, expected.m_name );
        EXPECT_EQ( actual.m_info, expected.m_info );

        ++counter;
    } );

    EXPECT_EQ( counter, 2 );
}

TEST_F( ZCcConfigurationSuite, interviewV3_Failed ) {
    getNodesProvider().nodeCreate( 1 );
    auto node = getNodesProvider().onFindNode( 1 );

    MockFunction<void( RequestFailTypeOpt )> callback;

    node->addCommandClass( 0, ZNET_COMMAND_CLASS_CONFIGURATION, 3 );
    auto cc = node->getCommandClass<ZCc0x70Configuration>( 0 );

    // First command sending failed
    {
        MockFunction<void( RequestFailTypeOpt )> callback;
        EXPECT_CALL( callback, Call( _ ) ).Times( 0 );

        getRequestsManager().addFailedRequest( ConfigurationUnknownPropertiesRequest( 1, 0, 0 ) );

        EXPECT_FALSE( cc->interview( callback.AsStdFunction(), {} ) );
        EXPECT_FALSE( cc->hasInterviewDone() );
    }

    // First command report failed
    {
        MockFunction<void( RequestFailTypeOpt )> callback;
        EXPECT_CALL( callback, Call( _ ) ).Times( 0 );

        EXPECT_CALL( callback, Call( RequestFailTypeOpt( RequestFailType::ReportedFail ) ) ).Times( 1 );

        EXPECT_TRUE( cc->interview( callback.AsStdFunction(), {} ) );

        node->getRequestsManager().onFail( ConfigurationUnknownPropertiesRequest( 1, 0, 0 ) );

        EXPECT_FALSE( cc->hasInterviewDone() );
    }

    znet_configuration_properties_report_t propReport;
    memset( &propReport, 0, sizeof( propReport ) );
    propReport.next_param_number = 1;

    // Name command sending failed
    {
        MockFunction<void( RequestFailTypeOpt )> callback;
        EXPECT_CALL( callback, Call( _ ) ).Times( 0 );

        getRequestsManager().addFailedRequest( ConfigurationNameRequest( 1, 0, 1, true ) );

        EXPECT_TRUE( cc->interview( callback.AsStdFunction(), {} ) );

        EXPECT_CALL( callback, Call( RequestFailTypeOpt( RequestFailType::SendingError ) ) ).Times( 1 );

        node->getRequestsManager().onRespond( ConfigurationUnknownPropertiesRequest( 1, 0, 0 ), propReport );
        getTimerProvider().addTime( DefaultPostponedRequestDelay );

        EXPECT_FALSE( cc->hasInterviewDone() );
    }

    // Name command report failed
    {
        MockFunction<void( RequestFailTypeOpt )> callback;
        EXPECT_CALL( callback, Call( _ ) ).Times( 0 );

        EXPECT_TRUE( cc->interview( callback.AsStdFunction(), {} ) );

        node->getRequestsManager().onRespond( ConfigurationUnknownPropertiesRequest( 1, 0, 0 ), propReport );

        EXPECT_CALL( callback, Call( RequestFailTypeOpt( RequestFailType::ReportedFail ) ) ).Times( 1 );

        node->getRequestsManager().onFail( ConfigurationNameRequest( 1, 0, 1, true ) );

        EXPECT_FALSE( cc->hasInterviewDone() );
    }

    uint8_t buffName[sizeof( znet_configuration_name_report_t ) + 255];
    memset( buffName, 0, sizeof( znet_configuration_name_report_t ) + 255 );
    znet_configuration_name_report_t* nameReport = (znet_configuration_name_report_t*)&buffName;

    nameReport->param_number = 1;
    memcpy( nameReport->data, "testName1", 9 );
    nameReport->name_length = 9;

    // Info command sending failed
    {
        MockFunction<void( RequestFailTypeOpt )> callback;
        EXPECT_CALL( callback, Call( _ ) ).Times( 0 );

        getRequestsManager().addFailedRequest( ConfigurationInfoRequest( 1, 0, 1, true ) );

        EXPECT_TRUE( cc->interview( callback.AsStdFunction(), {} ) );

        node->getRequestsManager().onRespond( ConfigurationUnknownPropertiesRequest( 1, 0, 0 ), propReport );

        EXPECT_CALL( callback, Call( RequestFailTypeOpt( RequestFailType::SendingError ) ) ).Times( 1 );

        node->getRequestsManager().onRespond( ConfigurationNameRequest( 1, 0, 1, true ), *nameReport );
        getTimerProvider().addTime( DefaultPostponedRequestDelay );

        EXPECT_FALSE( cc->hasInterviewDone() );
    }

    // Info command report failed
    {
        MockFunction<void( RequestFailTypeOpt )> callback;
        EXPECT_CALL( callback, Call( _ ) ).Times( 0 );

        EXPECT_TRUE( cc->interview( callback.AsStdFunction(), {} ) );

        node->getRequestsManager().onRespond( ConfigurationUnknownPropertiesRequest( 1, 0, 0 ), propReport );
        node->getRequestsManager().onRespond( ConfigurationNameRequest( 1, 0, 1, true ), *nameReport );

        EXPECT_CALL( callback, Call( RequestFailTypeOpt( RequestFailType::ReportedFail ) ) ).Times( 1 );

        node->getRequestsManager().onFail( ConfigurationInfoRequest( 1, 0, 1, true ) );

        EXPECT_FALSE( cc->hasInterviewDone() );
    }

    uint8_t buffInfo[sizeof( znet_configuration_info_report_t ) + 255];
    memset( buffInfo, 0, sizeof( znet_configuration_info_report_t ) + 255 );
    znet_configuration_info_report_t* infoReport = (znet_configuration_info_report_t*)&buffInfo;

    infoReport->param_number = 1;
    memcpy( infoReport->data, "testInfo1", 9 );
    infoReport->info_length = 9;

    // Properties command sending failed
    {
        MockFunction<void( RequestFailTypeOpt )> callback;
        EXPECT_CALL( callback, Call( _ ) ).Times( 0 );

        getRequestsManager().addFailedRequest( ConfigurationPropertiesRequest( 1, 0, 1 ) );

        EXPECT_TRUE( cc->interview( callback.AsStdFunction(), {} ) );

        node->getRequestsManager().onRespond( ConfigurationUnknownPropertiesRequest( 1, 0, 0 ), propReport );
        node->getRequestsManager().onRespond( ConfigurationNameRequest( 1, 0, 1, true ), *nameReport );

        EXPECT_CALL( callback, Call( RequestFailTypeOpt( RequestFailType::SendingError ) ) ).Times( 1 );

        node->getRequestsManager().onRespond( ConfigurationInfoRequest( 1, 0, 1, true ), *infoReport );
        getTimerProvider().addTime( DefaultPostponedRequestDelay );

        EXPECT_FALSE( cc->hasInterviewDone() );
    }

    // Properties command report failed
    {
        MockFunction<void( RequestFailTypeOpt )> callback;
        EXPECT_CALL( callback, Call( _ ) ).Times( 0 );

        EXPECT_TRUE( cc->interview( callback.AsStdFunction(), {} ) );

        node->getRequestsManager().onRespond( ConfigurationUnknownPropertiesRequest( 1, 0, 0 ), propReport );
        node->getRequestsManager().onRespond( ConfigurationNameRequest( 1, 0, 1, true ), *nameReport );
        node->getRequestsManager().onRespond( ConfigurationInfoRequest( 1, 0, 1, true ), *infoReport );

        EXPECT_CALL( callback, Call( RequestFailTypeOpt( RequestFailType::ReportedFail ) ) ).Times( 1 );

        node->getRequestsManager().onFail( ConfigurationPropertiesRequest( 1, 0, 1 ) );

        EXPECT_FALSE( cc->hasInterviewDone() );
    }
}

TEST_F( ZCcConfigurationSuite, requestValueV1 ) {
    getNodesProvider().nodeCreate( 1 );
    auto node = getNodesProvider().onFindNode( 1 );

    node->addCommandClass( 0, ZNET_COMMAND_CLASS_CONFIGURATION, 1 );
    auto cc = node->getCommandClass<ZCc0x70Configuration>( 0 );

    cc->interview( {}, {} );

    cc->addConfiguration( 1, 2 );
    cc->addConfiguration( 10, 2 );
    cc->addConfiguration( 15, 4 );

    MockFunction<void( RequestFailTypeOpt )> callback;

    EXPECT_CALL( callback, Call( _ ) ).Times( 0 );

    EXPECT_TRUE( cc->requestValue( createRequestValue( callback ) ) );

    getRequestsManager().onRespond( ConfigurationRequest( 1, 0, 1 ), znet_configuration_report_t{ 1, 2, 0 } );
    getRequestsManager().onRespond( ConfigurationRequest( 1, 0, 10 ), znet_configuration_report_t{ 1, 2, 0 } );

    EXPECT_CALL( callback, Call( RequestFailTypeOpt{} ) ).Times( 1 );
    getRequestsManager().onRespond( ConfigurationRequest( 1, 0, 15 ), znet_configuration_report_t{ 1, 4, 0 } );
}

TEST_F( ZCcConfigurationSuite, requestValueV1_fail ) {
    getNodesProvider().nodeCreate( 1 );
    auto node = getNodesProvider().onFindNode( 1 );

    node->addCommandClass( 0, ZNET_COMMAND_CLASS_CONFIGURATION, 1 );
    auto cc = node->getCommandClass<ZCc0x70Configuration>( 0 );

    cc->interview( {}, {} );

    cc->addConfiguration( 1, 2 );
    cc->addConfiguration( 10, 2 );
    cc->addConfiguration( 15, 4 );

    // reported fail
    {
        MockFunction<void( RequestFailTypeOpt )> callback;
        EXPECT_CALL( callback, Call( _ ) ).Times( 0 );

        EXPECT_TRUE( cc->requestValue( createRequestValue( callback ) ) );

        getRequestsManager().onRespond( ConfigurationRequest( 1, 0, 1 ), znet_configuration_report_t{ 1, 2, 0 } );
        getRequestsManager().onRespond( ConfigurationRequest( 1, 0, 10 ), znet_configuration_report_t{ 1, 2, 0 } );

        EXPECT_CALL( callback, Call( RequestFailTypeOpt{ RequestFailType::ReportedFail } ) ).Times( 1 );
        getRequestsManager().onFail( ConfigurationRequest( 1, 0, 15 ) );
    }

    // timeout fail: report no fail if timeout, TBD
    {
        MockFunction<void( RequestFailTypeOpt )> callback;
        EXPECT_CALL( callback, Call( _ ) ).Times( 0 );

        EXPECT_TRUE( cc->requestValue( createRequestValue( callback ) ) );

        getRequestsManager().onRespond( ConfigurationRequest( 1, 0, 1 ), znet_configuration_report_t{ 1, 2, 0 } );

        for( size_t i = 0; i < MaxRetryRequestNumber + 1; ++i ) {
            getRequestsManager().onRequestSent( ConfigurationRequest( 1, 0, 10 ) );
            getTimerProvider().addTime( RequestCheckingPeriod );
        }

        EXPECT_CALL( callback, Call( RequestFailTypeOpt{} ) ).Times( 1 );
        getRequestsManager().onRespond( ConfigurationRequest( 1, 0, 15 ), znet_configuration_report_t{ 1, 2, 0 } );
    }

    // sending fail
    {
        MockFunction<void( RequestFailTypeOpt )> callback;
        EXPECT_CALL( callback, Call( _ ) ).Times( 0 );

        getRequestsManager().addFailedRequest( ConfigurationRequest( 1, 0, 10 ) );

        EXPECT_TRUE( cc->requestValue( createRequestValue( callback ) ) );

        EXPECT_CALL( callback, Call( RequestFailTypeOpt{ RequestFailType::SendingError } ) ).Times( 1 );
        getRequestsManager().onRespond( ConfigurationRequest( 1, 0, 1 ), znet_configuration_report_t{ 1, 2, 0 } );
    }

    // first sending fail
    {
        MockFunction<void( RequestFailTypeOpt )> callback;
        EXPECT_CALL( callback, Call( _ ) ).Times( 0 );

        getRequestsManager().addFailedRequest( ConfigurationRequest( 1, 0, 1 ) );

        EXPECT_FALSE( cc->requestValue( createRequestValue( callback ) ) );
    }
}

TEST_F( ZCcConfigurationSuite, requestValueV3 ) {
    getNodesProvider().nodeCreate( 1 );
    auto node = getNodesProvider().onFindNode( 1 );

    node->addCommandClass( 0, ZNET_COMMAND_CLASS_CONFIGURATION, 3 );
    auto cc = node->getCommandClass<ZCc0x70Configuration>( 0 );

    MockFunction<void( RequestFailTypeOpt )> callback;

    generateV3DefaultConfigurations( callback );

    EXPECT_CALL( callback, Call( _ ) ).Times( 0 );

    EXPECT_TRUE( cc->requestValue( createRequestValue( callback ) ) );

    getRequestsManager().onRespond( ConfigurationRequest( 1, 0, 1 ), znet_configuration_report_t{ 1, 1, 0 } );

    uint8_t buff[sizeof( znet_configuration_bulk_report_t ) + 4];
    memset( buff, 0, sizeof( znet_configuration_bulk_report_t ) + 4 );
    znet_configuration_bulk_report_t* bulkReport = (znet_configuration_bulk_report_t*)&buff;
    bulkReport->param_offset = 256;
    bulkReport->params_count = 1;
    bulkReport->param_size = 4;
    bulkReport->rep_to_follows = 0;
    bulkReport->values[0] = 1234;

    getRequestsManager().onRespond( ConfigurationBulkRequest( 1, 0, 256, 1 ), *bulkReport );

    EXPECT_CALL( callback, Call( RequestFailTypeOpt{} ) ).Times( 1 );
    getRequestsManager().onRespond( ConfigurationRequest( 1, 0, 2 ), znet_configuration_report_t{ 2, 4, 0 } );
}

TEST_F( ZCcConfigurationSuite, requestValueV3_failed ) {
    getNodesProvider().nodeCreate( 1 );
    auto node = getNodesProvider().onFindNode( 1 );

    node->addCommandClass( 0, ZNET_COMMAND_CLASS_CONFIGURATION, 3 );
    auto cc = node->getCommandClass<ZCc0x70Configuration>( 0 );

    MockFunction<void( RequestFailTypeOpt )> callback;

    generateV3DefaultConfigurations( callback );

    uint8_t buff[sizeof( znet_configuration_bulk_report_t ) + 4];
    memset( buff, 0, sizeof( znet_configuration_bulk_report_t ) + 4 );
    znet_configuration_bulk_report_t* bulkReport = (znet_configuration_bulk_report_t*)&buff;
    bulkReport->param_offset = 256;
    bulkReport->params_count = 1;
    bulkReport->param_size = 4;
    bulkReport->rep_to_follows = 0;
    bulkReport->values[0] = 1234;

    // reported fail
    {
        MockFunction<void( RequestFailTypeOpt )> callback;
        EXPECT_CALL( callback, Call( _ ) ).Times( 0 );

        EXPECT_TRUE( cc->requestValue( createRequestValue( callback ) ) );

        getRequestsManager().onRespond( ConfigurationRequest( 1, 0, 1 ), znet_configuration_report_t{ 1, 1, 0 } );
        getRequestsManager().onRespond( ConfigurationBulkRequest( 1, 0, 256, 1 ), *bulkReport );

        EXPECT_CALL( callback, Call( RequestFailTypeOpt{ RequestFailType::ReportedFail } ) ).Times( 1 );
        getRequestsManager().onFail( ConfigurationRequest( 1, 0, 2 ) );
    }

    // bulk reported fail
    {
        MockFunction<void( RequestFailTypeOpt )> callback;
        EXPECT_CALL( callback, Call( _ ) ).Times( 0 );

        EXPECT_TRUE( cc->requestValue( createRequestValue( callback ) ) );

        getRequestsManager().onRespond( ConfigurationRequest( 1, 0, 1 ), znet_configuration_report_t{ 1, 1, 0 } );

        EXPECT_CALL( callback, Call( RequestFailTypeOpt{ RequestFailType::ReportedFail } ) ).Times( 1 );
        getRequestsManager().onFail( ConfigurationBulkRequest( 1, 0, 256, 1 ) );
    }

    // sending fail
    {
        MockFunction<void( RequestFailTypeOpt )> callback;
        EXPECT_CALL( callback, Call( _ ) ).Times( 0 );

        getRequestsManager().addFailedRequest( ConfigurationRequest( 1, 0, 2 ) );

        EXPECT_TRUE( cc->requestValue( createRequestValue( callback ) ) );

        getRequestsManager().onRespond( ConfigurationRequest( 1, 0, 1 ), znet_configuration_report_t{ 1, 1, 0 } );

        EXPECT_CALL( callback, Call( RequestFailTypeOpt{ RequestFailType::SendingError } ) ).Times( 1 );
        getRequestsManager().onRespond( ConfigurationBulkRequest( 1, 0, 256, 1 ), *bulkReport );
        getTimerProvider().addTime( DefaultPostponedRequestDelay );
    }

    // bulk sending fail
    {
        MockFunction<void( RequestFailTypeOpt )> callback;
        EXPECT_CALL( callback, Call( _ ) ).Times( 0 );

        getRequestsManager().addFailedRequest( ConfigurationBulkRequest( 1, 0, 256, 1 ) );

        EXPECT_TRUE( cc->requestValue( createRequestValue( callback ) ) );

        EXPECT_CALL( callback, Call( RequestFailTypeOpt{ RequestFailType::SendingError } ) ).Times( 1 );
        getRequestsManager().onRespond( ConfigurationRequest( 1, 0, 1 ), znet_configuration_report_t{ 1, 1, 0 } );
        getTimerProvider().addTime( DefaultPostponedRequestDelay );
    }
}

TEST_F( ZCcConfigurationSuite, interviewV3_NotCompletedInfoAndNameReports ) {
    getNodesProvider().nodeCreate( 1 );
    auto node = getNodesProvider().onFindNode( 1 );

    MockFunction<void( RequestFailTypeOpt )> callback;

    node->addCommandClass( 0, ZNET_COMMAND_CLASS_CONFIGURATION, 3 );
    auto cc = node->getCommandClass<ZCc0x70Configuration>( 0 );

    EXPECT_CALL( callback, Call( _ ) ).Times( 0 );

    EXPECT_TRUE( cc->interview( callback.AsStdFunction(), {} ) );

    znet_configuration_properties_report_t propReport;
    memset( &propReport, 0, sizeof( propReport ) );
    propReport.next_param_number = 1;

    node->getRequestsManager().onRespond( ConfigurationUnknownPropertiesRequest( 1, 0, 0 ), propReport );

    EXPECT_FALSE( cc->hasInterviewDone() );

    // Send first name part
    uint8_t buffName[sizeof( znet_configuration_name_report_t ) + 255];
    memset( buffName, 0, sizeof( znet_configuration_name_report_t ) + 255 );
    znet_configuration_name_report_t* nameReport = (znet_configuration_name_report_t*)&buffName;

    nameReport->param_number = 1;
    memcpy( nameReport->data, "name1_", 6 );
    nameReport->name_length = 6;
    nameReport->rep_to_follows = 2;

    node->getRequestsManager().onRespond( ConfigurationNameRequest( 1, 0, 1, true ), *nameReport );

    memcpy( nameReport->data, "name2_", 5 );
    nameReport->rep_to_follows = 1;
    node->getRequestsManager().onRespond( ConfigurationNameRequest( 1, 0, 1, true ), *nameReport );

    getTimerProvider().addTime( std::chrono::seconds( 25 ) );

    uint8_t buffInfo[sizeof( znet_configuration_info_report_t ) + 255];
    memset( buffInfo, 0, sizeof( znet_configuration_info_report_t ) + 255 );
    znet_configuration_info_report_t* infoReport = (znet_configuration_info_report_t*)&buffInfo;

    infoReport->param_number = 1;
    memcpy( infoReport->data, "info1_", 6 );
    infoReport->info_length = 6;
    infoReport->rep_to_follows = 20;

    node->getRequestsManager().onRespond( ConfigurationInfoRequest( 1, 0, 1, true ), *infoReport );

    getTimerProvider().addTime( std::chrono::seconds( 25 ) );

    EXPECT_FALSE( cc->hasInterviewDone() );
    EXPECT_CALL( callback, Call( RequestFailTypeOpt{} ) ).Times( 1 );

    memset( &propReport, 0, sizeof( propReport ) );
    propReport.param_number = 1;
    propReport.param_format = 3;
    propReport.param_size = 1;
    propReport.param_min_value = 1;
    propReport.param_max_value = 100;
    propReport.param_default_value = 50;
    propReport.next_param_number = 0;

    node->getRequestsManager().onRespond( ConfigurationPropertiesRequest( 1, 0, 1 ), propReport );

    EXPECT_TRUE( cc->hasInterviewDone() );

    int counter = 0;
    cc->forEachV3Parameter( [&counter]( const ZCc0x70Configuration::Parameter actual ) {
        ASSERT_EQ( counter, 0 );
        EXPECT_EQ( actual.m_name, "name1_name2_" );
        EXPECT_EQ( actual.m_info, "info1_" );

        EXPECT_EQ( actual.m_id, 1 );
        EXPECT_EQ( actual.m_format, ConfigurationFormat::BitField );
        EXPECT_EQ( actual.m_size, 1 );
        EXPECT_EQ( actual.m_readOnly, false );
        EXPECT_EQ( actual.m_minValue, 1 );
        EXPECT_EQ( actual.m_maxValue, 100 );
        EXPECT_EQ( actual.m_defaultValue, 50 );

        ++counter;
    } );

    EXPECT_EQ( counter, 1 );
}

}  // namespace ut
}  // namespace znet
