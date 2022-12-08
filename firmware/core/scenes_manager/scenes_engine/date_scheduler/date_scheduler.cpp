#include "scenes_manager/scenes_engine/date_scheduler/date_scheduler.h"

#include "log_utils/ezlo_log.h"
#include "tiny/tiny_consolecolors.h"

namespace SceneEngine {

void DateScheduler::setTime( ClockDuration now ) {
    ClockDuration delta( std::abs( now.count() - m_time.count() ) );
    if( m_recalculationEnabled && delta > std::chrono::seconds( 6 ) ) {  // legacy, retained to avoid breaking anything
        recalculateDates( now );
    }

    m_time = now;
}

void DateScheduler::enableRecalculationOnBigTimeshift( bool enabled ) {
    m_recalculationEnabled = enabled;
}

bool DateScheduler::empty() const {
    return m_queue.empty();
}

void DateScheduler::push( DateEventBasePtr event ) {
    EventEntry entry;
    entry.event = std::move( event );
    entry = recalculateEntry( entry, m_time );
    m_queue.insert( std::move( entry ) );
}

void DateScheduler::eraseBySceneId( const Scene::SceneID& id ) {
    for( auto it = m_queue.begin(); it != m_queue.end(); ) {
        if( it->event->getSceneId() == id ) {
            it = m_queue.erase( it );
            continue;
        }
        ++it;
    }
}

const DateScheduler::Queue& DateScheduler::getSchedule() const {
    return m_queue;
}

std::vector< DateEventBasePtr > DateScheduler::collectActivatedEvents() {
    std::vector< DateEventBasePtr > result;
    auto it = m_queue.begin();
    while( it != m_queue.end() && it->nextDate <= m_time ) {
        result.push_back( it->event );

        EventEntry newEntry = recalculateEntry( *it, m_time );
        it = m_queue.erase( it );
        m_queue.insert( std::move( newEntry ) );
    }

    return result;
}

void DateScheduler::recalculateDates( ClockDuration now ) {
    Queue queue;
    while( !m_queue.empty() ) {
        auto it = m_queue.begin();
        EventEntry newEntry = recalculateEntry( *it, now );
        m_queue.erase( it );
        queue.insert( newEntry );
    }
    m_queue = std::move( queue );
}

DateScheduler::EventEntry DateScheduler::recalculateEntry( const EventEntry& entry, ClockDuration now ) {
    using namespace GregorianCalendar;

    EventEntry newEntry = entry;
    newEntry.nextDate = ClockDuration::max();
    if( auto date = newEntry.event->getNextDate( TimePoint( now ) ) ) {
        newEntry.date = *date;
        if( *date > now ) {
            newEntry.nextDate = *date;
        }
    }

    LOGGER_DEBUG( cc::debug( "Recalculated date event entry, event id: " + newEntry.event->getEventId() +
                             ", date: " + Date( TimePoint( newEntry.date ) ).toString() +
                             ", next date: " + Date( TimePoint( newEntry.nextDate ) ).toString() ) );
    return newEntry;
}

bool DateScheduler::EventEntry::operator<( const EventEntry& other ) const {
    return nextDate < other.nextDate;
}

}  // namespace SceneEngine
