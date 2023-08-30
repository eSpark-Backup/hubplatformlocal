#include "scenes_manager/scenes_engine/date_scheduler/schedule_handler.h"
#include "scenes_manager/scenes_engine/events/transactional_events_updater.h"
#include "scenes_manager/scenes_engine/helpers/clock_interface.h"
#include "scenes_manager/scenes_engine/scene_manager.h"

namespace SceneEngine {

ScheduleHandler::ScheduleHandler( SceneManager& sceneManager, IClock& clock )
: m_sceneManager( sceneManager )
, m_epsilon( std::chrono::milliseconds( 1000 ) )
, m_clock( clock )
, m_isEnabled( false )
, m_isTimeChanged( false ) {}

ScheduleHandler::~ScheduleHandler() {
    tearDown();
}

void ScheduleHandler::tearDown() {
    std::unique_lock< std::mutex > lock( m_mutex );
    m_isEnabled = false;
    lock.unlock();
    m_queueCondVar.notify_all();
    if( m_thread.joinable() ) {
        m_thread.join();
    }
}

void ScheduleHandler::start() {
    m_isEnabled = true;
    m_thread = std::thread( [this]() {
        init();
        while( true ) {
            std::unique_lock< std::mutex > lock( m_mutex );

            if( !m_isEnabled )
                break;

            m_queueCondVar.wait_for( lock, m_epsilon, [this] { return m_isTimeChanged || !m_isEnabled; } );
            m_isTimeChanged = false;
            threadHandler();
        }
    } );
}

void ScheduleHandler::push( DateEventBasePtr event ) {
    std::unique_lock< std::mutex > lock( m_mutex );
    m_dateScheduler.setTime( m_clock.get().localTimeSinceEpoch() );
    m_dateScheduler.push( std::move( event ) );
    lock.unlock();
    m_queueCondVar.notify_all();
}

void ScheduleHandler::push( DateRangeEventBasePtr event ) {
    std::unique_lock< std::mutex > lock( m_mutex );
    m_dateRangeScheduler.push( std::move( event ) );
    lock.unlock();
    m_queueCondVar.notify_all();
}

void ScheduleHandler::erase( const Scene::SceneID& sceneId ) {
    std::unique_lock< std::mutex > lock( m_mutex );
    m_dateScheduler.eraseBySceneId( sceneId );
    m_dateRangeScheduler.eraseBySceneId( sceneId );
}

void ScheduleHandler::setClock( IClock& clock ) {
    std::unique_lock< std::mutex > lock( m_mutex );
    m_clock = clock;
    m_dateScheduler.setTime( m_clock.get().localTimeSinceEpoch() );
}

void ScheduleHandler::onSystemTimeChanged() {
    std::unique_lock< std::mutex > lock( m_mutex );
    m_isTimeChanged = true;
    lock.unlock();
    m_queueCondVar.notify_all();
}

ScheduleHandler::SchedulePoints ScheduleHandler::getSchedule() const {
    std::unique_lock< std::mutex > lock( m_mutex );
    SchedulePoints schedulePoints;
    auto now = m_clock.get().localTime();

    const auto& dateSchedule = m_dateScheduler.getSchedule();
    for( auto& entry : dateSchedule ) {
        const Scene& scene = entry.event->getScene();

        SchedulePoint point;
        point.date = entry.date;
        point.method = entry.event->getMethodName();
        point.sceneId = scene.getId();
        point.name = scene.getName();
        schedulePoints.emplace( std::move( point ) );
    }

    const auto& dateRangeEvents = m_dateRangeScheduler.getEvents();
    for( auto& event : dateRangeEvents ) {
        const Scene& scene = event->getScene();

        SchedulePoint point;

        point.date = GregorianCalendar::ClockDuration::max();
        if( auto date = event->getNextDate( now ) )
            point.date = *date;

        point.method = event->getMethodName();
        point.sceneId = scene.getId();
        point.name = scene.getName();
        schedulePoints.emplace( std::move( point ) );
    }
    return schedulePoints;
}

bool ScheduleHandler::evaluateCurrentStateForEvent( DateRangeEventBase& event ) const {
    event.update( m_clock.get().localTime() );
    return event.isInRange();
}

void ScheduleHandler::init() {
    m_dateScheduler.setTime( m_clock.get().localTimeSinceEpoch() );
}

void ScheduleHandler::threadHandler() {
    const auto now = m_clock.get().localTime();
    m_dateScheduler.setTime( now.time_since_epoch() );
    auto activatedDateEvents = m_dateScheduler.collectActivatedEvents();

    TransactionalEventsUpdater updater = m_sceneManager.makeEventsUpdater();

    for( const auto& dateEvent : activatedDateEvents )
        updater.requestTrigger( dateEvent );
    m_dateRangeScheduler.update( now, updater );
    updater.flush();

    for( const auto& dateEvent : activatedDateEvents )
        updater.requestResetSilent( dateEvent );
    updater.flush();
}

}  // namespace SceneEngine
