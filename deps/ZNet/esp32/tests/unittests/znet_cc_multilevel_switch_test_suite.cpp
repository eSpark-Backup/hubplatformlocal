#include <znet/znet_node.hpp>
#include <znet/cc_0x26_switch_multi_level.hpp>
#include <znet/znet_utils.hpp>

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "znet_node_requests_managing_fixture.h"
#include "fake_file.h"
#include "znet_fc_send_data_mock.h"
#include "MainFixture.h"

extern "C" {
#include <znet_lib_cc.h>
}

namespace znet {

namespace ut {

using ::testing::_;
using ::testing::ElementsAre;
using ::testing::Invoke;
using ::testing::MockFunction;

class ZCcMultilevelSwitchTestSuite : public MainFixture, public NodeRequestsManagingFixture {
public:
};

TEST_F( ZCcMultilevelSwitchTestSuite, interview_v2 ) {
    MockFunction<void( RequestFailTypeOpt )> callback;

    auto cc = ensureNodeAndCc<ZCc0x26SwitchMultiLevel>( 1, 0, 2 );

    EXPECT_CALL( callback, Call( _ ) ).Times( 0 );

    EXPECT_FALSE( cc->interview( callback.AsStdFunction(), {} ) );
    EXPECT_CALL( callback, Call( RequestFailTypeOpt{} ) ).Times( 0 );

    EXPECT_FALSE( cc->hasInterviewDone() );

    EXPECT_EQ( cc->getPrimaryType(), 0xFE );
    EXPECT_EQ( cc->getSecondaryType(), 0xFE );
}

TEST_F( ZCcMultilevelSwitchTestSuite, interview_v3 ) {
    MockFunction<void( RequestFailTypeOpt )> callback;

    auto cc = ensureNodeAndCc<ZCc0x26SwitchMultiLevel>( 1, 0, 3 );

    EXPECT_CALL( callback, Call( _ ) ).Times( 0 );

    EXPECT_TRUE( cc->interview( callback.AsStdFunction(), {} ) );

    EXPECT_FALSE( cc->hasInterviewDone() );

    EXPECT_CALL( callback, Call( RequestFailTypeOpt{} ) ).Times( 1 );

    znet_multilevel_switch_supported_report_t report{};
    report.primary_type = 10;
    report.secondary_type = 30;
    getRequestsManager().onRespond( SwitchMultilevelSupportedTypesRequest( 1, 0 ), report );

    EXPECT_TRUE( cc->hasInterviewDone() );

    EXPECT_EQ( cc->getPrimaryType(), 10 );
    EXPECT_EQ( cc->getSecondaryType(), 30 );
}

TEST_F( ZCcMultilevelSwitchTestSuite, startLevelChange_v1 ) {
    auto cc = ensureNodeAndCc<ZCc0x26SwitchMultiLevel>( 1, 0, 1 );

    FcSendDataMock sendDataMock;

    {
        const std::vector<uint8_t> expectedData = {
            COMMAND_CLASS_SWITCH_MULTILEVEL, SWITCH_MULTILEVEL_START_LEVEL_CHANGE, 0x60, 0xFE
        };

        EXPECT_CALL( sendDataMock, fcSendData( 1, 0, expectedData, false, false, Encapsulation_None ) );
        EXPECT_TRUE( cc->startLevelChange( true ) );
    }

    {
        const std::vector<uint8_t> expectedData = {
            COMMAND_CLASS_SWITCH_MULTILEVEL, SWITCH_MULTILEVEL_START_LEVEL_CHANGE, 0x40, 22
        };

        EXPECT_CALL( sendDataMock, fcSendData( 1, 0, expectedData, false, false, Encapsulation_None ) );
        EXPECT_TRUE( cc->startLevelChange( true, 22 ) );
    }

    {
        const std::vector<uint8_t> expectedData = {
            COMMAND_CLASS_SWITCH_MULTILEVEL, SWITCH_MULTILEVEL_START_LEVEL_CHANGE, 0x00, 22
        };

        EXPECT_CALL( sendDataMock, fcSendData( 1, 0, expectedData, false, false, Encapsulation_None ) );
        EXPECT_TRUE( cc->startLevelChange( false, 22, Seconds( 100 ) ) );
    }
}

TEST_F( ZCcMultilevelSwitchTestSuite, startLevelChange_v2 ) {
    auto cc = ensureNodeAndCc<ZCc0x26SwitchMultiLevel>( 1, 0, 2 );

    FcSendDataMock sendDataMock;

    {
        const std::vector<uint8_t> expectedData = {
            COMMAND_CLASS_SWITCH_MULTILEVEL, SWITCH_MULTILEVEL_START_LEVEL_CHANGE, 0x60, 0xFE, 0xFF
        };

        EXPECT_CALL( sendDataMock, fcSendData( 1, 0, expectedData, false, false, Encapsulation_None ) );
        EXPECT_TRUE( cc->startLevelChange( true ) );
    }

    {
        const std::vector<uint8_t> expectedData = {
            COMMAND_CLASS_SWITCH_MULTILEVEL, SWITCH_MULTILEVEL_START_LEVEL_CHANGE, 0x40, 22, 40
        };

        EXPECT_CALL( sendDataMock, fcSendData( 1, 0, expectedData, false, false, Encapsulation_None ) );
        EXPECT_TRUE( cc->startLevelChange( true, 22, Seconds( 40 ) ) );
    }

    {
        const std::vector<uint8_t> expectedData = {
            COMMAND_CLASS_SWITCH_MULTILEVEL, SWITCH_MULTILEVEL_START_LEVEL_CHANGE, 0x00, 22, 127 + 10
        };

        EXPECT_CALL( sendDataMock, fcSendData( 1, 0, expectedData, false, false, Encapsulation_None ) );
        EXPECT_TRUE( cc->startLevelChange( false, 22, Seconds( 600 ) ) );
    }
}

TEST_F( ZCcMultilevelSwitchTestSuite, startLevelChange_v3 ) {
    auto cc = ensureNodeAndCc<ZCc0x26SwitchMultiLevel>( 1, 0, 3 );

    FcSendDataMock sendDataMock;

    {
        const std::vector<uint8_t> expectedData = {
            COMMAND_CLASS_SWITCH_MULTILEVEL, SWITCH_MULTILEVEL_START_LEVEL_CHANGE, 0x78, 0xFE, 0xFF, 0
        };

        EXPECT_CALL( sendDataMock, fcSendData( 1, 0, expectedData, false, false, Encapsulation_None ) );
        EXPECT_TRUE( cc->startLevelChange( true ) );
    }

    {
        const std::vector<uint8_t> expectedData = {
            COMMAND_CLASS_SWITCH_MULTILEVEL, SWITCH_MULTILEVEL_START_LEVEL_CHANGE, 0x58, 22, 40, 0
        };

        EXPECT_CALL( sendDataMock, fcSendData( 1, 0, expectedData, false, false, Encapsulation_None ) );
        EXPECT_TRUE( cc->startLevelChange( true, 22, Seconds( 40 ) ) );
    }

    {
        const std::vector<uint8_t> expectedData = {
            COMMAND_CLASS_SWITCH_MULTILEVEL, SWITCH_MULTILEVEL_START_LEVEL_CHANGE, 0x18, 22, 127 + 10, 0
        };

        EXPECT_CALL( sendDataMock, fcSendData( 1, 0, expectedData, false, false, Encapsulation_None ) );
        EXPECT_TRUE( cc->startLevelChange( false, 22, Seconds( 600 ) ) );
    }
}

TEST_F( ZCcMultilevelSwitchTestSuite, stopLevelChange ) {
    auto cc = ensureNodeAndCc<ZCc0x26SwitchMultiLevel>( 1, 0, 3 );

    FcSendDataMock sendDataMock;

    {
        const std::vector<uint8_t> expectedData = { COMMAND_CLASS_SWITCH_MULTILEVEL,
                                                    SWITCH_MULTILEVEL_STOP_LEVEL_CHANGE };

        EXPECT_CALL( sendDataMock, fcSendData( 1, 0, expectedData, false, false, Encapsulation_None ) );
        EXPECT_TRUE( cc->stopLevelChange() );
    }
}

}  // namespace ut
}  // namespace znet
