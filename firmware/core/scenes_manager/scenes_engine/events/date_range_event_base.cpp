// Extracted from event_base.h/cpp
#include "scenes_manager/scenes_engine/events/date_range_event_base.h"

namespace SceneEngine {

DateRangeEventBase::DateRangeEventBase()
: m_isInRange( false ) {}

bool DateRangeEventBase::isInRange() const {
    return m_isInRange;
}

bool DateRangeEventBase::update( const GregorianCalendar::TimePoint& now ) {
    const bool nowInRange = isTimePointInRange( now );
    const bool wasInRange = m_isInRange.exchange( nowInRange, std::memory_order_relaxed );
    return wasInRange != nowInRange;
}

}  // namespace SceneEngine
