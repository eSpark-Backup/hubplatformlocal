#include "common_test_resources/fixtures/core_test_fixture.h"
#include "common_test_resources/resources/cored_resources.h"

#include <gtest/gtest.h>

/*

TESTPLAN

DONE    1. lua network socket timeout
DONE        1.0 socket set_timeout
DONE    2. lua network socket option
DONE        2.0 setoption broadcast
DONE        2.1 setoption ip-add-membership

*/

namespace HubPlatform {
namespace Tests {

class SocketNetworkLuadTests : public CoreNmaTest {
public:
    JsonObject checkUiBroadcast( const std::string& expectedValue, int line ) {
        JsonObject broadcast = waitForBroadcast( std::string( "hub.extensions.plugin.ui_broadcast" ) );
        EXPECT_EQ( broadcast.empty(), false );  // << "line: " << line;
        JsonObject params;
        EXPECT_TRUE( json_get_obj( broadcast, "result", params ) ) << "line: " << line;
        auto value = json_get_str( params, "value" );
        if( !expectedValue.empty() )
            EXPECT_EQ( value, expectedValue ) << "line: " << line;

        return broadcast;
    }
};

// resources
// gets network sockets commands
static char socketScript[] = "HUB:network_socket/scripts/socket_timeout.lua";

TEST_F( SocketNetworkLuadTests, setTimeout ) {
    auto timeoutMs = std::chrono::microseconds( 20 );
    auto request = replaceTokens( hubExtensionsPluginRun, { { "__SCRIPT__", socketScript } } );

    request[ "params" ][ "scriptParams" ] = JSON_EMPTY_OBJ;

    request[ "params" ][ "scriptParams" ][ "command" ] = "set_timeout";
    request[ "params" ][ "scriptParams" ][ "timeout" ] = timeoutMs.count();

    auto answer = nma().sendtoHubAndGetAnswer( request );
    auto error = answer[ "error" ];
    ASSERT_TRUE( error.empty() ) << error.dump( 4 );

    auto broadcast = checkUiBroadcast( "", __LINE__ );

    checkExtensionsPluginRunProgressBroadcast( "started", __LINE__ );
    checkExtensionsPluginRunProgressBroadcast( "finished", __LINE__ );
}

TEST_F( SocketNetworkLuadTests, setOptionBroadcast ) {
    auto request = replaceTokens( hubExtensionsPluginRun, { { "__SCRIPT__", socketScript } } );

    request[ "params" ][ "scriptParams" ] = JSON_EMPTY_OBJ;

    request[ "params" ][ "scriptParams" ][ "command" ] = "setoption";
    request[ "params" ][ "scriptParams" ][ "option" ] = "broadcast";

    auto answer = nma().sendtoHubAndGetAnswer( request );
    auto error = answer[ "error" ];
    ASSERT_TRUE( error.empty() ) << error.dump( 4 );

    auto broadcast = checkUiBroadcast( "", __LINE__ );

    checkExtensionsPluginRunProgressBroadcast( "started", __LINE__ );
    checkExtensionsPluginRunProgressBroadcast( "finished", __LINE__ );
}

TEST_F( SocketNetworkLuadTests, setOptionIpAddMembership ) {
    auto request = replaceTokens( hubExtensionsPluginRun, { { "__SCRIPT__", socketScript } } );

    request[ "params" ][ "scriptParams" ] = JSON_EMPTY_OBJ;

    request[ "params" ][ "scriptParams" ][ "command" ] = "setoption";
    request[ "params" ][ "scriptParams" ][ "option" ] = "ip-add-membership";

    auto answer = nma().sendtoHubAndGetAnswer( request );
    auto error = answer[ "error" ];
    ASSERT_TRUE( error.empty() ) << error.dump( 4 );

    auto broadcast = checkUiBroadcast( "", __LINE__ );

    checkExtensionsPluginRunProgressBroadcast( "started", __LINE__ );
    checkExtensionsPluginRunProgressBroadcast( "finished", __LINE__ );
}

}  // namespace Tests
}  // namespace HubPlatform
