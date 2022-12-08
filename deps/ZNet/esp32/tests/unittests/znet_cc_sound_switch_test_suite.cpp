#include <znet/znet_node.hpp>
#include <znet/cc_0x79_sound_switch.hpp>

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "znet_node_requests_managing_fixture.h"
#include "fake_file.h"

#include "MainFixture.h"
#include "znet_fc_send_data_mock.h"

namespace znet {

static bool operator==( const ZCc0x79SoundSwitch::ToneInfo& a, const ZCc0x79SoundSwitch::ToneInfo& b ) {
    return a.name == b.name && a.duration == b.duration;
}

namespace ut {

using ::testing::_;
using ::testing::ElementsAre;
using ::testing::Invoke;
using ::testing::MockFunction;

class ZCcSoundSwitchTestSuite : public MainFixture, public NodeRequestsManagingFixture, public FcSendDataMock {
public:
    static znet_cmd_sound_switch_tone_info_t getToneReport1() {
        znet_cmd_sound_switch_tone_info_t toneInfoReport1{};
        toneInfoReport1.name = reinterpret_cast<const uint8_t*>( "name1" );
        toneInfoReport1.name_length = 5;
        toneInfoReport1.tone_duration = 50;
        toneInfoReport1.tone_id = 1;

        return toneInfoReport1;
    }

    static znet_cmd_sound_switch_tone_info_t getToneReport2() {
        znet_cmd_sound_switch_tone_info_t toneInfoReport2{};
        toneInfoReport2.name = reinterpret_cast<const uint8_t*>( "name2" );
        toneInfoReport2.name_length = 5;
        toneInfoReport2.tone_duration = 60;
        toneInfoReport2.tone_id = 2;

        return toneInfoReport2;
    }
};

TEST_F( ZCcSoundSwitchTestSuite, interview ) {
    getNodesProvider().nodeCreate( 1 );
    auto node = getNodesProvider().onFindNode( 1 );

    MockFunction<void( RequestFailTypeOpt )> callback;

    node->addCommandClass( 0, ZNET_COMMAND_CLASS_SOUND_SWITCH, 1 );
    auto cc = node->getCommandClass<ZCc0x79SoundSwitch>( 0 );

    EXPECT_CALL( callback, Call( _ ) ).Times( 0 );

    EXPECT_TRUE( cc->interview( callback.AsStdFunction(), {} ) );

    node->getRequestsManager().onRespond( SoundSwitchTonesNumberRequest( 1, 0 ), 2 );

    node->getRequestsManager().onRespond( SoundSwitchToneInfoRequest( 1, 0, 1 ), getToneReport1() );

    EXPECT_FALSE( cc->hasInterviewDone() );
    EXPECT_CALL( callback, Call( RequestFailTypeOpt{} ) ).Times( 1 );

    node->getRequestsManager().onRespond( SoundSwitchToneInfoRequest( 1, 0, 2 ), getToneReport2() );

    EXPECT_TRUE( cc->hasInterviewDone() );

    const ZCc0x79SoundSwitch::ToneInfo toneInfo1{ 50, "name1" };
    const ZCc0x79SoundSwitch::ToneInfo toneInfo2{ 60, "name2" };

    EXPECT_THAT( cc->getToneInfos(), ElementsAre( toneInfo1, toneInfo2 ) );
}

TEST_F( ZCcSoundSwitchTestSuite, interview_fail ) {
    getNodesProvider().nodeCreate( 1 );
    auto node = getNodesProvider().onFindNode( 1 );

    node->addCommandClass( 0, ZNET_COMMAND_CLASS_SOUND_SWITCH, 1 );
    auto cc = node->getCommandClass<ZCc0x79SoundSwitch>( 0 );

    // report fail
    {
        MockFunction<void( RequestFailTypeOpt )> callback;
        EXPECT_CALL( callback, Call( _ ) ).Times( 0 );

        EXPECT_TRUE( cc->interview( callback.AsStdFunction(), {} ) );

        node->getRequestsManager().onRespond( SoundSwitchTonesNumberRequest( 1, 0 ), 2 );
        node->getRequestsManager().onRespond( SoundSwitchToneInfoRequest( 1, 0, 1 ), getToneReport1() );

        EXPECT_CALL( callback, Call( RequestFailTypeOpt{ RequestFailType::ReportedFail } ) ).Times( 1 );

        node->getRequestsManager().onFail( SoundSwitchToneInfoRequest( 1, 0, 2 ) );
        EXPECT_FALSE( cc->hasInterviewDone() );
    }

    // sending fail
    {
        MockFunction<void( RequestFailTypeOpt )> callback;
        EXPECT_CALL( callback, Call( _ ) ).Times( 0 );

        getRequestsManager().addFailedRequest( SoundSwitchToneInfoRequest( 1, 0, 2 ) );

        EXPECT_TRUE( cc->interview( callback.AsStdFunction(), {} ) );
        node->getRequestsManager().onRespond( SoundSwitchTonesNumberRequest( 1, 0 ), 2 );

        EXPECT_CALL( callback, Call( RequestFailTypeOpt{ RequestFailType::SendingError } ) ).Times( 1 );

        node->getRequestsManager().onRespond( SoundSwitchToneInfoRequest( 1, 0, 1 ), getToneReport1() );
        getTimerProvider().addTime( DefaultPostponedRequestDelay );

        EXPECT_FALSE( cc->hasInterviewDone() );
    }

    // first sending fail
    {
        MockFunction<void( RequestFailTypeOpt )> callback;
        EXPECT_CALL( callback, Call( _ ) ).Times( 0 );

        getRequestsManager().addFailedRequest( SoundSwitchTonesNumberRequest( 1, 0 ) );

        EXPECT_FALSE( cc->interview( callback.AsStdFunction(), {} ) );
        EXPECT_FALSE( cc->hasInterviewDone() );
    }
}

TEST_F( ZCcSoundSwitchTestSuite, requestValue ) {
    getNodesProvider().nodeCreate( 1 );
    auto node = getNodesProvider().onFindNode( 1 );

    node->addCommandClass( 0, ZNET_COMMAND_CLASS_SOUND_SWITCH, 1 );
    auto cc = node->getCommandClass<ZCc0x79SoundSwitch>( 0 );

    cc->interview( {}, {} );

    node->getRequestsManager().onRespond( SoundSwitchTonesNumberRequest( 1, 0 ), 2 );
    node->getRequestsManager().onRespond( SoundSwitchToneInfoRequest( 1, 0, 1 ), getToneReport1() );
    node->getRequestsManager().onRespond( SoundSwitchToneInfoRequest( 1, 0, 2 ), getToneReport2() );

    MockFunction<void( RequestFailTypeOpt )> callback;
    EXPECT_CALL( callback, Call( _ ) ).Times( 0 );

    EXPECT_TRUE( cc->requestValue( createRequestValue( callback ) ) );

    node->getRequestsManager().onRespond( SoundSwitchConfigurationRequest( 1, 0 ),
                                          znet_cmd_sound_switch_configuration_t{ 10, 1 } );

    EXPECT_CALL( callback, Call( RequestFailTypeOpt{} ) ).Times( 1 );
    node->getRequestsManager().onRespond( SoundSwitchTonePlayRequest( 1, 0 ),
                                          znet_cmd_sound_switch_tone_play_t{ 2, 20 } );
}

TEST_F( ZCcSoundSwitchTestSuite, requestValue_fail ) {
    getNodesProvider().nodeCreate( 1 );
    auto node = getNodesProvider().onFindNode( 1 );

    node->addCommandClass( 0, ZNET_COMMAND_CLASS_SOUND_SWITCH, 1 );
    auto cc = node->getCommandClass<ZCc0x79SoundSwitch>( 0 );

    // empty cc
    {
        MockFunction<void( RequestFailTypeOpt )> callback;
        EXPECT_CALL( callback, Call( _ ) ).Times( 0 );

        EXPECT_FALSE( cc->requestValue( createRequestValue( callback ) ) );
    }

    cc->interview( {}, {} );

    node->getRequestsManager().onRespond( SoundSwitchTonesNumberRequest( 1, 0 ), 2 );
    node->getRequestsManager().onRespond( SoundSwitchToneInfoRequest( 1, 0, 1 ), getToneReport1() );
    node->getRequestsManager().onRespond( SoundSwitchToneInfoRequest( 1, 0, 2 ), getToneReport2() );

    // report fail
    {
        MockFunction<void( RequestFailTypeOpt )> callback;
        EXPECT_CALL( callback, Call( _ ) ).Times( 0 );

        EXPECT_TRUE( cc->requestValue( createRequestValue( callback ) ) );

        node->getRequestsManager().onRespond( SoundSwitchConfigurationRequest( 1, 0 ),
                                              znet_cmd_sound_switch_configuration_t{ 10, 1 } );

        EXPECT_CALL( callback, Call( RequestFailTypeOpt{ RequestFailType::ReportedFail } ) ).Times( 1 );
        node->getRequestsManager().onFail( SoundSwitchTonePlayRequest( 1, 0 ) );
    }

    // sending fail
    {
        MockFunction<void( RequestFailTypeOpt )> callback;
        EXPECT_CALL( callback, Call( _ ) ).Times( 0 );

        getRequestsManager().addFailedRequest( SoundSwitchTonePlayRequest( 1, 0 ) );

        EXPECT_TRUE( cc->requestValue( createRequestValue( callback ) ) );

        EXPECT_CALL( callback, Call( RequestFailTypeOpt{ RequestFailType::SendingError } ) ).Times( 1 );
        node->getRequestsManager().onRespond( SoundSwitchConfigurationRequest( 1, 0 ),
                                              znet_cmd_sound_switch_configuration_t{ 10, 1 } );
    }

    // first sending fail
    {
        MockFunction<void( RequestFailTypeOpt )> callback;
        EXPECT_CALL( callback, Call( _ ) ).Times( 0 );

        getRequestsManager().addFailedRequest( SoundSwitchConfigurationRequest( 1, 0 ) );

        EXPECT_FALSE( cc->requestValue( createRequestValue( callback ) ) );
    }
}

TEST_F( ZCcSoundSwitchTestSuite, setTonePlay ) {
    // v1
    {
        auto cc = ensureNodeAndCc<ZCc0x79SoundSwitch>( 3, 0, 1 );

        EXPECT_CALL(
            *this,
            fcSendData( 3,
                        0,
                        std::vector<uint8_t>{ ZNET_COMMAND_CLASS_SOUND_SWITCH, SOUND_SWITCH_TONE_PLAY_SET, 0x9B, 0xFF },
                        false,
                        false,
                        Encapsulation_None ) );

        cc->setTonePlay( 0x9B, 0x8A );

        eraseCommandClass( cc );
    }

    // v1 turn off
    {
        auto cc = ensureNodeAndCc<ZCc0x79SoundSwitch>( 3, 0, 1 );

        EXPECT_CALL(
            *this,
            fcSendData( 3,
                        0,
                        std::vector<uint8_t>{ ZNET_COMMAND_CLASS_SOUND_SWITCH, SOUND_SWITCH_TONE_PLAY_SET, 0x00, 0x00 },
                        false,
                        false,
                        Encapsulation_None ) );

        cc->setTonePlay( 0x00, 0x8A );

        eraseCommandClass( cc );
    }

    // v2
    {
        auto cc = ensureNodeAndCc<ZCc0x79SoundSwitch>( 3, 0, 2 );

        EXPECT_CALL(
            *this,
            fcSendData( 3,
                        0,
                        std::vector<uint8_t>{ ZNET_COMMAND_CLASS_SOUND_SWITCH, SOUND_SWITCH_TONE_PLAY_SET, 0x61, 0x8A },
                        false,
                        false,
                        Encapsulation_None ) );

        cc->setTonePlay( 0x61, 0x8A );

        eraseCommandClass( cc );
    }
}

}  // namespace ut
}  // namespace znet
