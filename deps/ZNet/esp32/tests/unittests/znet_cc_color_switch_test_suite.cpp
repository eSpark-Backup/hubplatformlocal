#include <znet/znet_node.hpp>
#include <znet/cc_0x33_color_switch.hpp>
#include <znet/znet_utils.hpp>

#include <gtest/gtest.h>
#include <gmock/gmock.h>

extern "C" {
#include <znet_lib_cc.h>
}

#include "znet_node_requests_managing_fixture.h"
#include "fake_file.h"
#include "znet_fc_send_data_mock.h"
#include "MainFixture.h"

namespace znet {

namespace ut {

using ::testing::_;
using ::testing::ElementsAre;
using ::testing::Invoke;
using ::testing::MockFunction;

class ZCcColorSwitchTestSuite : public MainFixture, public NodeRequestsManagingFixture {
public:
};

TEST_F( ZCcColorSwitchTestSuite, interview ) {
    auto cc = ensureNodeAndCc<ZCc0x33ColorSwitch>( 1, 0, 1 );

    MockFunction<void( RequestFailTypeOpt )> callback;

    EXPECT_CALL( callback, Call( _ ) ).Times( 0 );

    EXPECT_TRUE( cc->interview( callback.AsStdFunction(), {} ) );

    EXPECT_FALSE( cc->hasInterviewDone() );

    EXPECT_CALL( callback, Call( RequestFailTypeOpt{} ) ).Times( 1 );

    uint8_t bitmask[2] = {
        0,
    };
    utils::setBit( bitmask, static_cast<uint8_t>( ColorComponentID::Blue ) );
    utils::setBit( bitmask, static_cast<uint8_t>( ColorComponentID::Purple ) );
    znet_color_switch_supported_report_t report{};
    report.component_mask = bitmask;
    report.component_mask_count = 1;

    getRequestsManager().onRespond( ColorSwitchSupportedTypesRequest( 1, 0 ), report );

    EXPECT_TRUE( cc->hasInterviewDone() );

    EXPECT_THAT( cc->getSupportedColors(), ElementsAre( ColorComponentID::Blue, ColorComponentID::Purple ) );
}

TEST_F( ZCcColorSwitchTestSuite, requestValue ) {
    getNodesProvider().nodeCreate( 1 );
    auto node = getNodesProvider().onFindNode( 1 );

    node->addCommandClass( 0, ZNET_COMMAND_CLASS_SWITCH_COLOR, 1 );
    auto cc = node->getCommandClass<ZCc0x33ColorSwitch>( 0 );

    cc->interview( {}, {} );

    uint8_t bitmask[2] = {
        0,
    };
    utils::setBit( bitmask, static_cast<uint8_t>( ColorComponentID::Blue ) );
    utils::setBit( bitmask, static_cast<uint8_t>( ColorComponentID::Purple ) );
    znet_color_switch_supported_report_t report{};
    report.component_mask = bitmask;
    report.component_mask_count = 1;

    node->getRequestsManager().onRespond( ColorSwitchSupportedTypesRequest( 1, 0 ), report );

    MockFunction<void( RequestFailTypeOpt )> callback;
    EXPECT_CALL( callback, Call( _ ) ).Times( 0 );

    EXPECT_TRUE( cc->requestValue( createRequestValue( callback ) ) );

    znet_color_switch_report_t colorReport1 = {};
    colorReport1.value = 10;
    colorReport1.component_id = static_cast<uint8_t>( ColorComponentID::Blue );
    node->getRequestsManager().onRespond( ColorSwitchValueRequest( 1, 0, colorReport1.component_id ), colorReport1 );

    EXPECT_CALL( callback, Call( RequestFailTypeOpt() ) ).Times( 1 );

    znet_color_switch_report_t colorReport2 = {};
    colorReport2.value = 11;
    colorReport2.component_id = static_cast<uint8_t>( ColorComponentID::Purple );
    node->getRequestsManager().onRespond( ColorSwitchValueRequest( 1, 0, colorReport2.component_id ), colorReport2 );
}

TEST_F( ZCcColorSwitchTestSuite, startLevelChange_v2 ) {
    auto cc = ensureNodeAndCc<ZCc0x33ColorSwitch>( 1, 0, 2 );

    // interview
    {
        cc->interview( {}, {} );
        uint8_t bitmask[2] = {
            0,
        };
        utils::setBit( bitmask, static_cast<uint8_t>( ColorComponentID::Blue ) );
        utils::setBit( bitmask, static_cast<uint8_t>( ColorComponentID::Purple ) );
        znet_color_switch_supported_report_t report{};
        report.component_mask = bitmask;
        report.component_mask_count = 1;

        getRequestsManager().onRespond( ColorSwitchSupportedTypesRequest( 1, 0 ), report );
    }

    FcSendDataMock sendDataMock;

    {
        const std::vector<uint8_t> expectedData = { COMMAND_CLASS_SWITCH_COLOR,
                                                    SWITCH_COLOR_START_LEVEL_CHANGE,
                                                    0x60,
                                                    static_cast<uint8_t>( ColorComponentID::Blue ),
                                                    0 };

        EXPECT_CALL( sendDataMock, fcSendData( 1, 0, expectedData, false, false, Encapsulation_None ) );
        EXPECT_TRUE( cc->startLevelChange( ColorComponentID::Blue, true ) );
    }

    {
        const std::vector<uint8_t> expectedData = { COMMAND_CLASS_SWITCH_COLOR,
                                                    SWITCH_COLOR_START_LEVEL_CHANGE,
                                                    0x40,
                                                    static_cast<uint8_t>( ColorComponentID::Purple ),
                                                    10 };

        EXPECT_CALL( sendDataMock, fcSendData( 1, 0, expectedData, false, false, Encapsulation_None ) );
        EXPECT_TRUE( cc->startLevelChange( ColorComponentID::Purple, true, 10 ) );
    }

    {
        const std::vector<uint8_t> expectedData = { COMMAND_CLASS_SWITCH_COLOR,
                                                    SWITCH_COLOR_START_LEVEL_CHANGE,
                                                    0x00,
                                                    static_cast<uint8_t>( ColorComponentID::Purple ),
                                                    20 };

        EXPECT_CALL( sendDataMock, fcSendData( 1, 0, expectedData, false, false, Encapsulation_None ) );
        EXPECT_TRUE( cc->startLevelChange( ColorComponentID::Purple, false, 20, Seconds( 100 ) ) );
    }

    {
        EXPECT_CALL( sendDataMock, fcSendData( _, _, _, _, _, _ ) ).Times( 0 );
        EXPECT_FALSE( cc->startLevelChange( ColorComponentID::Amber, true ) );
    }
}

TEST_F( ZCcColorSwitchTestSuite, startLevelChange_v3 ) {
    auto cc = ensureNodeAndCc<ZCc0x33ColorSwitch>( 1, 0, 3 );

    // interview
    {
        cc->interview( {}, {} );
        uint8_t bitmask[2] = {
            0,
        };
        utils::setBit( bitmask, static_cast<uint8_t>( ColorComponentID::Blue ) );
        utils::setBit( bitmask, static_cast<uint8_t>( ColorComponentID::Purple ) );
        znet_color_switch_supported_report_t report{};
        report.component_mask = bitmask;
        report.component_mask_count = 1;

        getRequestsManager().onRespond( ColorSwitchSupportedTypesRequest( 1, 0 ), report );
    }

    FcSendDataMock sendDataMock;

    {
        const std::vector<uint8_t> expectedData = { COMMAND_CLASS_SWITCH_COLOR,
                                                    SWITCH_COLOR_START_LEVEL_CHANGE,
                                                    0x60,
                                                    static_cast<uint8_t>( ColorComponentID::Blue ),
                                                    0,
                                                    0xFF };

        EXPECT_CALL( sendDataMock, fcSendData( 1, 0, expectedData, false, false, Encapsulation_None ) );
        EXPECT_TRUE( cc->startLevelChange( ColorComponentID::Blue, true ) );
    }

    {
        const std::vector<uint8_t> expectedData = { COMMAND_CLASS_SWITCH_COLOR,
                                                    SWITCH_COLOR_START_LEVEL_CHANGE,
                                                    0x40,
                                                    static_cast<uint8_t>( ColorComponentID::Purple ),
                                                    10,
                                                    33 };

        EXPECT_CALL( sendDataMock, fcSendData( 1, 0, expectedData, false, false, Encapsulation_None ) );
        EXPECT_TRUE( cc->startLevelChange( ColorComponentID::Purple, true, 10, Seconds( 33 ) ) );
    }

    {
        const std::vector<uint8_t> expectedData = { COMMAND_CLASS_SWITCH_COLOR,
                                                    SWITCH_COLOR_START_LEVEL_CHANGE,
                                                    0x00,
                                                    static_cast<uint8_t>( ColorComponentID::Purple ),
                                                    20,
                                                    127 + 10 };

        EXPECT_CALL( sendDataMock, fcSendData( 1, 0, expectedData, false, false, Encapsulation_None ) );
        EXPECT_TRUE( cc->startLevelChange( ColorComponentID::Purple, false, 20, Seconds( 600 ) ) );
    }

    {
        EXPECT_CALL( sendDataMock, fcSendData( _, _, _, _, _, _ ) ).Times( 0 );
        EXPECT_FALSE( cc->startLevelChange( ColorComponentID::Amber, false ) );
    }
}

TEST_F( ZCcColorSwitchTestSuite, stopLevelChange ) {
    auto cc = ensureNodeAndCc<ZCc0x33ColorSwitch>( 1, 0, 3 );

    // interview
    {
        cc->interview( {}, {} );
        uint8_t bitmask[2] = {
            0,
        };
        utils::setBit( bitmask, static_cast<uint8_t>( ColorComponentID::Blue ) );
        utils::setBit( bitmask, static_cast<uint8_t>( ColorComponentID::Purple ) );
        znet_color_switch_supported_report_t report{};
        report.component_mask = bitmask;
        report.component_mask_count = 1;

        getRequestsManager().onRespond( ColorSwitchSupportedTypesRequest( 1, 0 ), report );
    }

    FcSendDataMock sendDataMock;

    {
        const std::vector<uint8_t> expectedData = {
            COMMAND_CLASS_SWITCH_COLOR, SWITCH_COLOR_STOP_LEVEL_CHANGE, static_cast<uint8_t>( ColorComponentID::Blue )
        };

        EXPECT_CALL( sendDataMock, fcSendData( 1, 0, expectedData, false, false, Encapsulation_None ) );
        EXPECT_TRUE( cc->stopLevelChange( ColorComponentID::Blue ) );
    }

    {
        EXPECT_CALL( sendDataMock, fcSendData( _, _, _, _, _, _ ) ).Times( 0 );
        EXPECT_FALSE( cc->stopLevelChange( ColorComponentID::Cwhite ) );
    }
}

}  // namespace ut
}  // namespace znet
