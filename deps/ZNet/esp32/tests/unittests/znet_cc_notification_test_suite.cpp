#include <znet/znet_node.hpp>
#include <znet/cc_0x71_notification.hpp>
#include <znet/znet_utils.hpp>

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "znet_node_requests_managing_fixture.h"
#include "fake_file.h"
#include "MainFixture.h"
#include "znet_fc_send_data_mock.h"

namespace znet {

static bool operator==( const Notification& a, const Notification& b ) {
    return a.m_type == b.m_type && a.m_events == b.m_events;
}

namespace ut {

using ::testing::_;
using ::testing::ElementsAre;
using ::testing::Invoke;
using ::testing::MockFunction;

class ZCcNotificationTestSuite : public MainFixture, public NodeRequestsManagingFixture {
public:
};

TEST_F( ZCcNotificationTestSuite, interview_v1 ) {
    MockFunction<void( RequestFailTypeOpt )> callback;

    auto cc = ensureNodeAndCc<ZCc0x71Notification>( 1, 0, 1 );

    EXPECT_CALL( callback, Call( _ ) ).Times( 0 );

    EXPECT_FALSE( cc->interview( callback.AsStdFunction(), {} ) );
    EXPECT_TRUE( cc->hasInterviewDone() );

    EXPECT_TRUE( cc->isV1Supported() );
}

TEST_F( ZCcNotificationTestSuite, interview_v2 ) {
    MockFunction<void( RequestFailTypeOpt )> callback;
    bool isRequestProcessed = false;

    auto cc = ensureNodeAndCc<ZCc0x71Notification>( 1, 0, 2 );

    EXPECT_CALL( callback, Call( _ ) ).Times( 0 );

    EXPECT_TRUE( cc->interview( callback.AsStdFunction(), {} ) );

    uint8_t bitmask[32] = {
        0,
    };
    znet::utils::setBit( bitmask, static_cast<uint8_t>( NotificationType::System ) );
    znet::utils::setBit( bitmask, static_cast<uint8_t>( NotificationType::Siren ) );

    znet_notification_supported_report_t report{};
    report.v1_alarm = false;
    report.bit_mask_count = 32;
    report.bit_mask = bitmask;

    getRequestsManager().onRespond( NotificationSupportedRequest( 1, 0 ), report );

    EXPECT_FALSE( cc->hasInterviewDone() );

    znet_notification_report_t systemReport = {};
    systemReport.notification_type = static_cast<uint8_t>( NotificationType::System );
    systemReport.notification_event = 4;
    getRequestsManager().onRespond(
        NotificationV2Request( 1, 0, 0, systemReport.notification_type, systemReport.notification_event, 2 ),
        systemReport,
        isRequestProcessed );

    EXPECT_TRUE( isRequestProcessed );

    EXPECT_FALSE( cc->hasInterviewDone() );
    EXPECT_CALL( callback, Call( RequestFailTypeOpt{} ) ).Times( 1 );

    znet_notification_report_t sirenReport = {};
    sirenReport.notification_type = static_cast<uint8_t>( NotificationType::Siren );
    sirenReport.notification_event = 3;
    getRequestsManager().onRespond(
        NotificationV2Request( 1, 0, 0, sirenReport.notification_type, sirenReport.notification_event, 2 ),
        sirenReport,
        isRequestProcessed );

    EXPECT_TRUE( cc->hasInterviewDone() );

    EXPECT_FALSE( cc->isV1Supported() );

    const Notification systemNotification{ NotificationType::System, { 4 } };
    const Notification sirenNotification{ NotificationType::Siren, { 3 } };
    EXPECT_THAT( cc->getSupportedNotifications(), ElementsAre( systemNotification, sirenNotification ) );
}

TEST_F( ZCcNotificationTestSuite, interview_v3 ) {
    MockFunction<void( RequestFailTypeOpt )> callback;

    auto cc = ensureNodeAndCc<ZCc0x71Notification>( 1, 0, 3 );

    EXPECT_CALL( callback, Call( _ ) ).Times( 0 );

    EXPECT_TRUE( cc->interview( callback.AsStdFunction(), {} ) );

    uint8_t bitmask[32] = {
        0,
    };
    znet::utils::setBit( bitmask, static_cast<uint8_t>( NotificationType::System ) );
    znet::utils::setBit( bitmask, static_cast<uint8_t>( NotificationType::Siren ) );

    znet_notification_supported_report_t report{};
    report.v1_alarm = true;
    report.bit_mask_count = 32;
    report.bit_mask = bitmask;

    getRequestsManager().onRespond( NotificationSupportedRequest( 1, 0 ), report );

    EXPECT_FALSE( cc->hasInterviewDone() );

    const uint8_t systemScales = 3;
    getRequestsManager().onRespond(
        NotificationEventSupportedRequest( 1, 0, static_cast<uint8_t>( NotificationType::System ) ), &systemScales, 1 );

    EXPECT_FALSE( cc->hasInterviewDone() );
    EXPECT_CALL( callback, Call( RequestFailTypeOpt{} ) ).Times( 1 );

    const uint8_t sirenScales = 6;
    getRequestsManager().onRespond(
        NotificationEventSupportedRequest( 1, 0, static_cast<uint8_t>( NotificationType::Siren ) ), &sirenScales, 1 );

    EXPECT_TRUE( cc->hasInterviewDone() );

    EXPECT_TRUE( cc->isV1Supported() );

    const Notification systemNotification{ NotificationType::System, { 0, 1 } };
    const Notification sirenNotification{ NotificationType::Siren, { 1, 2 } };
    EXPECT_THAT( cc->getSupportedNotifications(), ElementsAre( systemNotification, sirenNotification ) );
}

TEST_F( ZCcNotificationTestSuite, interview_v3_fail ) {
    auto cc = ensureNodeAndCc<ZCc0x71Notification>( 1, 0, 3 );

    uint8_t bitmask[32] = {
        0,
    };
    znet::utils::setBit( bitmask, static_cast<uint8_t>( NotificationType::System ) );
    znet::utils::setBit( bitmask, static_cast<uint8_t>( NotificationType::GasAlarm ) );

    znet_notification_supported_report_t report{};
    report.v1_alarm = true;
    report.bit_mask_count = 32;
    report.bit_mask = bitmask;

    // reported fail
    {
        MockFunction<void( RequestFailTypeOpt )> callback;
        EXPECT_CALL( callback, Call( _ ) ).Times( 0 );

        EXPECT_TRUE( cc->interview( callback.AsStdFunction(), {} ) );

        getRequestsManager().onRespond( NotificationSupportedRequest( 1, 0 ), report );

        EXPECT_CALL( callback, Call( RequestFailTypeOpt( RequestFailType::ReportedFail ) ) ).Times( 1 );
        getRequestsManager().onFail(
            NotificationEventSupportedRequest( 1, 0, static_cast<uint8_t>( NotificationType::System ) ) );

        EXPECT_FALSE( cc->hasInterviewDone() );
    }

    // sending fail
    {
        MockFunction<void( RequestFailTypeOpt )> callback;
        EXPECT_CALL( callback, Call( _ ) ).Times( 0 );

        getRequestsManager().addFailedRequest(
            NotificationEventSupportedRequest( 1, 0, static_cast<uint8_t>( NotificationType::GasAlarm ) ) );

        EXPECT_TRUE( cc->interview( callback.AsStdFunction(), {} ) );

        getRequestsManager().onRespond( NotificationSupportedRequest( 1, 0 ), report );

        const uint8_t systemScales = 3;
        getRequestsManager().onRespond(
            NotificationEventSupportedRequest( 1, 0, static_cast<uint8_t>( NotificationType::System ) ),
            &systemScales,
            1 );

        EXPECT_CALL( callback, Call( RequestFailTypeOpt( RequestFailType::SendingError ) ) ).Times( 1 );
        getTimerProvider().addTime( DefaultPostponedRequestDelay );

        EXPECT_TRUE( cc->isV1Supported() );
        EXPECT_FALSE( cc->hasInterviewDone() );
    }

    // first sending fail
    {
        MockFunction<void( RequestFailTypeOpt )> callback;
        EXPECT_CALL( callback, Call( _ ) ).Times( 0 );

        getRequestsManager().addFailedRequest( NotificationSupportedRequest( 1, 0 ) );

        EXPECT_FALSE( cc->interview( callback.AsStdFunction(), {} ) );
        EXPECT_FALSE( cc->hasInterviewDone() );
    }
}

TEST_F( ZCcNotificationTestSuite, requestValue_v1 ) {
    MockFunction<void( RequestFailTypeOpt )> callback;

    auto cc = ensureNodeAndCc<ZCc0x71Notification>( 1, 0, 1 );

    EXPECT_CALL( callback, Call( _ ) ).Times( 0 );

    EXPECT_FALSE( cc->requestValue( createRequestValue( callback ) ) );
}

TEST_F( ZCcNotificationTestSuite, requestValue_v2 ) {
    MockFunction<void( RequestFailTypeOpt )> callback;
    bool isRequestProcessed = false;

    auto cc = ensureNodeAndCc<ZCc0x71Notification>( 1, 0, 2 );

    znet_notification_report_t systemReport = {};
    systemReport.notification_type = static_cast<uint8_t>( NotificationType::System );
    systemReport.notification_event = 4;

    znet_notification_report_t sirenReport = {};
    sirenReport.notification_type = static_cast<uint8_t>( NotificationType::Siren );
    sirenReport.notification_event = 3;

    // request value for empty cc
    {
        EXPECT_CALL( callback, Call( _ ) ).Times( 0 );
        EXPECT_FALSE( cc->requestValue( createRequestValue( callback ) ) );
    }

    eraseCommandClass( cc );

    // interview
    {
        cc->interview( {}, {} );

        uint8_t bitmask[32] = {
            0,
        };
        znet::utils::setBit( bitmask, static_cast<uint8_t>( NotificationType::System ) );
        znet::utils::setBit( bitmask, static_cast<uint8_t>( NotificationType::Siren ) );

        znet_notification_supported_report_t report{};
        report.bit_mask_count = 32;
        report.bit_mask = bitmask;

        getRequestsManager().onRespond( NotificationSupportedRequest( 1, 0 ), report );

        getRequestsManager().onRespond(
            NotificationV2Request( 1, 0, 0, systemReport.notification_type, systemReport.notification_event, 2 ),
            systemReport,
            isRequestProcessed );

        EXPECT_TRUE( isRequestProcessed );

        getRequestsManager().onRespond(
            NotificationV2Request( 1, 0, 0, sirenReport.notification_type, sirenReport.notification_event, 2 ),
            sirenReport,
            isRequestProcessed );
    }

    // request value
    {
        EXPECT_CALL( callback, Call( _ ) ).Times( 0 );
        EXPECT_TRUE( cc->requestValue( createRequestValue( callback ) ) );

        getRequestsManager().onRespond(
            NotificationV2Request( 1, 0, 0, systemReport.notification_type, systemReport.notification_event, 2 ),
            systemReport,
            isRequestProcessed );

        getRequestsManager().onRespond(
            NotificationV2Request( 1, 0, 0, systemReport.notification_type, systemReport.notification_event, 2 ),
            systemReport,
            isRequestProcessed );

        EXPECT_CALL( callback, Call( RequestFailTypeOpt{} ) ).Times( 1 );

        getRequestsManager().onRespond(
            NotificationV2Request( 1, 0, 0, sirenReport.notification_type, sirenReport.notification_event, 2 ),
            sirenReport,
            isRequestProcessed );
    }
}

TEST_F( ZCcNotificationTestSuite, requestValue_v3 ) {
    MockFunction<void( RequestFailTypeOpt )> callback;
    bool isRequestProcessed = false;

    auto cc = ensureNodeAndCc<ZCc0x71Notification>( 1, 0, 3 );

    // request value for empty cc
    {
        EXPECT_CALL( callback, Call( _ ) ).Times( 0 );
        EXPECT_FALSE( cc->requestValue( createRequestValue( callback ) ) );
    }

    // interview
    {
        cc->interview( {}, {} );

        uint8_t bitmask[32] = {
            0,
        };
        znet::utils::setBit( bitmask, static_cast<uint8_t>( NotificationType::System ) );
        znet::utils::setBit( bitmask, static_cast<uint8_t>( NotificationType::Siren ) );

        znet_notification_supported_report_t report{};
        report.bit_mask_count = 32;
        report.bit_mask = bitmask;

        getRequestsManager().onRespond( NotificationSupportedRequest( 1, 0 ), report );

        const uint8_t systemScales = 3;
        getRequestsManager().onRespond(
            NotificationEventSupportedRequest( 1, 0, static_cast<uint8_t>( NotificationType::System ) ),
            &systemScales,
            1 );

        const uint8_t sirenScales = 10;
        getRequestsManager().onRespond(
            NotificationEventSupportedRequest( 1, 0, static_cast<uint8_t>( NotificationType::Siren ) ),
            &sirenScales,
            1 );
    }

    getRequestsManager().setUseFakeRequests( false );

    // request value
    {
        FcSendDataMock sendDataMock;
        znet_notification_report_t systemReport = {};
        systemReport.notification_type = static_cast<uint8_t>( NotificationType::System );

        znet_notification_report_t sirenReport = {};
        sirenReport.notification_type = static_cast<uint8_t>( NotificationType::Siren );

        EXPECT_CALL(
            sendDataMock,
            fcSendData( 1,
                        0,
                        std::vector<uint8_t>{
                            ZNET_COMMAND_CLASS_NOTIFICATION, NOTIFICATION_GET_V3, 0, systemReport.notification_type },
                        true,
                        true,
                        Encapsulation_None ) );

        EXPECT_CALL( callback, Call( _ ) ).Times( 0 );
        EXPECT_TRUE( cc->requestValue( createRequestValue( callback ) ) );

        EXPECT_CALL(
            sendDataMock,
            fcSendData( 1,
                        0,
                        std::vector<uint8_t>{
                            ZNET_COMMAND_CLASS_NOTIFICATION, NOTIFICATION_GET_V3, 0, sirenReport.notification_type },
                        true,
                        true,
                        Encapsulation_None ) );

        systemReport.notification_event = 1;
        getRequestsManager().onRespond(
            NotificationV2Request( 1, 0, 0, systemReport.notification_type, systemReport.notification_event, 3 ),
            systemReport,
            isRequestProcessed );

        EXPECT_CALL( callback, Call( RequestFailTypeOpt{} ) ).Times( 1 );

        sirenReport.notification_event = 3;
        getRequestsManager().onRespond(
            NotificationV2Request( 1, 0, 0, sirenReport.notification_type, sirenReport.notification_event, 3 ),
            sirenReport,
            isRequestProcessed );
    }
}

}  // namespace ut
}  // namespace znet
