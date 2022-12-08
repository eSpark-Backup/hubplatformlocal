#pragma once
// Extracted from event_base.h/cpp
#include "scenes_manager/scenes_engine/events/event_base.h"
#include "base_utils/gregorian_calendar.h"

namespace SceneEngine {

class DateEventBase : public EventBase {
public:
    using Minute = uint8_t;
    using Hour = uint8_t;
    using Day = uint8_t;
    using Month = uint8_t;
    using Week = int8_t;
    using Year = uint16_t;

    using OptionalClock = std::unique_ptr< GregorianCalendar::ClockDuration >;
    virtual OptionalClock getNextDate( const GregorianCalendar::TimePoint& now ) const = 0;
};

using DateEventBasePtr = std::shared_ptr< DateEventBase >;

}  // namespace SceneEngine
