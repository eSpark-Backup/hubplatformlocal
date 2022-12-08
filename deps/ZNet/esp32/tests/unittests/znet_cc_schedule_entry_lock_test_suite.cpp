#include <znet/znet_node.hpp>
#include <znet/cc_0x4e_schedule_entry_lock.hpp>
#include <znet/cc_0x63_user_code.hpp>

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

class ZCcScheduleEntryLockTestSuite : public ::testing::Test, public NodeRequestsManagingFixture {
public:
};

TEST_F( ZCcScheduleEntryLockTestSuite, interview ) {
    getNodesProvider().nodeCreate( 1 );
    auto node = getNodesProvider().onFindNode( 1 );

    MockFunction<void( RequestFailTypeOpt )> callback;

    node->addCommandClass( 0, ZNET_COMMAND_CLASS_SCHEDULE_ENTRY_LOCK, 1 );
    auto cc = node->getCommandClass<ZCc0x4EScheduleEntryLock>( 0 );

    EXPECT_CALL( callback, Call( _ ) ).Times( 0 );

    EXPECT_TRUE( cc->interview( callback.AsStdFunction(), {} ) );

    EXPECT_FALSE( cc->hasInterviewDone() );
    EXPECT_CALL( callback, Call( RequestFailTypeOpt{} ) ).Times( 1 );

    znet_cmd_schedule_entry_lock_supported_report_t report{};
    report.week_day_slots = 4;
    report.year_day_slots = 10;
    report.daily_repeating_slots = 3;
    node->getRequestsManager().onRespond( ScheduleEntryLockSupportedRequest( 1, 0 ), report );

    EXPECT_TRUE( cc->hasInterviewDone() );

    EXPECT_EQ( cc->getWeekDaySlots(), 4 );
    EXPECT_EQ( cc->getYearDaySlots(), 10 );
    EXPECT_EQ( cc->getDailyRepeatingSlots(), 3 );
}

TEST_F( ZCcScheduleEntryLockTestSuite, requestValue ) {
    getNodesProvider().nodeCreate( 1 );
    auto node = getNodesProvider().onFindNode( 1 );

    node->addCommandClass( 0, ZNET_COMMAND_CLASS_SCHEDULE_ENTRY_LOCK, 1 );
    node->addCommandClass( 0, ZNET_COMMAND_CLASS_USER_CODE, 1 );
    auto scheduleEntryLockCc = node->getCommandClass<ZCc0x4EScheduleEntryLock>( 0 );
    auto userCodeCc = node->getCommandClass<ZCc0x63UserCode>( 0 );

    // cc interviews
    {
        scheduleEntryLockCc->interview( {}, {} );

        znet_cmd_schedule_entry_lock_supported_report_t supportedSlotsReport{};
        supportedSlotsReport.week_day_slots = 2;
        supportedSlotsReport.year_day_slots = 3;
        supportedSlotsReport.daily_repeating_slots = 4;
        node->getRequestsManager().onRespond( ScheduleEntryLockSupportedRequest( 1, 0 ), supportedSlotsReport );

        userCodeCc->interview( {}, {} );
        node->getRequestsManager().onRespond( UserCodesNumberRequest( 1, 0 ), 3 );
    }

    // fill user codes cache
    {
        userCodeCc->getUserIdsCache().onUserCodeUpdated( 1, UserCode::UserIDStatus::Available );
        userCodeCc->getUserIdsCache().onUserCodeUpdated( 2, UserCode::UserIDStatus::Enabled );
        userCodeCc->getUserIdsCache().onUserCodeUpdated( 3, UserCode::UserIDStatus::Enabled );
    }

    // request schedule entry lock values
    {
        MockFunction<void( RequestFailTypeOpt )> callback;
        EXPECT_CALL( callback, Call( _ ) ).Times( 0 );

        EXPECT_TRUE( scheduleEntryLockCc->requestValue( createRequestValue( callback ) ) );

        std::vector<uint8_t> userIds;
        userIds.assign( userCodeCc->getUserIdsCache().getUserIds().begin(),
                        userCodeCc->getUserIdsCache().getUserIds().end() );
        std::reverse( userIds.begin(), userIds.end() );

        size_t userIndex = 0;
        for( auto userId : userIds ) {
            ++userIndex;

            for( uint8_t slotId = 1; slotId <= 2; ++slotId ) {
                EXPECT_TRUE(
                    node->getRequestsManager().isWaitingForRespond( WeekDayScheduleRequest( 1, 0, userId, slotId ) ) );
                node->getRequestsManager().onRespond( WeekDayScheduleRequest( 1, 0, userId, slotId ),
                                                      znet_week_day_schedule_t{} );
            }

            for( uint8_t slotId = 1; slotId <= 3; ++slotId ) {
                EXPECT_TRUE(
                    node->getRequestsManager().isWaitingForRespond( YearDaySchduleRequest( 1, 0, userId, slotId ) ) );
                node->getRequestsManager().onRespond( YearDaySchduleRequest( 1, 0, userId, slotId ),
                                                      znet_year_day_schedule_t{} );
            }

            for( uint8_t slotId = 1; slotId <= 4; ++slotId ) {
                EXPECT_CALL( callback, Call( RequestFailTypeOpt{} ) )
                    .Times( userIndex == userCodeCc->getUserIdsCache().getUserIds().size() && slotId == 4 );

                EXPECT_TRUE( node->getRequestsManager().isWaitingForRespond(
                    DailyRepeatingScheduleRequest( 1, 0, userId, slotId ) ) );
                node->getRequestsManager().onRespond( DailyRepeatingScheduleRequest( 1, 0, userId, slotId ),
                                                      znet_daily_repeating_schedule_t{} );
            }
        }
    }
}

TEST_F( ZCcScheduleEntryLockTestSuite, requestValueYearSlotsOnly ) {
    getNodesProvider().nodeCreate( 1 );
    auto node = getNodesProvider().onFindNode( 1 );

    node->addCommandClass( 0, ZNET_COMMAND_CLASS_SCHEDULE_ENTRY_LOCK, 1 );
    node->addCommandClass( 0, ZNET_COMMAND_CLASS_USER_CODE, 1 );
    auto scheduleEntryLockCc = node->getCommandClass<ZCc0x4EScheduleEntryLock>( 0 );
    auto userCodeCc = node->getCommandClass<ZCc0x63UserCode>( 0 );

    // cc interviews
    {
        scheduleEntryLockCc->interview( {}, {} );

        znet_cmd_schedule_entry_lock_supported_report_t supportedSlotsReport{};
        supportedSlotsReport.year_day_slots = 3;
        node->getRequestsManager().onRespond( ScheduleEntryLockSupportedRequest( 1, 0 ), supportedSlotsReport );

        userCodeCc->interview( {}, {} );
        node->getRequestsManager().onRespond( UserCodesNumberRequest( 1, 0 ), 3 );
    }

    // fill user codes cache
    {
        userCodeCc->getUserIdsCache().onUserCodeUpdated( 1, UserCode::UserIDStatus::Available );
        userCodeCc->getUserIdsCache().onUserCodeUpdated( 2, UserCode::UserIDStatus::Enabled );
        userCodeCc->getUserIdsCache().onUserCodeUpdated( 3, UserCode::UserIDStatus::Enabled );
    }

    // request schedule entry lock values
    {
        MockFunction<void( RequestFailTypeOpt )> callback;
        EXPECT_CALL( callback, Call( _ ) ).Times( 0 );

        EXPECT_TRUE( scheduleEntryLockCc->requestValue( createRequestValue( callback ) ) );

        std::vector<uint8_t> userIds;
        userIds.assign( userCodeCc->getUserIdsCache().getUserIds().begin(),
                        userCodeCc->getUserIdsCache().getUserIds().end() );
        std::reverse( userIds.begin(), userIds.end() );

        size_t userIndex = 0;
        for( auto userId : userIds ) {
            ++userIndex;

            for( uint8_t slotId = 1; slotId <= 3; ++slotId ) {
                EXPECT_CALL( callback, Call( _ ) )
                    .Times( userIndex == userCodeCc->getUserIdsCache().getUserIds().size() && slotId == 3 );

                EXPECT_TRUE(
                    node->getRequestsManager().isWaitingForRespond( YearDaySchduleRequest( 1, 0, userId, slotId ) ) );
                node->getRequestsManager().onRespond( YearDaySchduleRequest( 1, 0, userId, slotId ),
                                                      znet_year_day_schedule_t{} );
            }
        }
    }
}

TEST_F( ZCcScheduleEntryLockTestSuite, requestValue_withoutUserCodesCache ) {
    getNodesProvider().nodeCreate( 1 );
    auto node = getNodesProvider().onFindNode( 1 );

    node->addCommandClass( 0, ZNET_COMMAND_CLASS_SCHEDULE_ENTRY_LOCK, 1 );
    node->addCommandClass( 0, ZNET_COMMAND_CLASS_USER_CODE, 1 );
    auto scheduleEntryLockCc = node->getCommandClass<ZCc0x4EScheduleEntryLock>( 0 );
    auto userCodeCc = node->getCommandClass<ZCc0x63UserCode>( 0 );

    // cc interviews
    {
        scheduleEntryLockCc->interview( {}, {} );

        znet_cmd_schedule_entry_lock_supported_report_t supportedSlotsReport{};
        node->getRequestsManager().onRespond( ScheduleEntryLockSupportedRequest( 1, 0 ), supportedSlotsReport );

        userCodeCc->interview( {}, {} );
        node->getRequestsManager().onRespond( UserCodesNumberRequest( 1, 0 ), 3 );
    }

    // fill user codes cache
    {
        userCodeCc->getUserIdsCache().onUserCodeUpdated( 1, UserCode::UserIDStatus::Available );
        userCodeCc->getUserIdsCache().onUserCodeUpdated( 2, UserCode::UserIDStatus::Enabled );
        userCodeCc->getUserIdsCache().onUserCodeUpdated( 3, UserCode::UserIDStatus::Enabled );
    }

    // request schecdule entry lock values
    {
        MockFunction<void( RequestFailTypeOpt )> callback;
        EXPECT_CALL( callback, Call( _ ) ).Times( 0 );

        EXPECT_FALSE( scheduleEntryLockCc->requestValue( createRequestValue( callback ) ) );
    }
}

TEST_F( ZCcScheduleEntryLockTestSuite, requestValue_withoutAnySlots ) {
    getNodesProvider().nodeCreate( 1 );
    auto node = getNodesProvider().onFindNode( 1 );

    node->addCommandClass( 0, ZNET_COMMAND_CLASS_SCHEDULE_ENTRY_LOCK, 1 );
    node->addCommandClass( 0, ZNET_COMMAND_CLASS_USER_CODE, 1 );
    auto scheduleEntryLockCc = node->getCommandClass<ZCc0x4EScheduleEntryLock>( 0 );
    auto userCodeCc = node->getCommandClass<ZCc0x63UserCode>( 0 );

    // cc interviews
    {
        scheduleEntryLockCc->interview( {}, {} );

        znet_cmd_schedule_entry_lock_supported_report_t supportedSlotsReport{};
        supportedSlotsReport.year_day_slots = 3;
        node->getRequestsManager().onRespond( ScheduleEntryLockSupportedRequest( 1, 0 ), supportedSlotsReport );

        userCodeCc->interview( {}, {} );
        node->getRequestsManager().onRespond( UserCodesNumberRequest( 1, 0 ), 3 );
    }

    // request schecdule entry lock values
    {
        MockFunction<void( RequestFailTypeOpt )> callback;
        EXPECT_CALL( callback, Call( _ ) ).Times( 0 );

        EXPECT_FALSE( scheduleEntryLockCc->requestValue( createRequestValue( callback ) ) );
    }
}

TEST_F( ZCcScheduleEntryLockTestSuite, requestValue_fail ) {
    getNodesProvider().nodeCreate( 1 );
    auto node = getNodesProvider().onFindNode( 1 );

    node->addCommandClass( 0, ZNET_COMMAND_CLASS_SCHEDULE_ENTRY_LOCK, 1 );
    node->addCommandClass( 0, ZNET_COMMAND_CLASS_USER_CODE, 1 );
    auto scheduleEntryLockCc = node->getCommandClass<ZCc0x4EScheduleEntryLock>( 0 );
    auto userCodeCc = node->getCommandClass<ZCc0x63UserCode>( 0 );

    // cc interviews
    {
        scheduleEntryLockCc->interview( {}, {} );

        znet_cmd_schedule_entry_lock_supported_report_t supportedSlotsReport{};
        supportedSlotsReport.week_day_slots = 2;
        supportedSlotsReport.year_day_slots = 3;
        supportedSlotsReport.daily_repeating_slots = 4;
        node->getRequestsManager().onRespond( ScheduleEntryLockSupportedRequest( 1, 0 ), supportedSlotsReport );

        userCodeCc->interview( {}, {} );
        node->getRequestsManager().onRespond( UserCodesNumberRequest( 1, 0 ), 3 );
    }

    // fill user codes cache
    {
        userCodeCc->getUserIdsCache().onUserCodeUpdated( 1, UserCode::UserIDStatus::Available );
        userCodeCc->getUserIdsCache().onUserCodeUpdated( 2, UserCode::UserIDStatus::Enabled );
        userCodeCc->getUserIdsCache().onUserCodeUpdated( 3, UserCode::UserIDStatus::Enabled );
    }

    std::vector<uint8_t> userIds;
    userIds.assign( userCodeCc->getUserIdsCache().getUserIds().begin(),
                    userCodeCc->getUserIdsCache().getUserIds().end() );
    std::reverse( userIds.begin(), userIds.end() );

    // YearDaySchduleRequest reported fail
    {
        MockFunction<void( RequestFailTypeOpt )> callback;
        EXPECT_CALL( callback, Call( _ ) ).Times( 0 );

        EXPECT_TRUE( scheduleEntryLockCc->requestValue( createRequestValue( callback ) ) );

        for( uint8_t slotId = 1; slotId <= 2; ++slotId ) {
            EXPECT_TRUE( node->getRequestsManager().isWaitingForRespond(
                WeekDayScheduleRequest( 1, 0, userIds.front(), slotId ) ) );
            node->getRequestsManager().onRespond( WeekDayScheduleRequest( 1, 0, userIds.front(), slotId ),
                                                  znet_week_day_schedule_t{} );
        }

        EXPECT_CALL( callback, Call( RequestFailTypeOpt( RequestFailType::ReportedFail ) ) ).Times( 1 );
        node->getRequestsManager().onFail( YearDaySchduleRequest( 1, 0, userIds.front(), 1 ) );
    }

    // YearDaySchduleRequest sending fail
    {
        MockFunction<void( RequestFailTypeOpt )> callback;
        EXPECT_CALL( callback, Call( _ ) ).Times( 0 );

        getRequestsManager().addFailedRequest( YearDaySchduleRequest( 1, 0, userIds.front(), 2 ) );

        EXPECT_TRUE( scheduleEntryLockCc->requestValue( createRequestValue( callback ) ) );

        for( uint8_t slotId = 1; slotId <= 2; ++slotId ) {
            EXPECT_TRUE( node->getRequestsManager().isWaitingForRespond(
                WeekDayScheduleRequest( 1, 0, userIds.front(), slotId ) ) );
            node->getRequestsManager().onRespond( WeekDayScheduleRequest( 1, 0, userIds.front(), slotId ),
                                                  znet_week_day_schedule_t{} );
        }

        EXPECT_CALL( callback, Call( RequestFailTypeOpt( RequestFailType::SendingError ) ) ).Times( 1 );

        EXPECT_TRUE(
            node->getRequestsManager().isWaitingForRespond( YearDaySchduleRequest( 1, 0, userIds.front(), 1 ) ) );
        node->getRequestsManager().onRespond( YearDaySchduleRequest( 1, 0, userIds.front(), 1 ),
                                              znet_year_day_schedule_t{} );
        getTimerProvider().addTime( DefaultPostponedRequestDelay );

        EXPECT_FALSE(
            node->getRequestsManager().isWaitingForRespond( YearDaySchduleRequest( 1, 0, userIds.front(), 2 ) ) );
    }

    // first sending fail
    {
        MockFunction<void( RequestFailTypeOpt )> callback;
        EXPECT_CALL( callback, Call( _ ) ).Times( 0 );

        getRequestsManager().addFailedRequest( WeekDayScheduleRequest( 1, 0, userIds.front(), 1 ) );

        EXPECT_TRUE( scheduleEntryLockCc->requestValue( createRequestValue( callback ) ) );

        EXPECT_CALL( callback, Call( RequestFailTypeOpt( RequestFailType::SendingError ) ) ).Times( 1 );
        getTimerProvider().addTime( DefaultPostponedRequestDelay );
    }
}

}  // namespace ut
}  // namespace znet
