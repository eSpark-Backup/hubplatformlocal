#include "scenes_manager/scenes_engine/events/days_of_period_event.h"
#include "scenes_manager/scenes_engine/model/model_resources.h"
#include "scenes_manager/scenes_engine/exceptions.h"

#include "base_utils/gregorian_calendar.h"
#include "log_utils/ezlo_log.h"
#include "tiny/tiny_consolecolors.h"

namespace SceneEngine {

DaysOfPeriodEvent::DaysOfPeriodEvent( Period period, std::set< uint8_t > days )
: m_daysList( std::move( days ) )
, m_period( period ) {
    verifyDaysValues();
}

std::string DaysOfPeriodEvent::getMethodName() const {
    return Model::Resources::MethodNames::isDate;
}

DaysOfPeriodEvent::OptionalClock DaysOfPeriodEvent::getNextDate( const GregorianCalendar::TimePoint& now ) const {
    LOGGER_ERROR(
        cc::error( "Unexpected call to getNextDate in DaysOfPeriodEvent, this method should not be called" ) );
    return nullptr;
}

bool DaysOfPeriodEvent::isTimePointInRange( const GregorianCalendar::TimePoint& now ) const {
    GregorianCalendar::Date date( now );
    const uint8_t day = getDayOfPeriod( date );
    return m_daysList.find( day ) != m_daysList.end();
}

uint8_t DaysOfPeriodEvent::getDayOfPeriod( const GregorianCalendar::Date& date ) const {
    switch( m_period ) {
    case Period::Month:
        return date.getDay();
    case Period::Week:
        return date.getWeekday();
    }

    LOGGER_ERROR( cc::error( "Unhandled period type: " ) + cc::num( (int) m_period ) );
    return 0;
}

void DaysOfPeriodEvent::verifyDaysValues() const {
    const uint8_t maxValue = [this]() {
        switch( m_period ) {
        case Period::Month:
            return 31;
        case Period::Week:
            return 7;
        }
        return 0;
    }();

    for( uint8_t day : m_daysList ) {
        if( day > maxValue )
            throw SceneException( Errors::INVALID_VALUE_RANGE, "Day number exceeded allowed range limit" );
    }
}

}  // namespace SceneEngine
