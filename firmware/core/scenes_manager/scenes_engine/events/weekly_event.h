#pragma once
// Extracted from date_event.h/cpp
#include "scenes_manager/scenes_engine/events/date_event.h"

namespace SceneEngine {

class WeeklyEvent : public DateEvent {
public:
    WeeklyEvent( TimeList timeList, std::set< Day > weekdays );
    OptionalClock getNextDate( const GregorianCalendar::TimePoint& now ) const override;

private:
    TimeList m_timeList;
    std::set< Day > m_weekdayList;
};

}  // namespace SceneEngine
