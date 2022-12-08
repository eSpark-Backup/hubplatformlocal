#include <znet/znet_node.hpp>
#include <znet/cc_0x86_version.hpp>

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "znet_node_requests_managing_fixture.h"
#include "fake_file.h"

namespace znet {

namespace ut {

using ::testing::_;
using ::testing::Invoke;
using ::testing::MockFunction;

class ZCcVersionTestSuite : public ::testing::Test, public NodeRequestsManagingFixture {
public:
};

TEST_F( ZCcVersionTestSuite, interview_v1 ) {
    getNodesProvider().nodeCreate( 1 );
    auto node = getNodesProvider().onFindNode( 1 );

    MockFunction<void( RequestFailTypeOpt )> callback;

    node->addCommandClass( 0, ZNET_COMMAND_CLASS_VERSION, 2 );
    auto cc = node->getCommandClass<ZCc0x86Version>( 0 );

    EXPECT_CALL( callback, Call( _ ) ).Times( 0 );

    EXPECT_TRUE( cc->interview( callback.AsStdFunction(), {} ) );

    EXPECT_FALSE( cc->hasInterviewDone() );
    EXPECT_CALL( callback, Call( RequestFailTypeOpt{} ) ).Times( 1 );

    znet_version_report_t versionReport{};
    versionReport.lib_type = 1;
    versionReport.proto.ver = 2;
    versionReport.proto.sub_ver = 3;
    versionReport.hard_ver = 4;

    node->getRequestsManager().onRespond( VersionRequest( 1 ), versionReport );

    EXPECT_TRUE( cc->hasInterviewDone() );
    EXPECT_EQ( cc->getLibraryType(), utils::optional<std::string>( "static_controller" ) );
    EXPECT_EQ( cc->getHardwareVersion(), utils::optional<uint8_t>( 4 ) );
    EXPECT_EQ( cc->getZwaveProtocolVersion(), utils::optional<std::string>( "2.3" ) );

    // TODO: add expectations here
}

TEST_F( ZCcVersionTestSuite, interview_v3 ) {
    getNodesProvider().nodeCreate( 1 );
    auto node = getNodesProvider().onFindNode( 1 );

    MockFunction<void( RequestFailTypeOpt )> callback;

    node->addCommandClass( 0, ZNET_COMMAND_CLASS_VERSION, 3 );
    auto cc = node->getCommandClass<ZCc0x86Version>( 0 );

    EXPECT_CALL( callback, Call( _ ) ).Times( 0 );

    EXPECT_TRUE( cc->interview( callback.AsStdFunction(), {} ) );

    node->getRequestsManager().onRespond( VersionRequest( 1 ), znet_version_report_t{} );

    EXPECT_FALSE( cc->hasInterviewDone() );
    EXPECT_CALL( callback, Call( RequestFailTypeOpt{} ) ).Times( 1 );

    znet_version_capabilities_report_t capabilitiesReport{};
    capabilitiesReport.command = 1;
    capabilitiesReport.version_supported = 1;
    capabilitiesReport.command_class_supported = 1;

    node->getRequestsManager().onRespond( VersionCapabilitiesRequest( 1 ), capabilitiesReport );

    EXPECT_TRUE( cc->hasInterviewDone() );

    // TODO: add expectations here
}

TEST_F( ZCcVersionTestSuite, interview_v3_softwareSupported ) {
    getNodesProvider().nodeCreate( 1 );
    auto node = getNodesProvider().onFindNode( 1 );

    MockFunction<void( RequestFailTypeOpt )> callback;

    node->addCommandClass( 0, ZNET_COMMAND_CLASS_VERSION, 3 );
    auto cc = node->getCommandClass<ZCc0x86Version>( 0 );

    EXPECT_CALL( callback, Call( _ ) ).Times( 0 );

    EXPECT_TRUE( cc->interview( callback.AsStdFunction(), {} ) );

    node->getRequestsManager().onRespond( VersionRequest( 1 ), znet_version_report_t{} );

    znet_version_capabilities_report_t capabilitiesReport{};
    capabilitiesReport.software_supported = 1;

    node->getRequestsManager().onRespond( VersionCapabilitiesRequest( 1 ), capabilitiesReport );

    EXPECT_FALSE( cc->hasInterviewDone() );
    EXPECT_CALL( callback, Call( RequestFailTypeOpt{} ) ).Times( 1 );

    znet_software_version_report_t softwareVrsionReport{};
    // TODO: fill report

    node->getRequestsManager().onRespond( SoftwareVersionRequest( 1 ), softwareVrsionReport );

    EXPECT_TRUE( cc->hasInterviewDone() );

    // TODO: add expectations here
}

}  // namespace ut
}  // namespace znet
