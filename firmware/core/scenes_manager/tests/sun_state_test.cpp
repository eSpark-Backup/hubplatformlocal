#include "scenes_manager/scenes_engine/sunstate_processor/sun_state.h"
#include "scenes_manager/tests/resources/sun_state_reference.h"
#include "base_utils/gregorian_calendar.h"

#include <gtest/gtest.h>
#include <gmock/gmock.h>

namespace SceneEngine {
namespace UT {

class SunStateTest : public testing::Test {};

TEST_F( SunStateTest, SunriseSunset ) {
    SunState sunState;
    for( const TestData& data : ReferenceRandomSunstatesList ) {
        sunState.setPosition( data.geoposition.latitude, data.geoposition.longitude, data.timeZoneOffset );
        sunState.setCurrentDate( data.date.year, data.date.month, data.date.day );
        SunState::Time sunriseTime = sunState.getSunriseTime();
        EXPECT_EQ( sunriseTime.hours, data.sunrise.hours ) << data;
        EXPECT_EQ( sunriseTime.minutes, data.sunrise.minutes ) << data;
        SunState::Time sunsetTime = sunState.getSunsetTime();
        EXPECT_EQ( sunsetTime.hours, data.sunset.hours ) << data;
        EXPECT_EQ( sunsetTime.minutes, data.sunset.minutes ) << data;
    }
}

}  // namespace UT
}  // namespace SceneEngine
