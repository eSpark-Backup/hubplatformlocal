// Extracted from date_event.h/cpp
#include "scenes_manager/scenes_engine/events/once_event.h"
#include "scenes_manager/scenes_engine/model/model_resources.h"
#include "common_utils/cpp14.h"

namespace SceneEngine {

OnceEvent::OnceEvent( const GregorianCalendar::Date& date )
: m_date( date ) {}

std::string OnceEvent::getMethodName() const {
    return Model::Resources::MethodNames::isOnce;
}

DateEvent::OptionalClock OnceEvent::getNextDate( const GregorianCalendar::TimePoint& /*now*/ ) const {
    return std::make_unique< GregorianCalendar::ClockDuration >( m_date.toClockDuration() );
}

}  // namespace SceneEngine
