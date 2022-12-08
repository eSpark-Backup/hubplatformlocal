local tests = require( "tests" )

local zwave = require( "zwave" )
local core = require( "core" )
local class_ids = require( "HUB:zwave/scripts/resources/class_ids" )

local fixture = hubplugins.weak_loadfile("HUB:zwave/scripts/tests/fixtures/energy_saving_mode_fixture")()
fixture:init()

core.update_item_value(fixture.energy_saving_mode_item_id, "aggressive")
core.get_next_week_event = function(...) return "2022-01-01T01:01:01" end

local function test_impl(params)
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
                heat_value = 68,
                cool_value = 78
            },
            description = "Wake start"
        },
        {
            value = {
                local_time = "2021-01-01T07:00:00",
                heat_value = 68,
                cool_value = 78
            },
            description = "Wake progress"
        },
        {
            value = {
                local_time = "2021-01-01T08:00:00",
                heat_value = 62,
                cool_value = 85
            },
            description = "Day start"
        },
        {
            value = {
                local_time = "2021-01-01T17:30:00",
                heat_value = 62,
                cool_value = 85
            },
            description = "Day progress"
        },
        {
            value = {
                local_time = "2021-01-01T18:00:00",
                heat_value = 68,
                cool_value = 78
            },
            description = "Evening start"
        },
        {
            value = {
                local_time = "2021-01-01T19:30:00",
                heat_value = 68,
                cool_value = 78
            },
            description = "Evening progress"
        },
        {
            value = {
                local_time = "2021-01-01T22:00:00",
                heat_value = 62,
                cool_value = 82
            },
            description = "Sleep start"
        },
        {
            value = {
                local_time = "2021-01-01T05:30:00",
                heat_value = 62,
                cool_value = 82
            },
            description = "Sleep progress"
        }
    }
)
