local tests = require( "tests" )

local zwave = require( "zwave" )
local core = require( "core" )
local class_ids = require( "HUB:zwave/scripts/resources/class_ids" )

local fixture = hubplugins.weak_loadfile("HUB:zwave/scripts/tests/fixtures/energy_saving_mode_fixture")()
fixture:init()

core.update_item_value(fixture.energy_saving_mode_item_id, "low")
core.get_next_week_event = function(...) return "2022-01-01T01:01:01" end

local function test_impl(params)
    fixture:set_setpoints_spread(params.spread)
    core.update_item_value(fixture.setpoint_heating_item_id, { value = 0, scale = "fahrenheit" })
    core.update_item_value(fixture.setpoint_cooling_item_id, { value = 0, scale = "fahrenheit" }) 
    core.get_local_time = function(...) return params.local_time end   
    
    zwave.set_setpoint_value = tests.create_mock()
    
    hubplugins.weak_loadfile("HUB:zwave/scripts/helpers/process_thermostats_energy_saving_mode")()
    
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
                local_time = "2021-01-01T06:00:00",
                spread = 3,
                heat_value = 70,
                cool_value = 73
            },
            description = "Spread 3"
        },
        {
            value = {
                local_time = "2021-01-01T07:00:00",
                spread = 4,
                heat_value = 70,
                cool_value = 74
            },
            description = "Spread 4"
        }
    }
)
