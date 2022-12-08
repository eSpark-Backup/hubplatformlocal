#include "scenes_manager/scenes_engine/events/weeks_of_period_event.h"
#include "scenes_manager/scenes_engine/model/model_resources.h"

#include "base_utils/gregorian_calendar_weeks.h"
#include "base_utils/gregorian_calendar.h"

#include "tiny/tiny_consolecolors.h"
#include "log_utils/ezlo_log.h"

namespace SceneEngine {

WeeksOfPeriodEvent::WeeksOfPeriodEvent( Period period, std::set< Week > weeks )
: m_weekList( std::move( weeks ) )
, m_period( period ) {}

std::string WeeksOfPeriodEvent::getMethodName() const {
    return Model::Resources::MethodNames::isDate;
}

WeeksOfPeriodEvent::OptionalClock WeeksOfPeriodEvent::getNextDate( const GregorianCalendar::TimePoint& now ) const {
    LOGGER_ERROR(
        cc::error( "Unexpected call to getNextDate in WeeksOfPeriodEvent, this method should not be called" ) );
    return nullptr;
}

bool WeeksOfPeriodEvent::isTimePointInRange( const GregorianCalendar::TimePoint& now ) const {
    GregorianCalendar::Date date( now );
    const uint8_t currentWeekNumber = getWeekOfPeriod( date );
    bool weekInRange = m_weekList.count( static_cast< Week >( currentWeekNumber ) ) > 0;

    constexpr int lastWeekMarker = -1;
    if( !weekInRange && m_weekList.count( lastWeekMarker ) > 0 ) {
        weekInRange = isLastWeekOfPeriod( date );
    }

    return weekInRange;
}

uint8_t WeeksOfPeriodEvent::getWeekOfPeriod( const GregorianCalendar::Date& date ) const {
    switch( m_period ) {
    case Period::Month:
        return GregorianCalendar::getWeekOfMonth( date );
    case Period::Year:
        return GregorianCalendar::getWeekOfYear( date );
    }

    return 0;
}

bool WeeksOfPeriodEvent::isLastWeekOfPeriod( const GregorianCalendar::Date& date ) const {
    switch( m_period ) {
    case Period::Month:
        return isLastWeekOfMonth( date );
    case Period::Year:
        return isLastWeekOfYear( date );
    }

    return false;
}

bool WeeksOfPeriodEvent::isLastWeekOfMonth( GregorianCalendar::Date date ) {
    const auto monthNow = date.getMonth();
    date.moveToWeekday( 1 );
    date.incrementWeek();
    return monthNow != date.getMonth();
}

bool WeeksOfPeriodEvent::isLastWeekOfYear( GregorianCalendar::Date date ) {
    const auto yearNow = date.getYear();
    date.moveToWeekday( 1 );
    date.incrementWeek();
    return yearNow != date.getYear();
}

}  // namespace SceneEngine
