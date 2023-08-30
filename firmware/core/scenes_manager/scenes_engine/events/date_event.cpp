#include "scenes_manager/scenes_engine/events/date_event.h"
#include "common_utils/cpp14.h"
#include "log_utils/ezlo_log.h"
#include "tiny/tiny_consolecolors.h"
#include "scenes_manager/scenes_engine/model/model_resources.h"

namespace SceneEngine {

std::string DateEvent::getMethodName() const {
    return Model::Resources::MethodNames::isDate;
}

DateEvent::OptionalClock DateEvent::findNextTime( GregorianCalendar::ClockDuration now,
                                                  GregorianCalendar::Date& date,
                                                  const TimeList& timeList ) const {
    for( const auto& time : timeList ) {
        date.setHour( time.first );
        date.setMinute( time.second );
        date.setSecond( 0 );
        date.setMillisecond( 0 );
        auto clock = date.toClockDuration();
        if( clock > now ) {
            LOGGER_TRACE( "next time is found: " + std::to_string( clock.count() ) );
            return std::make_unique< GregorianCalendar::ClockDuration >( clock );
        }
    }
    LOGGER_TRACE( "next time not found" );
    return nullptr;
}

}  // namespace SceneEngine
