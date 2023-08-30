#pragma once

#include "scenes_manager/scenes_engine/events/date_event.h"
#include "base_utils/gregorian_calendar.h"

#include <set>
#include <functional>

namespace SceneEngine {

class DateScheduler {
public:
    using ClockDuration = GregorianCalendar::ClockDuration;

    struct EventEntry {
        DateEventBasePtr event;
        ClockDuration nextDate{ 0 };
        ClockDuration date{ 0 };

        bool operator<( const EventEntry& other ) const;
    };
    using Queue = std::multiset< EventEntry >;

public:
    void setTime( ClockDuration now );
    void enableRecalculationOnBigTimeshift( bool enabled );

    bool empty() const;
    void push( DateEventBasePtr event );
    void eraseBySceneId( const Scene::SceneID& id );

    std::vector< DateEventBasePtr > collectActivatedEvents();

    const Queue& getSchedule() const;

private:
    void recalculateDates( ClockDuration now );
    static EventEntry recalculateEntry( const EventEntry& entry, ClockDuration now );

private:
    Queue m_queue;

    ClockDuration m_time{ 0 };
    bool m_recalculationEnabled{ true };
};

}  // namespace SceneEngine
