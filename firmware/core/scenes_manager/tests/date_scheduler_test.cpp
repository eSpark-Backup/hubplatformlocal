#include "scenes_manager/scenes_engine/date_scheduler/date_scheduler.h"
#include "scenes_manager/scenes_engine/events/date_event.h"
#include "scenes_manager/scenes_engine/events/daily_event.h"
#include "scenes_manager/scenes_engine/events/weekly_event.h"
#include "scenes_manager/scenes_engine/events/monthly_event.h"
#include "scenes_manager/scenes_engine/events/once_event.h"
#include "scenes_manager/scenes_engine/events/sun_state_event.h"
#include "scenes_manager/scenes_engine/events/interval_event.h"

#include "base_utils/gregorian_calendar.h"
#include "base_utils/timeutils.h"
#include "common_utils/cpp14.h"
#include "json_utils/json_wrapper.h"
#include "log_utils/ezlo_log.h"

#include <gtest/gtest.h>

/*

 TEST PLAN

DONE    1. Date event tests
DONE        1.1 daily event
DONE        1.2 weekly event
DONE        1.3 monthly event
DONE    2. Date scheduler tests
DONE        2.1 default state
DONE        2.2 singe daily event activated
DONE        2.3 single day weekly events
DONE        2.4 different day weekly events
DONE        2.5 date recalculation on big timeshifts
DONE        2.6 events are not activated by default
DONE        2.7 event activated once regardless of passed time
DONE        2.8 once event
DONE        2.9 event not removed on activation
DONE    3. Date events with same activation time
DONE        3.1 isDate events
DONE        3.2 isOnce events
DONE        3.3 isDate and isOnce
DONE        3.4 isSunState events
DONE        3.5 isInterval events

 */

namespace SceneEngine {
namespace UT {

const std::set< DateEvent::TimeType > timeArray{ { 10, 30 }, { 12, 01 }, { 03, 00 } };
const std::set< DateEventBase::Day > daysArray{ 1, 23, 17, 31 };
const std::set< DateEventBase::Day > weekdaysArray{ 5, 4, 1, 3 };
const std::set< DateEventBase::Week > weeksArray{ 5, 4, 1, 3, -1 };

const GregorianCalendar::ClockDuration defaultStartTime{ 1738301208000000000ll };  // 2025-01-31T05:26:48+0000

class DateEventSuite : public testing::Test {};

class DateSchedulerSuite : public testing::Test {
public:
    void SetUp() override {
        m_dateScheduler.setTime( defaultStartTime );
        m_dateScheduler.enableRecalculationOnBigTimeshift( false );
    }
    DateScheduler& dateScheduler() {
        return m_dateScheduler;
    }

    static std::string makeError( int line ) {
        return std::string{ __FILE__ }.append( ":" ).append( std::to_string( line ) );
    }
    void checkActivatedEvents( const std::vector< DateEventPtr >& expectedActivation, int line ) {
        auto activatedEvents = dateScheduler().collectActivatedEvents();
        EXPECT_EQ( activatedEvents, expectedActivation ) << makeError( line );
    }

private:
    DateScheduler m_dateScheduler;
};

TEST_F( DateEventSuite, dailyEvent ) {  // 1.1
    using namespace GregorianCalendar;

    Date date{ TimePoint( defaultStartTime ) };
    DailyEvent dailyEvent( timeArray );

    auto nextDate = dailyEvent.getNextDate( date.toTimePoint() );
    EXPECT_TRUE( nextDate );
    date = Date( TimePoint( *nextDate ) );
    EXPECT_EQ( date.getYear(), 2025 );
    EXPECT_EQ( date.getMonth(), 1 );
    EXPECT_EQ( date.getDay(), 31 );
    EXPECT_EQ( date.getHour(), 10 );
    EXPECT_EQ( date.getMinute(), 30 );
    EXPECT_EQ( date.getSecond(), 0 );
    EXPECT_EQ( date.getMillisecond(), 0 );

    nextDate = dailyEvent.getNextDate( date.toTimePoint() );
    EXPECT_TRUE( nextDate );
    date = TimePoint( *nextDate );
    EXPECT_EQ( date.getYear(), 2025 );
    EXPECT_EQ( date.getMonth(), 1 );
    EXPECT_EQ( date.getDay(), 31 );
    EXPECT_EQ( date.getHour(), 12 );
    EXPECT_EQ( date.getMinute(), 1 );
    EXPECT_EQ( date.getSecond(), 0 );
    EXPECT_EQ( date.getMillisecond(), 0 );

    nextDate = dailyEvent.getNextDate( date.toTimePoint() );
    EXPECT_TRUE( nextDate );
    date = TimePoint( *nextDate );
    EXPECT_EQ( date.getYear(), 2025 );
    EXPECT_EQ( date.getMonth(), 2 );
    EXPECT_EQ( date.getDay(), 1 );
    EXPECT_EQ( date.getHour(), 3 );
    EXPECT_EQ( date.getMinute(), 0 );
    EXPECT_EQ( date.getSecond(), 0 );
    EXPECT_EQ( date.getMillisecond(), 0 );
}

TEST_F( DateEventSuite, weeklyEvent ) {  // 1.2
    using namespace GregorianCalendar;

    Date date{ TimePoint( defaultStartTime ) };
    WeeklyEvent weeklyEvent( timeArray, weekdaysArray );

    auto nextDate = weeklyEvent.getNextDate( date.toTimePoint() );
    date = Date( TimePoint( *nextDate ) );
    EXPECT_TRUE( nextDate );
    EXPECT_EQ( date.getYear(), 2025 );
    EXPECT_EQ( date.getMonth(), 1 );
    EXPECT_EQ( date.getDay(), 31 );
    EXPECT_EQ( date.getWeekday(), 5 );
    EXPECT_EQ( date.getHour(), 10 );
    EXPECT_EQ( date.getMinute(), 30 );
    EXPECT_EQ( date.getSecond(), 0 );
    EXPECT_EQ( date.getMillisecond(), 0 );

    nextDate = weeklyEvent.getNextDate( date.toTimePoint() );
    date = Date( TimePoint( *nextDate ) );
    EXPECT_TRUE( nextDate );
    EXPECT_EQ( date.getYear(), 2025 );
    EXPECT_EQ( date.getMonth(), 1 );
    EXPECT_EQ( date.getDay(), 31 );
    EXPECT_EQ( date.getWeekday(), 5 );
    EXPECT_EQ( date.getHour(), 12 );
    EXPECT_EQ( date.getMinute(), 1 );
    EXPECT_EQ( date.getSecond(), 0 );
    EXPECT_EQ( date.getMillisecond(), 0 );

    nextDate = weeklyEvent.getNextDate( date.toTimePoint() );
    date = Date( TimePoint( *nextDate ) );
    EXPECT_TRUE( nextDate );
    EXPECT_EQ( date.getYear(), 2025 );
    EXPECT_EQ( date.getMonth(), 2 );
    EXPECT_EQ( date.getWeekday(), 1 );
    EXPECT_EQ( date.getHour(), 3 );
    EXPECT_EQ( date.getMinute(), 0 );
    EXPECT_EQ( date.getSecond(), 0 );
    EXPECT_EQ( date.getMillisecond(), 0 );

    nextDate = weeklyEvent.getNextDate( date.toTimePoint() );
    date = Date( TimePoint( *nextDate ) );
    EXPECT_TRUE( nextDate );
    EXPECT_EQ( date.getYear(), 2025 );
    EXPECT_EQ( date.getMonth(), 2 );
    EXPECT_EQ( date.getWeekday(), 1 );
    EXPECT_EQ( date.getHour(), 10 );
    EXPECT_EQ( date.getMinute(), 30 );
    EXPECT_EQ( date.getSecond(), 0 );
    EXPECT_EQ( date.getMillisecond(), 0 );

    nextDate = weeklyEvent.getNextDate( TimePoint( date.toClockDuration() ) );
    date = Date( TimePoint( *nextDate ) );
    EXPECT_TRUE( nextDate );
    EXPECT_EQ( date.getYear(), 2025 );
    EXPECT_EQ( date.getMonth(), 2 );
    EXPECT_EQ( date.getWeekday(), 1 );
    EXPECT_EQ( date.getHour(), 12 );
    EXPECT_EQ( date.getMinute(), 1 );
    EXPECT_EQ( date.getSecond(), 0 );
    EXPECT_EQ( date.getMillisecond(), 0 );

    nextDate = weeklyEvent.getNextDate( date.toTimePoint() );
    date = Date( TimePoint( *nextDate ) );
    EXPECT_TRUE( nextDate );
    EXPECT_EQ( date.getYear(), 2025 );
    EXPECT_EQ( date.getMonth(), 2 );
    EXPECT_EQ( date.getWeekday(), 3 );
    EXPECT_EQ( date.getHour(), 3 );
    EXPECT_EQ( date.getMinute(), 0 );
    EXPECT_EQ( date.getSecond(), 0 );
    EXPECT_EQ( date.getMillisecond(), 0 );
}

TEST_F( DateEventSuite, monthlyEvent ) {  // 1.3
    using namespace GregorianCalendar;

    Date date{ TimePoint( defaultStartTime ) };
    MonthlyEvent monthlyEvent( timeArray, daysArray );

    auto nextDate = monthlyEvent.getNextDate( date.toTimePoint() );
    date = Date( TimePoint( *nextDate ) );
    EXPECT_TRUE( nextDate );
    EXPECT_EQ( date.getYear(), 2025 );
    EXPECT_EQ( date.getMonth(), 1 );
    EXPECT_EQ( date.getDay(), 31 );
    EXPECT_EQ( date.getHour(), 10 );
    EXPECT_EQ( date.getMinute(), 30 );
    EXPECT_EQ( date.getSecond(), 0 );
    EXPECT_EQ( date.getMillisecond(), 0 );

    nextDate = monthlyEvent.getNextDate( date.toTimePoint() );
    date = TimePoint( *nextDate );
    EXPECT_TRUE( nextDate );
    EXPECT_EQ( date.getYear(), 2025 );
    EXPECT_EQ( date.getMonth(), 1 );
    EXPECT_EQ( date.getDay(), 31 );
    EXPECT_EQ( date.getHour(), 12 );
    EXPECT_EQ( date.getMinute(), 1 );
    EXPECT_EQ( date.getSecond(), 0 );
    EXPECT_EQ( date.getMillisecond(), 0 );

    nextDate = monthlyEvent.getNextDate( date.toTimePoint() );
    date = TimePoint( *nextDate );
    EXPECT_TRUE( nextDate );
    EXPECT_EQ( date.getYear(), 2025 );
    EXPECT_EQ( date.getMonth(), 2 );
    EXPECT_EQ( date.getDay(), 1 );
    EXPECT_EQ( date.getHour(), 3 );
    EXPECT_EQ( date.getMinute(), 0 );
    EXPECT_EQ( date.getSecond(), 0 );
    EXPECT_EQ( date.getMillisecond(), 0 );

    nextDate = monthlyEvent.getNextDate( date.toTimePoint() );
    date = TimePoint( *nextDate );
    EXPECT_TRUE( nextDate );
    EXPECT_EQ( date.getYear(), 2025 );
    EXPECT_EQ( date.getMonth(), 2 );
    EXPECT_EQ( date.getDay(), 1 );
    EXPECT_EQ( date.getHour(), 10 );
    EXPECT_EQ( date.getMinute(), 30 );
    EXPECT_EQ( date.getSecond(), 0 );
    EXPECT_EQ( date.getMillisecond(), 0 );

    nextDate = monthlyEvent.getNextDate( date.toTimePoint() );
    date = TimePoint( *nextDate );
    EXPECT_TRUE( nextDate );
    EXPECT_EQ( date.getYear(), 2025 );
    EXPECT_EQ( date.getMonth(), 2 );
    EXPECT_EQ( date.getDay(), 1 );
    EXPECT_EQ( date.getHour(), 12 );
    EXPECT_EQ( date.getMinute(), 1 );
    EXPECT_EQ( date.getSecond(), 0 );
    EXPECT_EQ( date.getMillisecond(), 0 );

    nextDate = monthlyEvent.getNextDate( date.toTimePoint() );
    date = TimePoint( *nextDate );
    EXPECT_TRUE( nextDate );
    EXPECT_EQ( date.getYear(), 2025 );
    EXPECT_EQ( date.getMonth(), 2 );
    EXPECT_EQ( date.getDay(), 17 );
    EXPECT_EQ( date.getHour(), 3 );
    EXPECT_EQ( date.getMinute(), 0 );
    EXPECT_EQ( date.getSecond(), 0 );
    EXPECT_EQ( date.getMillisecond(), 0 );
}

TEST_F( DateSchedulerSuite, defaultState ) {  // 2.1
    EXPECT_TRUE( dateScheduler().empty() );
    EXPECT_TRUE( dateScheduler().collectActivatedEvents().empty() );

    auto now = DateScheduler::ClockDuration::max();
    dateScheduler().setTime( now );
    EXPECT_TRUE( dateScheduler().collectActivatedEvents().empty() );
}

TEST_F( DateSchedulerSuite, singeDailyEventActivated ) {  // 2.2
    auto now = defaultStartTime;
    dateScheduler().setTime( now );

    auto testEvent = std::make_shared< DailyEvent >( timeArray );
    dateScheduler().push( testEvent );

    for( int i = 0; i < timeArray.size(); ++i ) {
        now = *testEvent->getNextDate( GregorianCalendar::TimePoint( now ) );
        dateScheduler().setTime( now );
        checkActivatedEvents( { testEvent }, __LINE__ );
    }
}

TEST_F( DateSchedulerSuite, singleDayWeeklyEvents ) {  // 2.3
    auto now = defaultStartTime;
    dateScheduler().setTime( now );

    const Json weekdays{ 2 };
    auto weeklyEvent1 = std::make_shared< WeeklyEvent >( WeeklyEvent::TimeList{ { 10, 00 }, { 12, 00 } }, weekdays );
    auto weeklyEvent2 = std::make_shared< WeeklyEvent >( WeeklyEvent::TimeList{ { 11, 00 }, { 13, 00 } }, weekdays );
    dateScheduler().push( weeklyEvent1 );
    dateScheduler().push( weeklyEvent2 );

    // all activations during a week
    now = *weeklyEvent1->getNextDate( GregorianCalendar::TimePoint( now ) );
    dateScheduler().setTime( now );
    checkActivatedEvents( { weeklyEvent1 }, __LINE__ );

    now = *weeklyEvent2->getNextDate( GregorianCalendar::TimePoint( now ) );
    dateScheduler().setTime( now );
    checkActivatedEvents( { weeklyEvent2 }, __LINE__ );

    now = *weeklyEvent2->getNextDate( GregorianCalendar::TimePoint( now ) );
    dateScheduler().setTime( now );
    checkActivatedEvents( { weeklyEvent1, weeklyEvent2 }, __LINE__ );

    // activation on next week
    now = *weeklyEvent1->getNextDate( GregorianCalendar::TimePoint( now ) );
    dateScheduler().setTime( now );
    checkActivatedEvents( { weeklyEvent1 }, __LINE__ );
}

TEST_F( DateSchedulerSuite, differentDayWeeklyEvents ) {  // 2.4
    using Day = WeeklyEvent::Day;

    auto now = defaultStartTime;
    dateScheduler().setTime( now );

    auto eventActivationTime1 = WeeklyEvent::TimeList{ { 10, 00 }, { 12, 00 } };
    auto eventActivationTime2 = WeeklyEvent::TimeList{ { 11, 00 }, { 11, 00 } };
    auto weeklyEvent1 = std::make_shared< WeeklyEvent >( std::move( eventActivationTime1 ), std::set< Day >{ 1 } );
    auto weeklyEvent2 = std::make_shared< WeeklyEvent >( std::move( eventActivationTime2 ), std::set< Day >{ 3 } );
    dateScheduler().push( weeklyEvent1 );
    dateScheduler().push( weeklyEvent2 );

    // all activations during a week
    for( int i = 0; i < eventActivationTime1.size(); ++i ) {
        now = *weeklyEvent1->getNextDate( GregorianCalendar::TimePoint( now ) );
        dateScheduler().setTime( now );
        checkActivatedEvents( { weeklyEvent1 }, __LINE__ );
    }
    for( int i = 0; i < eventActivationTime2.size(); ++i ) {
        now = *weeklyEvent2->getNextDate( GregorianCalendar::TimePoint( now ) );
        dateScheduler().setTime( now );
        checkActivatedEvents( { weeklyEvent2 }, __LINE__ );
    }

    // activation on next week
    now = *weeklyEvent1->getNextDate( GregorianCalendar::TimePoint( now ) );
    dateScheduler().setTime( now );
    checkActivatedEvents( { weeklyEvent1 }, __LINE__ );
}

TEST_F( DateSchedulerSuite, dateRecalculationOnBigTimeshifts ) {  // 2.5
    auto now = defaultStartTime;
    dateScheduler().setTime( now );
    dateScheduler().enableRecalculationOnBigTimeshift( true );

    auto testEvent = std::make_shared< DailyEvent >( timeArray );
    dateScheduler().push( testEvent );

    // event not activated due to big timeshift
    now = *testEvent->getNextDate( GregorianCalendar::TimePoint( now ) );
    dateScheduler().setTime( now );
    checkActivatedEvents( {}, __LINE__ );

    // event is activated in regular flow
    dateScheduler().enableRecalculationOnBigTimeshift( false );
    now = *testEvent->getNextDate( GregorianCalendar::TimePoint( now ) );
    dateScheduler().setTime( now );
    checkActivatedEvents( { testEvent }, __LINE__ );
}

TEST_F( DateSchedulerSuite, eventsAreNotActivatedByDefault ) {  // 2.6
    auto now = defaultStartTime;
    dateScheduler().setTime( now );

    auto testEvent = std::make_shared< DailyEvent >( timeArray );
    dateScheduler().push( testEvent );
    EXPECT_TRUE( dateScheduler().collectActivatedEvents().empty() );
}

TEST_F( DateSchedulerSuite, eventActivatedOnceRegardlessOfPassedTime ) {  // 2.7
    auto now = defaultStartTime;
    dateScheduler().setTime( defaultStartTime );

    auto testEvent = std::make_shared< DailyEvent >( timeArray );
    dateScheduler().push( testEvent );

    now = *testEvent->getNextDate( GregorianCalendar::TimePoint( now ) );
    now = *testEvent->getNextDate( GregorianCalendar::TimePoint( now ) );
    dateScheduler().setTime( now );
    checkActivatedEvents( { testEvent }, __LINE__ );
}

TEST_F( DateSchedulerSuite, onceEvent ) {  // 2.8
    auto now = GregorianCalendar::Date( "2025-01-31T05:26:48" ).toClockDuration();
    dateScheduler().setTime( now );

    GregorianCalendar::Date date1;
    GregorianCalendar::Date date2;
    ASSERT_TRUE( date1.setDate( 2026, 5, 15 ) && date1.setTime( 12, 10, 0 ) );
    ASSERT_TRUE( date2.setDate( 2026, 8, 17 ) && date2.setTime( 20, 15, 0 ) );

    auto onceEvent1 = std::make_shared< OnceEvent >( date1 );
    auto onceEvent2 = std::make_shared< OnceEvent >( date2 );

    dateScheduler().push( onceEvent1 );
    dateScheduler().push( onceEvent2 );

    now = *onceEvent1->getNextDate( GregorianCalendar::TimePoint( now ) );
    dateScheduler().setTime( now );
    checkActivatedEvents( { onceEvent1 }, __LINE__ );

    now = *onceEvent2->getNextDate( GregorianCalendar::TimePoint( now ) );
    dateScheduler().setTime( now );
    checkActivatedEvents( { onceEvent2 }, __LINE__ );
}

TEST_F( DateSchedulerSuite, eventNotRemovedOnActivation ) {  // 2.9
    dateScheduler().setTime( defaultStartTime );

    auto testEvent = std::make_shared< DailyEvent >( timeArray );
    dateScheduler().push( testEvent );

    auto nextDate = *testEvent->getNextDate( GregorianCalendar::TimePoint( defaultStartTime ) );
    dateScheduler().setTime( nextDate );
    EXPECT_EQ( dateScheduler().collectActivatedEvents().size(), 1 );
    EXPECT_FALSE( dateScheduler().empty() );
}

TEST_F( DateSchedulerSuite, isDateEventsWithSameActivationTime ) {  // 3.1
    dateScheduler().setTime( defaultStartTime );

    const auto timeList = DailyEvent::TimeList{ { 10, 30 } };
    auto event1 = std::make_shared< DailyEvent >( timeList );
    auto event2 = std::make_shared< DailyEvent >( timeList );
    dateScheduler().push( event1 );
    dateScheduler().push( event2 );

    auto nextDate = *event1->getNextDate( GregorianCalendar::TimePoint( defaultStartTime ) );
    dateScheduler().setTime( nextDate );
    checkActivatedEvents( { event1, event2 }, __LINE__ );
}

TEST_F( DateSchedulerSuite, isOnceEventsWithSameActivationTime ) {  // 3.2
    dateScheduler().setTime( defaultStartTime );

    GregorianCalendar::Date date;
    ASSERT_TRUE( date.setDate( 2026, 5, 15 ) && date.setTime( 12, 10, 0 ) );

    auto event1 = std::make_shared< OnceEvent >( date );
    auto event2 = std::make_shared< OnceEvent >( date );

    dateScheduler().push( event1 );
    dateScheduler().push( event2 );

    auto nextDate = *event1->getNextDate( GregorianCalendar::TimePoint( defaultStartTime ) );
    dateScheduler().setTime( nextDate );
    checkActivatedEvents( { event1, event2 }, __LINE__ );
}

TEST_F( DateSchedulerSuite, dailyAndOnceEventsWithSameActivationTime ) {  // 3.3
    using namespace GregorianCalendar;

    dateScheduler().setTime( defaultStartTime );

    auto dailyEvent = std::make_shared< DailyEvent >( DailyEvent::TimeList{ { 10, 30 } } );

    Date date( TimePoint{ defaultStartTime } );
    ASSERT_TRUE( date.setTime( 10, 30, 0 ) );
    auto onceEvent = std::make_shared< OnceEvent >( date );

    dateScheduler().push( dailyEvent );
    dateScheduler().push( onceEvent );

    auto nextDate = *dailyEvent->getNextDate( GregorianCalendar::TimePoint( defaultStartTime ) );
    dateScheduler().setTime( nextDate );
    checkActivatedEvents( { dailyEvent, onceEvent }, __LINE__ );
}

TEST_F( DateSchedulerSuite, isSunStateEventsWithSameActivationTime ) {  // 3.4
    dateScheduler().setTime( defaultStartTime );

    auto stateType = SunStateEvent::State::SUN_SET;
    auto offsetType = SunStateEvent::OffsetType::IN_TIME;
    auto timeZoneOffset = std::chrono::hours( 0 );
    auto geoPositionGetter = []() { return HubPlatform::CoreDRemoteInterface::GeoPosition{ 0.0f, 0.0f }; };

    auto event1 = std::make_shared< SunStateEvent >( stateType, offsetType, timeZoneOffset, geoPositionGetter );
    auto event2 = std::make_shared< SunStateEvent >( stateType, offsetType, timeZoneOffset, geoPositionGetter );
    ASSERT_FALSE( event1->isRangeEvent() );
    ASSERT_FALSE( event2->isRangeEvent() );

    dateScheduler().push( std::static_pointer_cast< DateEventBase >( event1 ) );
    dateScheduler().push( std::static_pointer_cast< DateEventBase >( event2 ) );

    auto nextDate = event1->getNextDate( GregorianCalendar::TimePoint( defaultStartTime ) );
    ASSERT_NE( nextDate, nullptr );

    dateScheduler().setTime( *nextDate );
    checkActivatedEvents( { event1, event2 }, __LINE__ );
}

TEST_F( DateSchedulerSuite, isIntervalEventsWithSameActivationTime ) {  // 3.5
    dateScheduler().setTime( defaultStartTime );

    const Json intervalSettings( "2s" );
    auto event1 = std::make_shared< IntervalEvent >( intervalSettings );
    auto event2 = std::make_shared< IntervalEvent >( intervalSettings );
    dateScheduler().push( event1 );
    dateScheduler().push( event2 );

    auto nextDate = *event1->getNextDate( GregorianCalendar::TimePoint( defaultStartTime ) );
    dateScheduler().setTime( nextDate );
    checkActivatedEvents( { event1, event2 }, __LINE__ );
}

}  // namespace UT
}  // namespace SceneEngine
