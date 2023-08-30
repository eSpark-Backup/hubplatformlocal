#include "scenes_manager/scenes_engine/events/sun_state_event.h"
#include "scenes_manager/scenes_engine/environment.h"
#include "scenes_manager/scenes_service.h"
#include "scenes_manager/scenes_engine/exceptions.h"

#include "scenes_manager/tests/resources/sun_state_reference.h"

#include "service_interfaces/bus_wrapper.h"
#include "bus/base_bus.h"
#include "bus/fake_bus.h"
#include "database_client/fake_db/fake_db_client.h"
#include "base_utils/timeutils.h"

#include "json_utils/json_utils.h"
#include "common_utils/cpp14.h"

#include "test_utils/temporary_root_folder_wrapper.h"

#include <ctime>
#include <sstream>
#include <gtest/gtest.h>
#include <gmock/gmock.h>

namespace SceneEngine {
namespace UT {

using namespace HubPlatform;

class SunStateEventTest : public testing::Test {
public:
    std::string getSunriseDate( const TestData& data ) {
        std::stringstream ss;
        ss << data.date << "T" << data.sunrise;
        return ss.str();
    }

    std::string getSunsetDate( const TestData& data ) {
        std::stringstream ss;
        ss << data.date << "T" << data.sunset;
        if( ss.str() == "2019-07-15T24:12:00" ) {
            return "2019-07-16T00:12:00";
        }
        return ss.str();
    }

    std::string getMidnightDate( const TestData& data ) {
        std::stringstream ss;
        ss << data.date << "T00:00:00";
        return ss.str();
    }

    std::size_t getTimeInterval( struct tm time ) {
        return time.tm_hour * 60 * 60 + time.tm_min * 60 + time.tm_sec;
    }

    void setSunState( const SunStateEvent::State& state ) {
        m_sunState = state;
    }

    void setDays( SunStateEvent::Days&& days ) {
        m_days = std::move( days );
    }

    void setWeekdays( SunStateEvent::Weekdays&& weekdays ) {
        m_weekdays = std::move( weekdays );
    }

    void copySunstateDate( const GregorianCalendar::Date& src, TestData& dst, SunStateEvent::State state ) {
        dst.date.year = src.getYear();
        dst.date.month = src.getMonth();
        dst.date.day = src.getDay();
        if( state == SunStateEvent::State::SUN_RISE ) {
            dst.sunrise.hours = src.getHour();
            dst.sunrise.minutes = src.getMinute();
            dst.sunrise.seconds = src.getSecond();
        }
        else {
            dst.sunset.hours = src.getHour();
            dst.sunset.minutes = src.getMinute();
            dst.sunset.seconds = src.getSecond();
        }
    }

    void addTimeOffset( TestData& data,
                        SunStateEvent::State state,
                        SunStateEvent::OffsetType offsetType,
                        const std::string& timeOffset ) {
        if( offsetType == SunStateEvent::OffsetType::IN_TIME ) {
            return;
        }
        struct tm timeOffsetStruct = TimeUtils::stringToTimeStruct( timeOffset, "%H:%M:%S" );
        std::string sunstateDate =
            ( state == SunStateEvent::State::SUN_RISE ) ? getSunriseDate( data ) : getSunsetDate( data );
        GregorianCalendar::Date date( sunstateDate );
        if( offsetType == SunStateEvent::OffsetType::AFTER ) {
            date.addSeconds( timeOffsetStruct.tm_sec );
            date.addMinutes( timeOffsetStruct.tm_min );
            date.addHours( timeOffsetStruct.tm_hour );
        }
        else if( offsetType == SunStateEvent::OffsetType::BEFORE ) {
            date.addSeconds( -timeOffsetStruct.tm_sec );
            date.addMinutes( -timeOffsetStruct.tm_min );
            date.addHours( -timeOffsetStruct.tm_hour );
        }
        copySunstateDate( date, data, state );
    }

    void testSunState( SunStateEvent::OffsetType offsetType, const JsonObject& timeOffset, int codeLine ) {
        std::string codeLineStr = "code line: " + std::to_string( codeLine );
        for( TestData data : ReferenceRandomSunstatesList ) {
            GregorianCalendar::Date currentDate( getMidnightDate( data ) );
            const HubPlatform::CoreDRemoteInterface::GeoPosition& geoPosition = data.geoposition;
            SunStateEvent sunstateEvent(
                m_sunState, offsetType, std::chrono::hours( data.timeZoneOffset ), [&geoPosition]() {
                    return geoPosition;
                } );
            if( timeOffset.empty() ) {
                sunstateEvent.setTimeOffset( timeOffset );
                addTimeOffset( data, m_sunState, offsetType, timeOffset );
            }
            SunStateEvent::OptionalClock suntateDate =
                sunstateEvent.getNextDate( GregorianCalendar::TimePoint( currentDate.toClockDuration() ) );
            bool isSunstatePossible =
                ( m_sunState == SunStateEvent::State::SUN_RISE ) ? data.sunrise.hours == NaN : data.sunset.hours == NaN;
            if( isSunstatePossible ) {
                EXPECT_FALSE( suntateDate ) << codeLineStr;
                continue;
            }
            else {
                EXPECT_TRUE( suntateDate ) << codeLineStr;
            }
            auto suntateTimePoint = GregorianCalendar::TimePoint( *suntateDate );
            std::string sunstateDate =
                ( m_sunState == SunStateEvent::State::SUN_RISE ) ? getSunriseDate( data ) : getSunsetDate( data );
            EXPECT_EQ( GregorianCalendar::Date( suntateTimePoint ).toString( "%FT%T" ), sunstateDate ) << codeLineStr;
        }
    }

    void runTestCases( SunStateEvent::State state ) {
        setSunState( state );
        testSunState( SunStateEvent::OffsetType::IN_TIME, "", __LINE__ );
        testSunState( SunStateEvent::OffsetType::IN_TIME, "00:00:20", __LINE__ );
        testSunState( SunStateEvent::OffsetType::AFTER, "12:40:30", __LINE__ );
        testSunState( SunStateEvent::OffsetType::AFTER, "25:40:30", __LINE__ );
        testSunState( SunStateEvent::OffsetType::BEFORE, "12:40:30", __LINE__ );
        testSunState( SunStateEvent::OffsetType::BEFORE, "24:40:30", __LINE__ );
    }

    void testWeeklySunState( SunStateEvent::State state, int codeLine ) {
        std::string codeLineStr = "code line: " + std::to_string( codeLine );
        using Weekday = SunStateEvent::Weekday;
        std::vector< int > weekdays = { (int) Weekday::THU, (int) Weekday::SUN, (int) Weekday::MON };
        SunStateEvent sunstateEvent( state, SunStateEvent::OffsetType::IN_TIME, std::chrono::hours( 3 ), []() {
            return HubPlatform::CoreDRemoteInterface::GeoPosition{ 49.99450, 36.21506 };
        } );
        sunstateEvent.setWeekdays( weekdays );
        std::sort( weekdays.begin(), weekdays.end() );
        GregorianCalendar::Date weekdayIterator( "2019-09-02T00:00:00", "%FT%T" );
        const int repeatingTimes = 4;
        for( int i = 0; i < repeatingTimes; ++i ) {
            for( const int& weekday : weekdays ) {
                weekdayIterator.moveToWeekday( weekday - 1 );
                SunStateEvent::OptionalClock suntateDate =
                    sunstateEvent.getNextDate( GregorianCalendar::TimePoint( weekdayIterator.toClockDuration() ) );
                EXPECT_TRUE( suntateDate ) << codeLineStr;
                if( suntateDate ) {
                    auto suntateTimePoint = GregorianCalendar::Date( GregorianCalendar::TimePoint( *suntateDate ) );
                    const TestData& referenceSunstate = ReferenceWeeklySunstatesList[ suntateTimePoint.getDay() - 1 ];
                    std::string sunstateDate = ( state == SunStateEvent::State::SUN_RISE ) ?
                                                   getSunriseDate( referenceSunstate ) :
                                                   getSunsetDate( referenceSunstate );
                    EXPECT_EQ( suntateTimePoint.toString( "%FT%T" ), sunstateDate ) << codeLineStr;
                }
            }
            weekdayIterator.addWeekdays( 2 );
        }
    }

    void testMonthlySunState( SunStateEvent::State state, int codeLine ) {
        std::string codeLineStr = "code line: " + std::to_string( codeLine );
        SunStateEvent::Days days = { 28, 5, 1, 11, 25, 17 };
        SunStateEvent sunstateEvent( state, SunStateEvent::OffsetType::IN_TIME, std::chrono::hours( 3 ), []() {
            return HubPlatform::CoreDRemoteInterface::GeoPosition{ 49.99450, 36.21506 };
        } );
        sunstateEvent.setDays( days );
        std::sort( days.begin(), days.end() );
        GregorianCalendar::Date dayIterator( "2019-07-31T00:00:00", "%FT%T" );
        const int repeatingTimes = 3;
        int counter = 0;
        for( int i = 0; i < repeatingTimes; ++i ) {
            int oldDay = -1;
            for( const SunStateEvent::Day& day : days ) {
                SunStateEvent::OptionalClock suntateDate =
                    sunstateEvent.getNextDate( GregorianCalendar::TimePoint( dayIterator.toClockDuration() ) );
                dayIterator.addDays( day - oldDay );
                oldDay = day;
                EXPECT_TRUE( suntateDate ) << codeLineStr;
                if( suntateDate ) {
                    auto suntateTimePoint = GregorianCalendar::Date( GregorianCalendar::TimePoint( *suntateDate ) );
                    const TestData& referenceSunstate = ReferenceMonthlySunstatesList[ counter ];
                    std::string sunstateDate = ( state == SunStateEvent::State::SUN_RISE ) ?
                                                   getSunriseDate( referenceSunstate ) :
                                                   getSunsetDate( referenceSunstate );
                    EXPECT_EQ( suntateTimePoint.toString( "%FT%T" ), sunstateDate ) << codeLineStr;
                }
                ++counter;
            }
            dayIterator.setDay( dayIterator.getMaxMonthDay() );
        }
    }

private:
    SunStateEvent::State m_sunState;
    SunStateEvent::Days m_days;
    SunStateEvent::Weekdays m_weekdays;
};

TEST_F( SunStateEventTest, toDuration ) {
    EXPECT_EQ( SunStateEvent::toDuration( "15" ), std::chrono::seconds( 15 ) );
    EXPECT_EQ( SunStateEvent::toDuration( "20:00" ), std::chrono::seconds( 20 * 60 ) );
    EXPECT_EQ( SunStateEvent::toDuration( "00:20:00" ), std::chrono::seconds( 20 * 60 ) );
    EXPECT_EQ( SunStateEvent::toDuration( "00:20:00" ), std::chrono::seconds( 20 * 60 ) );
    EXPECT_EQ( SunStateEvent::toDuration( "12:20:00" ), std::chrono::seconds( 12 * 60 * 60 + 20 * 60 ) );
    EXPECT_EQ( SunStateEvent::toDuration( "01:40:00" ), std::chrono::seconds( 1 * 60 * 60 + 40 * 60 ) );
    EXPECT_EQ( SunStateEvent::toDuration( "00:00:15" ), std::chrono::seconds( 15 ) );
    EXPECT_EQ( SunStateEvent::toDuration( "00:00:60" ), std::chrono::seconds( 60 ) );
    EXPECT_EQ( SunStateEvent::toDuration( "00:00:100" ), std::chrono::seconds( 100 ) );
    EXPECT_EQ( SunStateEvent::toDuration( "00:00:1000" ), std::chrono::seconds( 1000 ) );
    EXPECT_EQ( SunStateEvent::toDuration( "00:40:15" ), std::chrono::seconds( 40 * 60 + 15 ) );
    EXPECT_EQ( SunStateEvent::toDuration( "23:40:15" ), std::chrono::seconds( 23 * 60 * 60 + 40 * 60 + 15 ) );
    EXPECT_EQ( SunStateEvent::toDuration( "15:23:55" ), std::chrono::seconds( 15 * 60 * 60 + 23 * 60 + 55 ) );
    EXPECT_ANY_THROW( SunStateEvent::toDuration( "15:23:55:00" ) );
}

TEST_F( SunStateEventTest, setTimeOffset ) {
    SunStateEvent sunstateEvent(
        SunStateEvent::State::SUN_RISE, SunStateEvent::OffsetType::IN_TIME, std::chrono::hours( 0 ), []() {
            return HubPlatform::CoreDRemoteInterface::GeoPosition{ 1, 1 };
        } );
    EXPECT_NO_THROW( sunstateEvent.setTimeOffset( "00:20:00" ) );
    EXPECT_ANY_THROW( sunstateEvent.setTimeOffset( "15:23:55:00" ) );
    EXPECT_ANY_THROW( sunstateEvent.setTimeOffset( "string" ) );
    EXPECT_ANY_THROW( sunstateEvent.setTimeOffset( 10 ) );
}

TEST_F( SunStateEventTest, DailySunRiseTest ) {
    runTestCases( SunStateEvent::State::SUN_RISE );
}

TEST_F( SunStateEventTest, WeeklySunRiseTest ) {
    testWeeklySunState( SunStateEvent::State::SUN_RISE, __LINE__ );
}

TEST_F( SunStateEventTest, MonthlySunRiseTest ) {
    testMonthlySunState( SunStateEvent::State::SUN_RISE, __LINE__ );
}

TEST_F( SunStateEventTest, DailySunSetTest ) {
    runTestCases( SunStateEvent::State::SUN_SET );
}

TEST_F( SunStateEventTest, WeeklySunSetTest ) {
    testWeeklySunState( SunStateEvent::State::SUN_SET, __LINE__ );
}

TEST_F( SunStateEventTest, MonthlySunSetTest ) {
    testMonthlySunState( SunStateEvent::State::SUN_SET, __LINE__ );
}

}  // namespace UT
}  // namespace SceneEngine
