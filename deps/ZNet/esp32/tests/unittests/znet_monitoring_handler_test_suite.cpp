#include "fake_owner_fixture.h"
#include "MainFixture.h"
#include <znet_handler_monitoring.hpp>
#include <znet/cc_0x84_wakeup.hpp>
#include <znet/cc_0x63_user_code.hpp>
#include <znet/cc_0x32_meter.hpp>
#include <znet/cc_0x5d_anti_theft.hpp>
#include <znet/cc_0x7e_anti_theft_unlock.hpp>

#include <znet/enums/thermostat_setback.hpp>
#include <gtest/gtest.h>
#include <gmock/gmock.h>

namespace znet {

namespace UserCode {

static bool operator==( const UserCodeRange& a, const UserCodeRange& b ) {
    return a.id == b.id && a.code == b.code && a.lastId == b.lastId && a.status == b.status;
}

static void PrintTo( const UserCodeRanges& ranges, std::ostream* os ) {
    for( const auto& range : ranges ) {
        *os << "[" << range.id << "; " << range.lastId << "; " << static_cast<uint8_t>( range.status ) << "; "
            << range.code << "]";
    }
}

}  // namespace UserCode

namespace ut {

using ::testing::_;
using ::testing::ElementsAre;

class ZNetMonitoringHandlerTestSuite : public MainFixture, public FakeOwnerFixture {
protected:
    void SetUp() override {
        MainFixture::SetUp();

        propagateCallback<ZNetMonitoringHandlerTestSuite,
                          decltype( &znet_callbacks_t::node_cmd_extended_user_code_result ),
                          &znet_callbacks_t::node_cmd_extended_user_code_result>();

        propagateCallback<ZNetMonitoringHandlerTestSuite,
                          decltype( &znet_callbacks_t::node_cmd_user_code_report ),
                          &znet_callbacks_t::node_cmd_user_code_report>();

        propagateCallback<ZNetMonitoringHandlerTestSuite,
                          decltype( &znet_callbacks_t::node_cmd_meter_result ),
                          &znet_callbacks_t::node_cmd_meter_result>();

        propagateCallback<ZNetMonitoringHandlerTestSuite,
                          decltype( &znet_callbacks_t::node_cmd_anti_theft_result ),
                          &znet_callbacks_t::node_cmd_anti_theft_result>();

        propagateCallback<ZNetMonitoringHandlerTestSuite,
                          decltype( &znet_callbacks_t::node_cmd_anti_theft_unlock_state_result ),
                          &znet_callbacks_t::node_cmd_anti_theft_unlock_state_result>();

        propagateCallback<ZNetMonitoringHandlerTestSuite,
                          decltype( &znet_callbacks_t::node_cmd_binary_switch_result ),
                          &znet_callbacks_t::node_cmd_binary_switch_result>();
    }
};

TEST_F( ZNetMonitoringHandlerTestSuite, onWakeUpResult ) {
    getOwner().nodeCreate( 2 );
    auto node = getOwner().onFindNode( 2 );

    node->addCommandClass( 0, ZNET_COMMAND_CLASS_WAKE_UP );
    auto wakeupCc = node->getCommandClass<ZCc0x84WakeUp>( 0 );

    void* callbackArg = &getImpl();

    getImpl().activateMode( IZNet::Mode::Monitoring );
    const auto& callbacks = getImpl().getCallbacks();

    // non-zero interval reported
    {
        EXPECT_CALL( getOwner(), onWakeupIntervalUpdated( 2, Seconds( 400 ), _ ) );
        EXPECT_CALL( getOwner(), onUpdateReachable( 2, true ) );

        callbacks.node_cmd_wakeup_result( ZNET_ERR_OK, 2, 0, 400, znet_get_self_node_id(), callbackArg );
        EXPECT_EQ( wakeupCc->getInterval(), 400 );
    }

    // the same interval reported, reset reachable
    {
        node->setReachable( ReachableState::Unreachable );

        EXPECT_CALL( getOwner(), onWakeupIntervalUpdated( _, _, _ ) ).Times( 1 );
        EXPECT_CALL( getOwner(), onUpdateReachable( 2, true ) );

        callbacks.node_cmd_wakeup_result( ZNET_ERR_OK, 2, 0, 400, znet_get_self_node_id(), callbackArg );
        EXPECT_EQ( wakeupCc->getInterval(), 400 );
    }

    // interval for another target node reported, reset reachable
    {
        node->setReachable( ReachableState::Unreachable );

        EXPECT_CALL( getOwner(), onWakeupIntervalUpdated( 2, Seconds( 0 ), _ ) );
        EXPECT_CALL( getOwner(), onUpdateReachable( 2, true ) );

        callbacks.node_cmd_wakeup_result( ZNET_ERR_OK, 2, 0, 400, znet_get_self_node_id() + 1, callbackArg );
        EXPECT_EQ( wakeupCc->getInterval(), 0 );
    }

    // send zero interval (the same) for my target node, don't reset reachable
    {
        EXPECT_CALL( getOwner(), onWakeupIntervalUpdated( _, _, _ ) ).Times( 1 );
        EXPECT_CALL( getOwner(), onUpdateReachable( _, _ ) ).Times( 0 );

        callbacks.node_cmd_wakeup_result( ZNET_ERR_OK, 2, 0, 0, znet_get_self_node_id(), callbackArg );
        EXPECT_EQ( wakeupCc->getInterval(), 0 );
    }
}

TEST_F( ZNetMonitoringHandlerTestSuite, onThermostatSetbackResult ) {
    getOwner().nodeCreate( 2 );
    auto node = getOwner().onFindNode( 2 );
    node->addCommandClass( 0, ZNET_COMMAND_CLASS_THERMOSTAT_SETBACK );

    void* callbackArg = &getImpl();

    getImpl().activateMode( IZNet::Mode::Monitoring );
    const auto& callbacks = getImpl().getCallbacks();

    {
        EXPECT_CALL( getOwner(),
                     onUpdateThermostatSetbackValue( 2,
                                                     0,
                                                     thermostat_setback::OverrideType::TemporaryOverride,
                                                     thermostat_setback::Mode::CustomValue,
                                                     thermostat_setback::SetbackValueOpt( 5.1f ) ) );
        EXPECT_CALL( getOwner(), onUpdateReachable( 2, true ) );

        callbacks.node_cmd_thermostat_setback_result( ZNET_ERR_OK, 2, 0, 1, 51, callbackArg );
    }

    {
        EXPECT_CALL( getOwner(),
                     onUpdateThermostatSetbackValue( 2,
                                                     0,
                                                     thermostat_setback::OverrideType::TemporaryOverride,
                                                     thermostat_setback::Mode::FrostProtection,
                                                     thermostat_setback::SetbackValueOpt() ) );
        EXPECT_CALL( getOwner(), onUpdateReachable( _, _ ) ).Times( 0 );

        callbacks.node_cmd_thermostat_setback_result( ZNET_ERR_OK, 2, 0, 1, 0x79, callbackArg );
    }
}

TEST_F( ZNetMonitoringHandlerTestSuite, userCodeReport ) {
    getOwner().nodeCreate( 2 );
    auto node = getOwner().onFindNode( 2 );

    node->addCommandClass( 0, ZNET_COMMAND_CLASS_USER_CODE, 1 );
    auto cc = node->getCommandClass<ZCc0x63UserCode>( 0 );

    cc->interview( {}, {} );
    node->getRequestsManager().onRespond( UserCodesNumberRequest( 2, 0 ), 200 );

    getImpl().activateMode( IZNet::Mode::Monitoring );

    {
        EXPECT_CALL( getOwner(),
                     onUserCodeUpdated( 2,
                                        0,
                                        UserCode::UserCodeRanges{ { 3, 3, UserCode::UserIDStatus::Enabled, "0123" } },
                                        200,
                                        znet::UserCode::UserPollStatus::Idle ) );

        const std::vector<uint8_t> extendedUserCodeReport = { COMMAND_CLASS_USER_CODE,
                                                              USER_CODE_REPORT,
                                                              3,  // user id
                                                              1,  // status
                                                              '0',
                                                              '1',
                                                              '2',
                                                              '3' };

        onMessageReceived( 2, 0, extendedUserCodeReport, Encapsulation_None, MESSAGE_TYPE_REPORT );
    }

    {
        EXPECT_CALL( getOwner(),
                     onUserCodeUpdated( 2,
                                        0,
                                        UserCode::UserCodeRanges{ { 0, 0, UserCode::UserIDStatus::Available, {} } },
                                        200,
                                        znet::UserCode::UserPollStatus::Idle ) );

        const std::vector<uint8_t> extendedUserCodeReport = {
            COMMAND_CLASS_USER_CODE,
            USER_CODE_REPORT,
            0,  // user id
            0,  // status
            0,
            0,
            0,
            0  // code
        };

        onMessageReceived( 2, 0, extendedUserCodeReport, Encapsulation_None, MESSAGE_TYPE_REPORT );
    }
}

TEST_F( ZNetMonitoringHandlerTestSuite, extendedUserCodeReport ) {
    getOwner().nodeCreate( 2 );
    auto node = getOwner().onFindNode( 2 );

    node->addCommandClass( 0, ZNET_COMMAND_CLASS_USER_CODE, 2 );
    auto cc = node->getCommandClass<ZCc0x63UserCode>( 0 );

    cc->interview( {}, {} );

    node->getRequestsManager().onRespond( UserCodesNumberRequest( 2, 0 ), 1000 );
    node->getRequestsManager().onRespond( UserCodeCapabilitiesRequest( 2, 0 ), znet_user_code_capabilities_report_t{} );

    getImpl().activateMode( IZNet::Mode::Monitoring );

    {
        EXPECT_CALL( getOwner(),
                     onUserCodeUpdated( 2,
                                        0,
                                        UserCode::UserCodeRanges{
                                            { 1, 257, UserCode::UserIDStatus::Available, {} },
                                            { 258, 258, UserCode::UserIDStatus::Disabled, "12345" },
                                            { 259, 259, UserCode::UserIDStatus::Messaging, "9876" },
                                            { 260, 514, UserCode::UserIDStatus::Available, {} },
                                        },
                                        1000,
                                        znet::UserCode::UserPollStatus::Idle ) );

        const std::vector<uint8_t> extendedUserCodeReport = {
            COMMAND_CLASS_USER_CODE,
            EXTENDED_USER_CODE_REPORT,
            3,  // user codes count

            0,
            1,  // user id
            0,  // status
            0,  // code length

            1,
            2,  // user id
            2,  // status
            5,  // code length
            '1',
            '2',
            '3',
            '4',
            '5',  // code

            1,
            3,  // user id
            3,  // status
            4,  // code length
            '9',
            '8',
            '7',
            '6',  // code

            0x02,
            0x03  // next user code
        };

        onMessageReceived( 2, 0, extendedUserCodeReport, Encapsulation_None, MESSAGE_TYPE_REPORT );
    }

    {
        EXPECT_CALL( getOwner(),
                     onUserCodeUpdated( 2,
                                        0,
                                        UserCode::UserCodeRanges{
                                            { 1, 1, UserCode::UserIDStatus::Available, {} },
                                            { 2, 2, UserCode::UserIDStatus::Disabled, "1234" },
                                            { 3, 1000, UserCode::UserIDStatus::Available, {} },
                                        },
                                        1000,
                                        znet::UserCode::UserPollStatus::Idle ) );

        const std::vector<uint8_t> extendedUserCodeReport = {
            COMMAND_CLASS_USER_CODE,
            EXTENDED_USER_CODE_REPORT,
            2,  // user codes count

            0,
            1,  // user id
            0,  // status
            0,  // code length

            0,
            2,  // user id
            2,  // status
            4,  // code length
            '1',
            '2',
            '3',
            '4',  // code

            0x00,
            0x00  // next user code
        };

        onMessageReceived( 2, 0, extendedUserCodeReport, Encapsulation_None, MESSAGE_TYPE_REPORT );
    }
}

TEST_F( ZNetMonitoringHandlerTestSuite, onUpdateFloatValue_correctReport ) {
    getOwner().nodeCreate( 2 );
    auto node = getOwner().onFindNode( 2 );

    node->addCommandClass( 0, ZNET_COMMAND_CLASS_METER, 2 );
    auto cc = node->getCommandClass<ZCc0x32Meter>( 0 );

    // cc->interview( {}, {} );

    getImpl().activateMode( IZNet::Mode::Monitoring );

    // value
    {
        EXPECT_CALL( getOwner(),
                     onUpdateFloatValue( 2, ZNET_COMMAND_CLASS_METER, 0, meter::MeterType::Electric, 5, 0x0107, 2 ) );

        const std::vector<uint8_t> meterReport = {
            ZNET_COMMAND_CLASS_METER,
            METER_REPORT,
            1 << 0 | 1 << 5 | 1 << 7,  // meter type | rate type | scale(2)
            2 << 0 | 1 << 3 | 2 << 5,  // size | scale(1:0) | precesion
            0x01,
            0x07  // value
        };

        onMessageReceived( 2, 0, meterReport, Encapsulation_None, MESSAGE_TYPE_REPORT );
    }

    // value, time
    {
        EXPECT_CALL( getOwner(),
                     onUpdateFloatValue( 2, ZNET_COMMAND_CLASS_METER, 0, meter::MeterType::Cooling, 6, 0x09, 3 ) );

        const std::vector<uint8_t> meterReport = {
            ZNET_COMMAND_CLASS_METER,
            METER_REPORT,
            5 << 0 | 2 << 5 | 1 << 7,  // meter type | rate type | scale(2)
            1 << 0 | 2 << 3 | 3 << 5,  // size | scale(1:0) | precesion
            0x09,                      // value
            0x00,
            0x00  // time
        };

        onMessageReceived( 2, 0, meterReport, Encapsulation_None, MESSAGE_TYPE_REPORT );
    }

    // value, time, scale2
    {
        EXPECT_CALL(
            getOwner(),
            onUpdateFloatValue( 2, ZNET_COMMAND_CLASS_METER, 0, meter::MeterType::Electric, 8 + 1, 0x03040506, 1 ) );

        const std::vector<uint8_t> meterReport = {
            ZNET_COMMAND_CLASS_METER,
            METER_REPORT,
            1 << 0 | 2 << 5 | 1 << 7,  // meter type | rate type | scale(2)
            4 << 0 | 3 << 3 | 1 << 5,  // size | scale(1:0) | precesion
            0x03,
            0x04,
            0x05,
            0x06,  // value
            0x00,
            0x00,  // time
            0x01   // scale2
        };

        onMessageReceived( 2, 0, meterReport, Encapsulation_None, MESSAGE_TYPE_REPORT );
    }

    // value, time, prev value, scale2
    {
        EXPECT_CALL(
            getOwner(),
            onUpdateFloatValue( 2, ZNET_COMMAND_CLASS_METER, 0, meter::MeterType::Electric, 8 + 0, 0x0506, 1 ) );

        const std::vector<uint8_t> meterReport = {
            ZNET_COMMAND_CLASS_METER,
            METER_REPORT,
            1 << 0 | 2 << 5 | 1 << 7,  // meter type | rate type | scale(2)
            2 << 0 | 3 << 3 | 1 << 5,  // size | scale(1:0) | precesion
            0x05,
            0x06,  // value
            0x00,
            0x01,  // time
            0x03,
            0x04,
            0x00  // scale 2
        };

        onMessageReceived( 2, 0, meterReport, Encapsulation_None, MESSAGE_TYPE_REPORT );
    }
}

TEST_F( ZNetMonitoringHandlerTestSuite, onUpdateFloatValue_incorrectReport ) {
    getOwner().nodeCreate( 2 );
    auto node = getOwner().onFindNode( 2 );

    node->addCommandClass( 0, ZNET_COMMAND_CLASS_METER, 2 );
    auto cc = node->getCommandClass<ZCc0x32Meter>( 0 );

    // cc->interview( {}, {} );

    getImpl().activateMode( IZNet::Mode::Monitoring );

    // value is incomplete
    {
        EXPECT_CALL( getOwner(),
                     onUpdateFloatValue( 2, ZNET_COMMAND_CLASS_METER, 0, meter::MeterType::Electric, 5, 0, 2 ) );

        const std::vector<uint8_t> meterReport = {
            ZNET_COMMAND_CLASS_METER,
            METER_REPORT,
            1 << 0 | 1 << 5 | 1 << 7,  // meter type | rate type | scale(2)
            2 << 0 | 1 << 3 | 2 << 5,  // size | scale(1:0) | precesion
            0x07                       // value
        };

        onMessageReceived( 2, 0, meterReport, Encapsulation_None, MESSAGE_TYPE_REPORT );
    }

    // time is incomplete
    {
        EXPECT_CALL( getOwner(),
                     onUpdateFloatValue( 2, ZNET_COMMAND_CLASS_METER, 0, meter::MeterType::Water, 5, 0x0701, 2 ) );

        const std::vector<uint8_t> meterReport = {
            ZNET_COMMAND_CLASS_METER,
            METER_REPORT,
            3 << 0 | 1 << 5 | 1 << 7,  // meter type | rate type | scale(2)
            2 << 0 | 1 << 3 | 2 << 5,  // size | scale(1:0) | precesion
            0x07,
            0x01,  // value
            0x00   // incomplete time
        };

        onMessageReceived( 2, 0, meterReport, Encapsulation_None, MESSAGE_TYPE_REPORT );
    }

    // prev value is incomplete and scale2 not found
    {
        EXPECT_CALL( getOwner(),
                     onUpdateFloatValue( 2,
                                         ZNET_COMMAND_CLASS_METER,
                                         0,
                                         meter::MeterType::Water,
                                         7,  // scale2 not found
                                         0x0701,
                                         2 ) );

        const std::vector<uint8_t> meterReport = {
            ZNET_COMMAND_CLASS_METER,
            METER_REPORT,
            3 << 0 | 1 << 5 | 1 << 7,  // meter type | rate type | scale(2)
            2 << 0 | 3 << 3 | 2 << 5,  // size | scale(1:0) | precesion
            0x07,
            0x01,  // value
            0x00,
            0x01,  // time
            0x01   // incomplete time
        };

        onMessageReceived( 2, 0, meterReport, Encapsulation_None, MESSAGE_TYPE_REPORT );
    }

    // scale2 not found
    {
        EXPECT_CALL( getOwner(),
                     onUpdateFloatValue( 2,
                                         ZNET_COMMAND_CLASS_METER,
                                         0,
                                         meter::MeterType::Water,
                                         7,  // scale2 not found
                                         0x0701,
                                         2 ) );

        const std::vector<uint8_t> meterReport = {
            ZNET_COMMAND_CLASS_METER,
            METER_REPORT,
            3 << 0 | 1 << 5 | 1 << 7,  // meter type | rate type | scale(2)
            2 << 0 | 3 << 3 | 2 << 5,  // size | scale(1:0) | precesion
            0x07,
            0x01,  // value
            0x00,
            0x01,  // time
            0x01,
            0x01  // prev value
        };

        onMessageReceived( 2, 0, meterReport, Encapsulation_None, MESSAGE_TYPE_REPORT );
    }

    // time is 0 but prev value exists
    {
        EXPECT_CALL( getOwner(),
                     onUpdateFloatValue( 2,
                                         ZNET_COMMAND_CLASS_METER,
                                         0,
                                         meter::MeterType::Water,
                                         8 + 1,  // byte from prev value
                                         0x0701,
                                         2 ) );

        const std::vector<uint8_t> meterReport = {
            ZNET_COMMAND_CLASS_METER,
            METER_REPORT,
            3 << 0 | 1 << 5 | 1 << 7,  // meter type | rate type | scale(2)
            2 << 0 | 3 << 3 | 2 << 5,  // size | scale(1:0) | precesion
            0x07,
            0x01,  // value
            0x00,
            0x00,  // time
            0x01,
            0x02,  // prev value
            0x03   // scale2
        };

        onMessageReceived( 2, 0, meterReport, Encapsulation_None, MESSAGE_TYPE_REPORT );
    }
}

TEST_F( ZNetMonitoringHandlerTestSuite, onAntiTheftStateUpdated_ANTITHEFT_CC ) {
    getOwner().nodeCreate( 2 );
    auto node = getOwner().onFindNode( 2 );

    node->addCommandClass( 0, ZNET_COMMAND_CLASS_ANTITHEFT, 2 );
    node->getCommandClass<ZCc0x5DAntiTheft>( 0 );

    getImpl().activateMode( IZNet::Mode::Monitoring );

    // not locked
    {
        EXPECT_CALL( getOwner(), onAntiTheftStateUpdated( 2, 0, ZNET_COMMAND_CLASS_ANTITHEFT, AntiTheftInfoOpt() ) );

        const std::vector<uint8_t> antiTheftReport = {
            ZNET_COMMAND_CLASS_ANTITHEFT,
            ANTITHEFT_REPORT,
            0x01,  // status
            0x03,
            0x06,  // manufacturer id
            0x01,  // hint length
            0x21,  // hint
            0x01,
            0x02  // locking id
        };

        onMessageReceived( 2, 0, antiTheftReport, Encapsulation_None, MESSAGE_TYPE_REPORT );
    }

    // no locking id, not restricted
    {
        EXPECT_CALL( getOwner(),
                     onAntiTheftStateUpdated(
                         2,
                         0,
                         ZNET_COMMAND_CLASS_ANTITHEFT,
                         AntiTheftInfoOpt( AntiTheftInfo( false, { 0xAF, 0x01, 0xD7, 0x01, 0x21 }, 0x0306, {} ) ) ) );

        const std::vector<uint8_t> antiTheftReport = {
            ZNET_COMMAND_CLASS_ANTITHEFT,
            ANTITHEFT_REPORT,
            0x02,  // status
            0x03,
            0x06,  // manufacturer id
            0x05,  // hint length
            0xAF,
            0x01,
            0xD7,
            0x01,
            0x21  // hint
        };

        onMessageReceived( 2, 0, antiTheftReport, Encapsulation_None, MESSAGE_TYPE_REPORT );
    }

    // has locking id
    {
        EXPECT_CALL( getOwner(),
                     onAntiTheftStateUpdated(
                         2,
                         0,
                         ZNET_COMMAND_CLASS_ANTITHEFT,
                         AntiTheftInfoOpt( AntiTheftInfo( true, { 0xAF, 0x01, 0xD7, 0x01 }, 0x0205, { 0x0908 } ) ) ) );

        const std::vector<uint8_t> antiTheftReport = {
            ZNET_COMMAND_CLASS_ANTITHEFT,
            ANTITHEFT_REPORT,
            0x03,  // status
            0x02,
            0x05,  // manufacturer id
            0x04,  // hint length
            0xAF,
            0x01,
            0xD7,
            0x01,  // hint
            0x09,
            0x08  // locking id
        };

        onMessageReceived( 2, 0, antiTheftReport, Encapsulation_None, MESSAGE_TYPE_REPORT );
    }
}

TEST_F( ZNetMonitoringHandlerTestSuite, onAntiTheftStateUpdated_ANTITHEFT_UNLOCK_CC ) {
    getOwner().nodeCreate( 2 );
    auto node = getOwner().onFindNode( 2 );

    node->addCommandClass( 0, ZNET_COMMAND_CLASS_ANTITHEFT_UNLOCK, 2 );

    getImpl().activateMode( IZNet::Mode::Monitoring );

    // not locked
    {
        EXPECT_CALL( getOwner(),
                     onAntiTheftStateUpdated( 2, 0, ZNET_COMMAND_CLASS_ANTITHEFT_UNLOCK, AntiTheftInfoOpt() ) );

        const std::vector<uint8_t> antiTheftReport = {
            ZNET_COMMAND_CLASS_ANTITHEFT_UNLOCK,
            ANTITHEFT_UNLOCK_STATE_REPORT,
            1 << 1 | 2 << 2,  // wrong params: state is 0 but restructed is true and hind length is 2
            0xA9,
            0x21,  // hint
            0x01,
            0x02,  // manufacturer id
            0x01,
            0x02  // locking id
        };

        onMessageReceived( 2, 0, antiTheftReport, Encapsulation_None, MESSAGE_TYPE_REPORT );
    }

    // has locking id, not restricted
    {
        EXPECT_CALL(
            getOwner(),
            onAntiTheftStateUpdated( 2,
                                     0,
                                     ZNET_COMMAND_CLASS_ANTITHEFT_UNLOCK,
                                     AntiTheftInfoOpt( AntiTheftInfo( false, { 0xA9, 0x21 }, 0x0102, { 0x0304 } ) ) ) );

        const std::vector<uint8_t> antiTheftReport = {
            ZNET_COMMAND_CLASS_ANTITHEFT_UNLOCK,
            ANTITHEFT_UNLOCK_STATE_REPORT,
            0x01 | 2 << 2,  // hint len is 2
            0xA9,
            0x21,  // hint
            0x01,
            0x02,  // manufacturer id
            0x03,
            0x04  // locking id
        };

        onMessageReceived( 2, 0, antiTheftReport, Encapsulation_None, MESSAGE_TYPE_REPORT );
    }

    // no locking id, restricted
    {
        EXPECT_CALL( getOwner(),
                     onAntiTheftStateUpdated( 2,
                                              0,
                                              ZNET_COMMAND_CLASS_ANTITHEFT_UNLOCK,
                                              AntiTheftInfoOpt( AntiTheftInfo(
                                                  true, { 0xA9, 0x21, 0x34, 0x9F, 0x73, 0xCA }, 0x0102, {} ) ) ) );

        const std::vector<uint8_t> antiTheftReport = {
            ZNET_COMMAND_CLASS_ANTITHEFT_UNLOCK,
            ANTITHEFT_UNLOCK_STATE_REPORT,
            0x01 | 1 << 1 | 6 << 2,  // restricted, hint len is 6
            0xA9,
            0x21,
            0x34,
            0x9F,
            0x73,
            0xCA,  // hint
            0x01,
            0x02  // manufacturer id
        };

        onMessageReceived( 2, 0, antiTheftReport, Encapsulation_None, MESSAGE_TYPE_REPORT );
    }
}

TEST_F( ZNetMonitoringHandlerTestSuite, onUpdateValue_SWITCH_BINARY_CC ) {
    getOwner().nodeCreate( 2 );
    auto node = getOwner().onFindNode( 2 );

    node->addCommandClass( 0, ZNET_COMMAND_CLASS_SWITCH_BINARY, 2 );

    getImpl().activateMode( IZNet::Mode::Monitoring );

    // v1
    {
        EXPECT_CALL( getOwner(),
                     onUpdateValue( 2, ZNET_COMMAND_CLASS_SWITCH_BINARY, 0, 10, Uint8Opt(), SecondsOpt() ) );

        const std::vector<uint8_t> switchReport = { ZNET_COMMAND_CLASS_SWITCH_BINARY, SWITCH_BINARY_REPORT, 10 };

        onMessageReceived( 2, 0, switchReport, Encapsulation_None, MESSAGE_TYPE_REPORT );
    }

    // v2
    {
        EXPECT_CALL(
            getOwner(),
            onUpdateValue(
                2, ZNET_COMMAND_CLASS_SWITCH_BINARY, 0, 12, Uint8Opt( 87 ), SecondsOpt( std::chrono::minutes( 3 ) ) ) );

        const std::vector<uint8_t> switchReport = {
            ZNET_COMMAND_CLASS_SWITCH_BINARY, SWITCH_BINARY_REPORT, 12, 87, 130
        };

        onMessageReceived( 2, 0, switchReport, Encapsulation_None, MESSAGE_TYPE_REPORT );
    }
}

}  // namespace ut

}  // namespace znet
