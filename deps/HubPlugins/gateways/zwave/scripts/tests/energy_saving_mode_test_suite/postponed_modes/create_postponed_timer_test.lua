local tests = require( "tests" )
local core = require( "core" )
local timer = require( "timer" )

local storage = hubplugins.object("storage_helper", "HUB:zwave/scripts/utils")

local fixture = hubplugins.weak_loadfile("HUB:zwave/scripts/tests/fixtures/energy_saving_mode_fixture")()
fixture:init()

core.get_local_time = function(...) return "2021-03-04T04:05:18" end

local function test_impl(mode, time_params)
    core.update_item_value(fixture.energy_saving_mode_item_id, mode)
    storage:call("set_postponed_state_end_time", fixture.energy_saving_mode_item_id, time_params.time_with_delay)
    
    timer.set_timeout_with_id = tests.create_mock()
    hubplugins.weak_loadfile("HUB:zwave/scripts/helpers/process_thermostats_energy_saving_mode")()
    
    tests.assert_called(timer.set_timeout_with_id, "check timer created", {
        { 
            time_params.timer_duration, 
            "zwave_thermostats_energy_saving_timer", 
            "HUB:zwave/scripts/timer_handlers/timeout/process_thermostats_energy_saving_modes", 
            {}
        }
    })
end

tests.run_with_params(
    test_impl,
    {
        {
            value = "low_postponed",
            description = "low_postponed mode test"
        },
        {
            value = "moderate_postponed",
            description = "moderate_postponed mode test"
        },
        {
            value = "aggressive_postponed",
            description = "aggressive_postponed mode test"
        }
    },
    {
        {
            value = {
                time_with_delay = {year = 2021, mounth = 3, day = 4, hour = 12, minute = 0, second = 0},
                timer_duration = 28482000
            },
            description = "same day"
        },
        {
            value = {
                time_with_delay = {year = 2021, mounth = 3, day = 5, hour = 12, minute = 0, second = 0},
                timer_duration = 71683000
            },
            description = "next day"
        }
    }
)
