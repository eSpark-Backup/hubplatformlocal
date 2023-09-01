local tests = require( "tests" )
local core = require( "core" )
local timer = require( "timer" )
local zwave = require( "zwave" )

local storage = hubplugins.object("storage_helper", "HUB:zwave/scripts/utils")
local class_ids = require( "HUB:zwave/scripts/resources/class_ids" )

local fixture = hubplugins.weak_loadfile("HUB:zwave/scripts/tests/fixtures/energy_saving_mode_fixture")()
fixture:init()

core.get_local_time = function(...) return "2021-03-04T04:05:18" end
core.get_next_week_event = function(...) return "2022-01-01T01:01:01" end

local function test_impl(params)
    core.update_item_value(fixture.energy_saving_mode_item_id, params.postponed_mode)
    core.update_item_value(fixture.setpoint_heating_item_id, { value = 0, scale = "fahrenheit" })
    core.update_item_value(fixture.setpoint_cooling_item_id, { value = 0, scale = "fahrenheit" }) 
    storage:call(
        "set_postponed_state_end_time", 
        fixture.energy_saving_mode_item_id, 
        {year = 2021, mounth = 3, day = 4, hour = 3, minute = 0, second = 0}
    )
    
    zwave.set_setpoint_value = tests.create_mock()
    
    hubplugins.weak_loadfile("HUB:zwave/scripts/helpers/process_thermostats_energy_saving_mode")()
    
    tests.assert_eq(core.get_item(fixture.energy_saving_mode_item_id).value, params.active_mode)
    tests.assert_eq(storage:call("find_postponed_state_end_time", fixture.energy_saving_mode_item_id), nil)
    
    tests.assert_called(zwave.set_setpoint_value, "check set setpoint value", {
        { "9000", class_ids.thermostat_setpoint, 0, 1, { value = params.heat_value, scale = "fahrenheit" } },
        { "9000", class_ids.thermostat_setpoint, 0, 2, { value = params.cool_value, scale = "fahrenheit" } }
    })
end

tests.run_with_params(
    test_impl,
    {
        {
            value = {
                postponed_mode = "low_postponed",
                active_mode = "low",
                heat_value = 68,
                cool_value = 74
            },
            description = "low_postponed mode test"
        },
        {
            value = {
                postponed_mode = "moderate_postponed",
                active_mode = "moderate",
                heat_value = 66,
                cool_value = 78
            },
            description = "moderate_postponed mode test"
        },
        {
            value = {
                postponed_mode = "aggressive_postponed",
                active_mode = "aggressive",
                heat_value = 62,
                cool_value = 82
            },
            description = "aggressive_postponed mode test"
        }
    }
)
