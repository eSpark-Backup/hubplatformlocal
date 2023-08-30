
#include "nma_api_tests/application.h"

#include "test_utils/component_tests/ws_server/test_server_peer.h"
#include "common_utils/firmware_info_helper.h"

#include "test_utils/component_tests/common_nma_test_fixture.h"

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#define STRING( S ) STR( S )
#define STR( S ) #S

namespace HubPlatform {
namespace Tests {

class StatusRequestsTests : public CommonNmaTests {};

TEST_F( StatusRequestsTests, getInfoHubRequest ) {
    const auto answer = nma().sendtoHubAndGetAnswer( { { "method", "hub.info.get" }, { "params", JSON_EMPTY_OBJ } } );
    const auto resultField = answer[ "result" ];

    const auto firmwareVersion = json_get_str( resultField, "firmware" );
    EXPECT_EQ( firmwareVersion, FirmwareInfoHelper::getFirmwareVersionString() );

    const auto hubType = json_get_str( resultField, "model" );
    EXPECT_EQ( hubType, "local" );

    const auto hubSerial = json_get_str( resultField, "serial" );
    EXPECT_EQ( hubSerial, std::string{ __HUB_SERIAL_STR__ } );
}

TEST_F( StatusRequestsTests, checkMethodNameInAnswer ) {
    const std::string methodName = "hub.info.get";
    const auto answer = nma().sendtoHubAndGetAnswer( { { "method", methodName }, { "params", JSON_EMPTY_OBJ } } );

    const std::string answerMethodName = json_get_str( answer, "method" );
    EXPECT_EQ( answerMethodName, methodName );
}
}  // namespace Tests
}  // namespace HubPlatform
