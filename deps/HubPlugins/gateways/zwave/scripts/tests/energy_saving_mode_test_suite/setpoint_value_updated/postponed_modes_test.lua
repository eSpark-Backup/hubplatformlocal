local tests = require( "tests" )
local core = require( "core" )
local timer = require( "timer" )

local storage = hubplugins.object("storage_helper", "HUB:zwave/scripts/utils")
local events_handler = hubplugins.weak_loadfile("HUB:zwave/scripts/events/events_handling")
local class_ids = require("HUB:zwave/scripts/resources/class_ids")

local fixture = hubplugins.weak_loadfile("HUB:zwave/scripts/tests/fixtures/energy_saving_mode_fixture")()
fixture:init()

local function test_impl(mode)
    core.update_item_value(fixture.energy_saving_mode_item_id, mode)
    
    timer.set_timeout_with_id = tests.create_mock()
    
    events_handler({
        event = "thermostat_setpoint_value_updated",
        node_id = 9000,
        channel_id = 0,
        class_id = class_ids.thermostat_setpoint,
        type = 1,
        value = 0,
        scale = 1
    })
    
    events_handler({
        event = "thermostat_setpoint_value_updated",
        node_id = 9000,
        channel_id = 0,
        class_id = class_ids.thermostat_setpoint,
        type = 2,
        value = 0,
        scale = 1
    })
        
    tests.assert_called(timer.set_timeout_with_id, "check timer created", {})
    tests.assert_eq(core.get_item(fixture.energy_saving_mode_item_id).value, mode)
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
        },
        {
            value = "off",
            description = "off mode test"
        }
    }
)
