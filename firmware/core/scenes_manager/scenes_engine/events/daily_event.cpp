// Extracted from date_event.h/cpp
#include "scenes_manager/scenes_engine/events/daily_event.h"
#include "log_utils/ezlo_log.h"

namespace SceneEngine {

DailyEvent::DailyEvent( TimeList timeList )
: m_timeList( std::move( timeList ) ) {}

DateEvent::OptionalClock DailyEvent::getNextDate( const GregorianCalendar::TimePoint& now ) const {
    GregorianCalendar::Date date( now );
    for( int i = 0; i <= 1; ++i, date.incrementDay() ) {
        if( auto clock = findNextTime( now.time_since_epoch(), date, m_timeList ) ) {
            return clock;
        }
    }
    LOGGER_ERROR( "next date not found" );
    return nullptr;
}

}  // namespace SceneEngine