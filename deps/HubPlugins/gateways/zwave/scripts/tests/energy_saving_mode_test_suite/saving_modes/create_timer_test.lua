local tests = require( "tests" )
local core = require( "core" )
local timer = require( "timer" )

local storage = hubplugins.object("storage_helper", "HUB:zwave/scripts/utils")

local fixture = hubplugins.weak_loadfile("HUB:zwave/scripts/tests/fixtures/energy_saving_mode_fixture")()
fixture:init()

core.get_local_time = function(...) return "2021-03-04T04:05:18" end

local function test_impl(mode, time_params)
    core.update_item_value(fixture.energy_saving_mode_item_id, mode)
    
    local counter = 0
    core.get_next_week_event = function(...) 
        counter = counter + 1
        return time_params.week_event_times[counter]
    end
    
    timer.set_timeout_with_id = tests.create_mock()
    hubplugins.weak_loadfile("HUB:zwave/scripts/helpers/process_thermostats_energy_saving_mode")()
    
    local calls_counter = 0
    tests.assert_called(
        timer.set_timeout_with_id, 
        "check timer created", 
        function(call_index, duration, name, path, params)
            if name == "zwave_thermostats_energy_saving_timer" then
                calls_counter = calls_counter + 1
                tests.assert_eq(duration, time_params.timer_duration)
                tests.assert_eq(path, "HUB:zwave/scripts/timer_handlers/timeout/process_thermostats_energy_saving_modes")
                tests.assert_eq(params, {})
            end
        end,
        3
    )
    
    tests.assert_eq(calls_counter, 1)
end

tests.run_with_params(
    test_impl,
    {
        {
            value = "low",
            description = "low mode test"
        },
        {
            value = "moderate",
            description = "moderate mode test"
        },
        {
            value = "aggressive",
            description = "aggressive mode test"
        }
    },
    {
        {
            value = {
                week_event_times = {
                    "2021-03-04T05:06:49",
                    "2021-03-04T06:05:18",
                    "2021-03-04T06:08:18",
                    "2021-03-04T06:08:35"
                },
                timer_duration = 3691000
            },
            description = "same day"
        },
        {
            value = {
                week_event_times = {
                    "2021-03-05T05:06:49",
                    "2021-03-05T06:05:18",
                    "2021-03-05T06:08:18",
                    "2021-03-05T06:08:35"
                },
                timer_duration = 71683000
            },
            description = "next day"
        },
        {
            value = {
                week_event_times = {
                    "2021-03-04T05:06:49",
                    "2021-03-04T06:05:18",
                    "2021-03-04T06:08:18",
                    "2021-03-04T06:08:35"
                },
                timer_duration = 3691000
            },
            description = "Wake first"
        },
        {
            value = {
                week_event_times = {
                    "2021-03-04T07:06:49",
                    "2021-03-04T06:05:18",
                    "2021-03-04T06:08:18",
                    "2021-03-04T06:08:35"
                },
                timer_duration = 7200000
            },
            description = "Day first"
        },
        {
            value = {
                week_event_times = {
                    "2021-03-04T07:06:49",
                    "2021-03-04T06:05:18",
                    "2021-03-04T06:04:18",
                    "2021-03-04T06:08:35"
                },
                timer_duration = 7140000
            },
            description = "Evening first"
        },
        {
            value = {
                week_event_times = {
                    "2021-03-04T07:06:49",
                    "2021-03-04T06:05:18",
                    "2021-03-04T06:04:18",
                    "2021-03-04T05:08:35"
                },
                timer_duration = 3797000
            },
            description = "Sleep first"
        },
    }
)
