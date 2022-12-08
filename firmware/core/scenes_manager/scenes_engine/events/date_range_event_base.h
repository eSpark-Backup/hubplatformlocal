#pragma once
// Extracted from event_base.h/cpp
#include "scenes_manager/scenes_engine/events/date_event_base.h"
#include <atomic>

namespace SceneEngine {

class DateRangeEventBase : public DateEventBase {
public:
    DateRangeEventBase();

    bool isInRange() const;
    bool update( const GregorianCalendar::TimePoint& now );  // returns "true" if crossed an edge of the range

private:
    virtual bool isTimePointInRange( const GregorianCalendar::TimePoint& now ) const = 0;

    std::atomic_bool m_isInRange;
};

using DateRangeEventBasePtr = std::shared_ptr< DateRangeEventBase >;

}  // namespace SceneEngine
