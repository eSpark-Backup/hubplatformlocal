// Extracted from date_event.h/cpp
#include "scenes_manager/scenes_engine/events/weekly_event.h"
#include "log_utils/ezlo_log.h"

namespace SceneEngine {

WeeklyEvent::WeeklyEvent( TimeList timeList, std::set< Day > weekdays )
: m_timeList( std::move( timeList ) )
, m_weekdayList( std::move( weekdays ) ) {}

DateEvent::OptionalClock WeeklyEvent::getNextDate( const GregorianCalendar::TimePoint& now ) const {
    GregorianCalendar::Date date( now );
    for( int i = 0; i <= 1; ++i, date.incrementWeek() ) {
        for( const auto& weekday : m_weekdayList ) {
            date.moveToWeekday( weekday );
            if( auto clock = findNextTime( now.time_since_epoch(), date, m_timeList ) ) {
                return clock;
            }
        }
    }
    LOGGER_ERROR( "next date not found" );
    return nullptr;
}

}  // namespace SceneEngine