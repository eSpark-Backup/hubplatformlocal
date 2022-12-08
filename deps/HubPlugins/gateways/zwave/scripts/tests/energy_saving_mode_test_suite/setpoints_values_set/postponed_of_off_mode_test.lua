local tests = require( "tests" )

local zwave = require( "zwave" )
local core = require( "core" )
local storage = hubplugins.object("storage_helper", "HUB:zwave/scripts/utils")

local fixture = hubplugins.weak_loadfile("HUB:zwave/scripts/tests/fixtures/energy_saving_mode_fixture")()
fixture:init()

core.get_local_time = function(...) return "2021-01-01T01:01:01" end
storage:call(
    "set_postponed_state_end_time", 
    fixture.energy_saving_mode_item_id, 
    {
        year = 2021, mounth = 2, day = 1,
        hour = 1, minute = 1, second = 1
    })

local function test_impl(saving_mode)
    core.update_item_value(fixture.setpoint_heating_item_id, { value = 0, scale = "fahrenheit" })
    core.update_item_value(fixture.setpoint_cooling_item_id, { value = 0, scale = "fahrenheit" })    
    core.update_item_value(fixture.energy_saving_mode_item_id, saving_mode)
    
    zwave.set_setpoint_value = tests.create_mock()
    
    hubplugins.weak_loadfile("HUB:zwave/scripts/helpers/process_thermostats_energy_saving_mode")()
    
    tests.assert_called(zwave.set_setpoint_value, "check no set setpoint value", {})
end

tests.run_with_params(
    test_impl,
    {
        {
            value = "off",
            description = "off mode"
        },
        {
            value = "low_postponed",
            description = "low_postponed mode"
        },
        {
            value = "moderate_postponed",
            description = "moderate_postponed mode"
        },
        {
            value = "aggressive_postponed",
            description = "aggressive_postponed mode"
        }
    }
)
