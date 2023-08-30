#include <znet/znet_node.hpp>
#include <znet/cc_0x63_user_code.hpp>

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <znet_lib_cc_codes.h>

#include "znet_node_requests_managing_fixture.h"
#include "znet/znet_utils.hpp"
#include "fake_file.h"
#include "MainFixture.h"

#include "znet_fc_send_data_mock.h"

namespace znet {

namespace ut {

using ::testing::_;
using ::testing::ElementsAre;
using ::testing::Invoke;
using ::testing::MockFunction;

class ZCcUserCodeTestSuite : public MainFixture, public NodeRequestsManagingFixture {
public:
};

TEST_F( ZCcUserCodeTestSuite, interview_v1 ) {
    auto cc = ensureNodeAndCc<ZCc0x63UserCode>( 1, 0, 1 );

    MockFunction<void( RequestFailTypeOpt )> callback;

    EXPECT_EQ( cc->getSupportedUsersCount(), 0 );

    EXPECT_CALL( callback, Call( _ ) ).Times( 0 );

    EXPECT_TRUE( cc->interview( callback.AsStdFunction(), {} ) );

    EXPECT_FALSE( cc->hasInterviewDone() );
    EXPECT_CALL( callback, Call( RequestFailTypeOpt{} ) ).Times( 1 );

    getRequestsManager().onRespond( UserCodesNumberRequest( 1, 0 ), 3 );

    EXPECT_TRUE( cc->hasInterviewDone() );

    EXPECT_EQ( cc->getSupportedUsersCount(), 3 );
    EXPECT_EQ( cc->getCapabilities(), nullptr );
}

TEST_F( ZCcUserCodeTestSuite, interview_v2 ) {
    auto cc = ensureNodeAndCc<ZCc0x63UserCode>( 1, 0, 2 );

    MockFunction<void( RequestFailTypeOpt )> callback;

    EXPECT_EQ( cc->getSupportedUsersCount(), 0 );

    EXPECT_CALL( callback, Call( _ ) ).Times( 0 );

    EXPECT_TRUE( cc->interview( callback.AsStdFunction(), {} ) );

    EXPECT_FALSE( cc->hasInterviewDone() );

    getRequestsManager().onRespond( UserCodesNumberRequest( 1, 0 ), 5 );

    EXPECT_CALL( callback, Call( RequestFailTypeOpt{} ) ).Times( 1 );

    znet_user_code_capabilities_report_t capabilitiesReport{};
    getRequestsManager().onRespond( UserCodeCapabilitiesRequest( 1, 0 ), capabilitiesReport );

    EXPECT_TRUE( cc->hasInterviewDone() );

    EXPECT_EQ( cc->getSupportedUsersCount(), 5 );
    ASSERT_NE( cc->getCapabilities(), nullptr );

    // TODO: check capabilities
}

TEST_F( ZCcUserCodeTestSuite, requestValue_v1 ) {
    auto userCodeCc = ensureNodeAndCc<ZCc0x63UserCode>( 1, 0, 1 );

    userCodeCc->interview( {}, {} );
    getRequestsManager().onRespond( UserCodesNumberRequest( 1, 0 ), 3 );

    MockFunction<void( RequestFailTypeOpt )> callback;
    EXPECT_CALL( callback, Call( _ ) ).Times( 0 );

    EXPECT_TRUE( userCodeCc->requestValue( createRequestValue( callback ) ) );

    UserCode::UserPollStatus pollStatus = UserCode::UserPollStatus::Idle;

    znet_user_code_report_t report1{};
    report1.user_id = 1;
    getRequestsManager().onRespond( UserCodesRequest( 1, 0, 1 ), report1, &pollStatus );
    EXPECT_EQ( pollStatus, UserCode::UserPollStatus::Polling );

    znet_user_code_report_t report2{};
    report2.user_id = 2;
    getRequestsManager().onRespond( UserCodesRequest( 1, 0, 2 ), report2, &pollStatus );
    EXPECT_EQ( pollStatus, UserCode::UserPollStatus::Polling );

    EXPECT_CALL( callback, Call( RequestFailTypeOpt{} ) ).Times( 1 );

    znet_user_code_report_t report3{};
    report3.user_id = 3;
    getRequestsManager().onRespond( UserCodesRequest( 1, 0, 3 ), report3, &pollStatus );
    EXPECT_EQ( pollStatus, UserCode::UserPollStatus::Finished );
}

TEST_F( ZCcUserCodeTestSuite, severalRequestValues_v1 ) {
    auto userCodeCc = ensureNodeAndCc<ZCc0x63UserCode>( 1, 0, 1 );

    userCodeCc->interview( {}, {} );
    getRequestsManager().onRespond( UserCodesNumberRequest( 1, 0 ), 3 );

    MockFunction<void( RequestFailTypeOpt )> callback1;
    EXPECT_CALL( callback1, Call( _ ) ).Times( 0 );

    EXPECT_TRUE( userCodeCc->requestValue( createRequestValue( callback1 ) ) );

    UserCode::UserPollStatus pollStatus = UserCode::UserPollStatus::Idle;

    znet_user_code_report_t report1{};
    report1.user_id = 1;
    getRequestsManager().onRespond( UserCodesRequest( 1, 0, 1 ), report1, &pollStatus );
    EXPECT_EQ( pollStatus, UserCode::UserPollStatus::Polling );

    MockFunction<void( RequestFailTypeOpt )> callback2;
    EXPECT_CALL( callback2, Call( _ ) ).Times( 0 );
    EXPECT_TRUE( userCodeCc->requestValue( createRequestValue( callback2 ) ) );

    EXPECT_CALL( callback1, Call( RequestFailTypeOpt{} ) ).Times( 1 );

    znet_user_code_report_t report2{};
    report2.user_id = 2;
    getRequestsManager().onRespond( UserCodesRequest( 1, 0, 2 ), report2, &pollStatus );
    EXPECT_EQ( pollStatus, UserCode::UserPollStatus::Polling );

    EXPECT_CALL( callback1, Call( _ ) ).Times( 0 );

    pollStatus = UserCode::UserPollStatus::Idle;

    getRequestsManager().onRespond( UserCodesRequest( 1, 0, 1 ), report1, &pollStatus );
    EXPECT_EQ( pollStatus, UserCode::UserPollStatus::Polling );

    getRequestsManager().onRespond( UserCodesRequest( 1, 0, 2 ), report2, &pollStatus );
    EXPECT_EQ( pollStatus, UserCode::UserPollStatus::Polling );

    EXPECT_CALL( callback2, Call( RequestFailTypeOpt{} ) ).Times( 1 );

    znet_user_code_report_t report3{};
    report2.user_id = 3;
    getRequestsManager().onRespond( UserCodesRequest( 1, 0, 3 ), report3, &pollStatus );
    EXPECT_EQ( pollStatus, UserCode::UserPollStatus::Finished );
}

class ZCcUserCodeTestSuiteRequestValueV2Parameteric
: public ZCcUserCodeTestSuite,
  public ::testing::WithParamInterface<std::tuple<bool, bool, bool, bool> > {};

INSTANTIATE_TEST_SUITE_P( parametric,
                          ZCcUserCodeTestSuiteRequestValueV2Parameteric,
                          ::testing::Values( std::make_tuple( true, true, true, true ),
                                             std::make_tuple( false, true, true, true ),
                                             std::make_tuple( true, false, true, true ),
                                             std::make_tuple( false, false, true, true ),
                                             std::make_tuple( false, true, false, true ),
                                             std::make_tuple( true, false, false, true ),

                                             std::make_tuple( true, true, true, false ),
                                             std::make_tuple( false, true, true, false ),
                                             std::make_tuple( true, false, true, false ),
                                             std::make_tuple( false, false, true, false ),
                                             std::make_tuple( false, true, false, false ),
                                             std::make_tuple( true, false, false, false ) ) );

TEST_P( ZCcUserCodeTestSuiteRequestValueV2Parameteric, requestValue_v2 ) {
    const auto hasMasterCode = std::get<0>( GetParam() );
    const auto hasKeyModes = std::get<1>( GetParam() );
    const auto hasUserCodes = std::get<2>( GetParam() );
    const auto checksumSuported = std::get<3>( GetParam() );

    auto userCodeCc = ensureNodeAndCc<ZCc0x63UserCode>( 1, 0, 2 );

    MockFunction<void( RequestFailTypeOpt )> callback;
    EXPECT_CALL( callback, Call( _ ) ).Times( 0 );

    EXPECT_FALSE( userCodeCc->requestValue( createRequestValue( callback ) ) );

    userCodeCc->interview( {}, {} );
    getRequestsManager().onRespond( UserCodesNumberRequest( 1, 0 ), hasUserCodes ? 25 : 0 );

    znet_user_code_capabilities_report_t capabilitiesReport{};
    uint8_t keypadModes = 5;
    capabilitiesReport.keypad_modes_capability.bitmask = &keypadModes;
    capabilitiesReport.keypad_modes_capability.bitmask_length = hasKeyModes;
    capabilitiesReport.user_id_status_capability.MC_support = hasMasterCode;
    capabilitiesReport.keypad_modes_capability.UCC_support = checksumSuported;
    getRequestsManager().onRespond( UserCodeCapabilitiesRequest( 1, 0 ), capabilitiesReport );

    EXPECT_TRUE( userCodeCc->requestValue( createRequestValue( callback ) ) );

    EXPECT_CALL( callback, Call( RequestFailTypeOpt{} ) )
        .Times( hasMasterCode && !hasKeyModes && !checksumSuported && !hasUserCodes );

    EXPECT_EQ( getRequestsManager().isWaitingForRespond( MasterCodeRequest( 1, 0 ) ), hasMasterCode );
    getRequestsManager().onRespond( MasterCodeRequest( 1, 0 ), 0, nullptr );

    EXPECT_CALL( callback, Call( RequestFailTypeOpt{} ) ).Times( hasKeyModes && !checksumSuported && !hasUserCodes );

    EXPECT_EQ( getRequestsManager().isWaitingForRespond( UserCodesKeypadModeRequest( 1, 0 ) ), hasKeyModes );
    getRequestsManager().onRespond( UserCodesKeypadModeRequest( 1, 0 ), 0 );

    EXPECT_CALL( callback, Call( RequestFailTypeOpt{} ) ).Times( checksumSuported && !hasUserCodes );

    EXPECT_EQ( getRequestsManager().isWaitingForRespond( UserCodeChecksumRequest( 1, 0 ) ), checksumSuported );
    getRequestsManager().onRespond( UserCodeChecksumRequest( 1, 0 ), 0 );

    EXPECT_EQ( getRequestsManager().isWaitingForRespond( ExtendedCodesRequest( 1, 0, 1, {} ) ), hasUserCodes );
    EXPECT_FALSE( getRequestsManager().isWaitingForRespond( UserCodesRequest( 1, 0, 1 ) ) );

    if( !hasUserCodes ) {
        return;
    }

    UserCode::UserPollStatus pollStatus = UserCode::UserPollStatus::Idle;

    znet_user_code_report_t report1{};
    report1.user_id = 1;
    getRequestsManager().onRespond( ExtendedCodesRequest( 1, 0, 1, {} ), 1, &report1, 2, &pollStatus );
    EXPECT_EQ( pollStatus, UserCode::UserPollStatus::Polling );

    znet_user_code_report_t report2[2] = {
        znet_user_code_report_t{},
    };
    report2[0].user_id = 2;
    report2[1].user_id = 4;
    EXPECT_FALSE( getRequestsManager().isWaitingForRespond( UserCodesRequest( 1, 0, 2 ) ) );
    getRequestsManager().onRespond( ExtendedCodesRequest( 1, 0, 2, {} ), 2, report2, 6, &pollStatus );
    EXPECT_EQ( pollStatus, UserCode::UserPollStatus::Polling );

    EXPECT_CALL( callback, Call( RequestFailTypeOpt{} ) ).Times( 1 );

    znet_user_code_report_t report3[7] = {
        znet_user_code_report_t{},
    };
    for( uint8_t i = 0; i < 7; ++i ) {
        report3[i].user_id = i * 2 + 6;
    }
    EXPECT_FALSE( getRequestsManager().isWaitingForRespond( UserCodesRequest( 1, 0, 6 ) ) );
    getRequestsManager().onRespond( ExtendedCodesRequest( 1, 0, 6, {} ), 7, report3, 0, &pollStatus );
    EXPECT_EQ( pollStatus, UserCode::UserPollStatus::Finished );
}

TEST_F( ZCcUserCodeTestSuite, requestSingeUserCode ) {
    auto userCodeCc = ensureNodeAndCc<ZCc0x63UserCode>( 1, 0, 1 );

    userCodeCc->interview( {}, {} );
    getRequestsManager().onRespond( UserCodesNumberRequest( 1, 0 ), 3 );

    MockFunction<void( RequestFailTypeOpt )> callback;

    EXPECT_CALL( callback, Call( RequestFailTypeOpt{} ) ).Times( 1 );

    RequestValueUserCode request;
    request.userId = 2;
    request.resultCallback = callback.AsStdFunction();
    EXPECT_TRUE( userCodeCc->requestValue( request ) );

    UserCode::UserPollStatus pollStatus = UserCode::UserPollStatus::Idle;

    znet_user_code_report_t report{};
    report.user_id = 2;
    getRequestsManager().onRespond( UserCodesRequest( 1, 0, 2 ), report, &pollStatus );
    EXPECT_EQ( pollStatus, UserCode::UserPollStatus::Idle );

    EXPECT_FALSE( getRequestsManager().isWaitingForRespond( UserCodesRequest( 1, 0, 1 ) ) );
    EXPECT_FALSE( getRequestsManager().isWaitingForRespond( UserCodesRequest( 1, 0, 2 ) ) );
    EXPECT_FALSE( getRequestsManager().isWaitingForRespond( UserCodesRequest( 1, 0, 3 ) ) );
}

TEST_F( ZCcUserCodeTestSuite, requestValueAndSingleCodes ) {
    auto userCodeCc = ensureNodeAndCc<ZCc0x63UserCode>( 1, 0, 1 );

    userCodeCc->interview( {}, {} );
    getRequestsManager().onRespond( UserCodesNumberRequest( 1, 0 ), 3 );

    MockFunction<void( RequestFailTypeOpt )> allValuesCallback;
    EXPECT_CALL( allValuesCallback, Call( _ ) ).Times( 0 );

    MockFunction<void( RequestFailTypeOpt )> singleValuesCallback;
    EXPECT_CALL( singleValuesCallback, Call( _ ) ).Times( 0 );

    EXPECT_TRUE( userCodeCc->requestValue( createRequestValue( allValuesCallback ) ) );

    {
        RequestValueUserCode request;
        request.userId = 2;
        request.resultCallback = singleValuesCallback.AsStdFunction();
        EXPECT_TRUE( userCodeCc->requestValue( request ) );

        UserCode::UserPollStatus pollStatus = UserCode::UserPollStatus::Idle;

        EXPECT_CALL( singleValuesCallback, Call( RequestFailTypeOpt{} ) ).Times( 1 );
        znet_user_code_report_t report2{};
        report2.user_id = 2;
        getRequestsManager().onRespond( UserCodesRequest( 1, 0, 2 ), report2, &pollStatus );
        EXPECT_EQ( pollStatus, UserCode::UserPollStatus::Idle );

        znet_user_code_report_t report1{};
        report2.user_id = 1;
        getRequestsManager().onRespond( UserCodesRequest( 1, 0, 1 ), report1, &pollStatus );
        EXPECT_EQ( pollStatus, UserCode::UserPollStatus::Polling );
    }

    {
        RequestValueUserCode request;
        request.userId = 2;
        request.resultCallback = singleValuesCallback.AsStdFunction();
        EXPECT_TRUE( userCodeCc->requestValue( request ) );

        UserCode::UserPollStatus pollStatus = UserCode::UserPollStatus::Idle;

        EXPECT_CALL( singleValuesCallback, Call( RequestFailTypeOpt{} ) ).Times( 1 );
        znet_user_code_report_t report{};
        report.user_id = 2;
        getRequestsManager().onRespond( UserCodesRequest( 1, 0, 2 ), report, &pollStatus );
        EXPECT_EQ( pollStatus, UserCode::UserPollStatus::Polling );
    }

    {
        RequestValueUserCode request;
        request.userId = 3;
        request.resultCallback = singleValuesCallback.AsStdFunction();
        EXPECT_TRUE( userCodeCc->requestValue( request ) );

        UserCode::UserPollStatus pollStatus = UserCode::UserPollStatus::Idle;

        EXPECT_CALL( singleValuesCallback, Call( RequestFailTypeOpt{} ) ).Times( 1 );
        EXPECT_CALL( allValuesCallback, Call( RequestFailTypeOpt{} ) ).Times( 1 );

        znet_user_code_report_t report{};
        report.user_id = 3;
        getRequestsManager().onRespond( UserCodesRequest( 1, 0, 3 ), report, &pollStatus );
        EXPECT_EQ( pollStatus, UserCode::UserPollStatus::Finished );
    }
}

TEST_F( ZCcUserCodeTestSuite, requestValue_v1_fail ) {
    auto userCodeCc = ensureNodeAndCc<ZCc0x63UserCode>( 1, 0, 1 );

    userCodeCc->interview( {}, {} );
    getRequestsManager().onRespond( UserCodesNumberRequest( 1, 0 ), 3 );

    MockFunction<void( RequestFailTypeOpt )> callback;
    EXPECT_CALL( callback, Call( _ ) ).Times( 0 );

    znet_user_code_report_t report1{};
    report1.user_id = 1;

    znet_user_code_report_t report2{};
    report2.user_id = 2;

    // sending fail
    {
        getRequestsManager().addFailedRequest( UserCodesRequest( 1, 0, 3 ) );

        EXPECT_TRUE( userCodeCc->requestValue( createRequestValue( callback ) ) );

        getRequestsManager().onRespond( UserCodesRequest( 1, 0, 1 ), report1, nullptr );

        EXPECT_CALL( callback, Call( RequestFailTypeOpt{ RequestFailType::SendingError } ) ).Times( 1 );

        getRequestsManager().onRespond( UserCodesRequest( 1, 0, 2 ), report2, nullptr );
        getTimerProvider().addTime( DefaultPostponedRequestDelay );
    }

    // reported fail
    {
        EXPECT_TRUE( userCodeCc->requestValue( createRequestValue( callback ) ) );

        getRequestsManager().onRespond( UserCodesRequest( 1, 0, 1 ), report1, nullptr );

        EXPECT_CALL( callback, Call( RequestFailTypeOpt{ RequestFailType::ReportedFail } ) ).Times( 1 );
        getRequestsManager().onFail( UserCodesRequest( 1, 0, 2 ) );
    }

    // first sending fail
    {
        userCodeCc->getParentNode().setRequestDelay( std::chrono::seconds( 1 ) + DefaultPostponedRequestDelay );
        getRequestsManager().addFailedRequest( UserCodesRequest( 1, 0, 1 ) );
        EXPECT_CALL( callback, Call( _ ) ).Times( 0 );

        EXPECT_TRUE( userCodeCc->requestValue( createRequestValue( callback ) ) );
        getTimerProvider().addTime( DefaultPostponedRequestDelay );

        EXPECT_CALL( callback, Call( _ ) ).Times( 1 );
        getTimerProvider().addTime( std::chrono::seconds( 1 ) );
    }
}

TEST_F( ZCcUserCodeTestSuite, requestValue_v2_fail ) {
    auto userCodeCc = ensureNodeAndCc<ZCc0x63UserCode>( 1, 0, 2 );

    userCodeCc->interview( {}, {} );
    getRequestsManager().onRespond( UserCodesNumberRequest( 1, 0 ), 5 );

    MockFunction<void( RequestFailTypeOpt )> callback;
    EXPECT_CALL( callback, Call( _ ) ).Times( 0 );

    // TODO: investigate: how to handle MasterCodeRequest fail and UserCodesKeypadModeRequest fail
    getRequestsManager().addFailedRequest( MasterCodeRequest( 1, 0 ) );
    getRequestsManager().addFailedRequest( UserCodesKeypadModeRequest( 1, 0 ) );
    getRequestsManager().addFailedRequest( UserCodeChecksumRequest( 1, 0 ) );

    // reported fail
    {
        EXPECT_TRUE( userCodeCc->requestValue( createRequestValue( callback ) ) );

        znet_user_code_report_t report1{};
        report1.user_id = 1;
        getRequestsManager().onRespond( ExtendedCodesRequest( 1, 0, 1, {} ), 1, &report1, 4, nullptr );

        EXPECT_CALL( callback, Call( RequestFailTypeOpt{ RequestFailType::ReportedFail } ) ).Times( 1 );
        getRequestsManager().onFail( ExtendedCodesRequest( 1, 0, 4, {} ) );
    }
}

TEST_F( ZCcUserCodeTestSuite, setUserCode_v1 ) {
    auto userCodeCc = ensureNodeAndCc<ZCc0x63UserCode>( 1, 0, 1 );
    userCodeCc->interview( {}, {} );
    getRequestsManager().onRespond( UserCodesNumberRequest( 1, 0 ), 200 );

    FcSendDataMock sendDataMock;

    {
        const std::vector<uint8_t> expectedData = {
            ZNET_COMMAND_CLASS_USER_CODE,
            USER_CODE_SET,
            100,
            0,  // status
            0,
            0,
            0,
            0  // code
        };

        EXPECT_CALL( sendDataMock, fcSendData( 1, 0, expectedData, false, false, Encapsulation_None ) );
        userCodeCc->setUserCode( 100, 0 /*available status*/, "12345" );
    }

    {
        const std::vector<uint8_t> expectedData = {
            ZNET_COMMAND_CLASS_USER_CODE,
            USER_CODE_SET,
            100,  // user id
            2,    // status
            '1',
            '2',
            '3',
            '4',
            '5'  // code
        };

        EXPECT_CALL( sendDataMock, fcSendData( 1, 0, expectedData, false, false, Encapsulation_None ) );
        userCodeCc->setUserCode( 100, 2, "12345" );
    }
}

TEST_F( ZCcUserCodeTestSuite, setUserCode_v2 ) {
    auto userCodeCc = ensureNodeAndCc<ZCc0x63UserCode>( 1, 0, 2 );
    userCodeCc->interview( {}, {} );
    getRequestsManager().onRespond( UserCodesNumberRequest( 1, 0 ), 200 );
    getRequestsManager().onRespond( UserCodeCapabilitiesRequest( 1, 0 ), znet_user_code_capabilities_report_t{} );

    FcSendDataMock sendDataMock;

    {
        const std::vector<uint8_t> expectedData = {
            ZNET_COMMAND_CLASS_USER_CODE,
            EXTENDED_USER_CODE_SET,
            1,  // number of user codes
            0,
            100,  // user id
            0,    // status
            0,    // length
        };

        EXPECT_CALL( sendDataMock, fcSendData( 1, 0, expectedData, false, false, Encapsulation_None ) );
        userCodeCc->setUserCode( 100, 0 /*available status*/, "12345" );
    }

    {
        const std::vector<uint8_t> expectedData = {
            ZNET_COMMAND_CLASS_USER_CODE,
            EXTENDED_USER_CODE_SET,
            1,  // number of user codes
            0,
            100,  // user id
            2,    // status
            5,    // length
            '1',
            '2',
            '3',
            '4',
            '5'  // code
        };

        EXPECT_CALL( sendDataMock, fcSendData( 1, 0, expectedData, false, false, Encapsulation_None ) );
        userCodeCc->setUserCode( 100, 2, "12345" );
    }

    {
        const std::vector<uint8_t> expectedData = {
            ZNET_COMMAND_CLASS_USER_CODE,
            EXTENDED_USER_CODE_SET,
            1,  // number of user codes
            0x04,
            0x01,  // user id (1025)
            3,     // status
            4,     // length
            '1',
            '2',
            '3',
            '4'  // code
        };

        EXPECT_CALL( sendDataMock, fcSendData( 1, 0, expectedData, false, false, Encapsulation_None ) );
        userCodeCc->setUserCode( 1025, 3, "1234" );
    }
}

TEST_F( ZCcUserCodeTestSuite, setMasterCode ) {
    auto userCodeCc = ensureNodeAndCc<ZCc0x63UserCode>( 1, 0, 2 );
    userCodeCc->interview( {}, {} );
    getRequestsManager().onRespond( UserCodesNumberRequest( 1, 0 ), 200 );

    znet_user_code_capabilities_report_t report{};
    report.user_id_status_capability.MC_support = 1;
    report.user_id_status_capability.MCD_support = 1;
    getRequestsManager().onRespond( UserCodeCapabilitiesRequest( 1, 0 ), report );

    FcSendDataMock sendDataMock;

    {
        const std::vector<uint8_t> expectedData = {
            ZNET_COMMAND_CLASS_USER_CODE,
            MASTER_CODE_SET,
            5,  // length
            '1',
            '2',
            '3',
            '4',
            '5'  // code
        };

        EXPECT_CALL( sendDataMock, fcSendData( 1, 0, expectedData, false, false, Encapsulation_None ) );
        userCodeCc->setMasterCode( "12345" );
    }

    {
        const std::vector<uint8_t> expectedData = {
            ZNET_COMMAND_CLASS_USER_CODE,
            MASTER_CODE_SET,
            0,  // length
        };

        EXPECT_CALL( sendDataMock, fcSendData( 1, 0, expectedData, false, false, Encapsulation_None ) );
        userCodeCc->setMasterCode( "" );
    }
}

TEST_F( ZCcUserCodeTestSuite, setKeypadMode ) {
    auto userCodeCc = ensureNodeAndCc<ZCc0x63UserCode>( 1, 0, 2 );
    userCodeCc->interview( {}, {} );
    getRequestsManager().onRespond( UserCodesNumberRequest( 1, 0 ), 200 );

    znet_user_code_capabilities_report_t report{};
    getRequestsManager().onRespond( UserCodeCapabilitiesRequest( 1, 0 ), report );

    FcSendDataMock sendDataMock;

    {
        const std::vector<uint8_t> expectedData = { ZNET_COMMAND_CLASS_USER_CODE, USER_CODE_KEYPAD_MODE_SET, 0x02 };

        EXPECT_CALL( sendDataMock, fcSendData( 1, 0, expectedData, false, false, Encapsulation_None ) );
        userCodeCc->setKeypadMode( UserCode::KeypadMode::Privacy );
    }

    {
        const std::vector<uint8_t> expectedData = { ZNET_COMMAND_CLASS_USER_CODE, USER_CODE_KEYPAD_MODE_SET, 0x00 };

        EXPECT_CALL( sendDataMock, fcSendData( 1, 0, expectedData, false, false, Encapsulation_None ) );
        userCodeCc->setKeypadMode( UserCode::KeypadMode::Normal );
    }
}

}  // namespace ut
}  // namespace znet
