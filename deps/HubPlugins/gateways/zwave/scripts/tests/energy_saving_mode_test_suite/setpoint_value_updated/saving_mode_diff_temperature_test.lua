local tests = require( "tests" )
local core = require( "core" )
local timer = require( "timer" )

local storage = hubplugins.object("storage_helper", "HUB:zwave/scripts/utils")
local events_handler = hubplugins.weak_loadfile("HUB:zwave/scripts/events/events_handling")
local class_ids = require("HUB:zwave/scripts/resources/class_ids")

local fixture = hubplugins.weak_loadfile("HUB:zwave/scripts/tests/fixtures/energy_saving_mode_fixture")()
fixture:init()

core.get_local_time = function(...) return "2021-03-04T04:05:18" end
core.calculate_time_with_delay = function(...) return "2021-03-04T12:05:18" end

local function update_setpoint(type, value, is_requested)
    events_handler({
        event = "thermostat_setpoint_value_updated",
        node_id = 9000,
        channel_id = 0,
        class_id = class_ids.thermostat_setpoint,
        type = type,
        value = value,
        scale = 1,
        is_requested = is_requested
    })
end


local function test_impl(params)
    timer.cancel("zwave_thermostats_energy_saving_timer")
    timer.cancel("zwave_thermostats_energy_saving_startup_timer")
    timer.cancel("zwave_thermostats_energy_saving_mode_changed_timer")
    if params.init_start_timer then
        timer.set_timeout_with_id(10000, "zwave_thermostats_energy_saving_startup_timer", "", {})
    end
    if params.init_mode_changed_timer then
        timer.set_timeout_with_id(10000, "zwave_thermostats_energy_saving_mode_changed_timer", "", {})
    end
    core.update_item_value(fixture.energy_saving_mode_item_id, params.mode)
    
    update_setpoint(1, params.heat, params.heat_requested ~= nil and params.heat_requested or false)
    update_setpoint(2, params.cool, params.cool_requested ~= nil and params.cool_requested or false)
    
    tests.assert_eq(timer.exists("zwave_thermostats_energy_saving_timer"), params.has_timer)
    tests.assert_eq(core.get_item(fixture.energy_saving_mode_item_id).value, params.mode_updated)
end

tests.run_with_params(
    test_impl,
    {
        -- low mode
        {
            value = {
                mode = "low",
                heat = 70,
                heat_requested = true,
                cool = 74,
                has_timer = true,
                mode_updated = "low"
            },
            description = "low heat wrong requested test"
        },
        {
            value = {
                mode = "low",
                heat = 68,
                cool = 70,
                cool_requested = true,
                has_timer = true,
                mode_updated = "low"
            },
            description = "low cool wrong requested test"
        },
        {
            value = {
                mode = "low",
                heat = 70,
                cool = 74,
                has_timer = true,
                mode_updated = "low_postponed"
            },
            description = "low heat wrong not requested test"
        },
        {
            value = {
                mode = "low",
                heat = 68,
                cool = 70,
                has_timer = true,
                mode_updated = "low_postponed"
            },
            description = "low cool wrong not requested test"
        },
        {
            value = {
                mode = "low",
                heat = 68,
                cool = 70,
                init_start_timer = true,
                has_timer = false,
                mode_updated = "low"
            },
            description = "low cool wrong not requested with startup timer test"
        },
        {
            value = {
                mode = "low",
                heat = 68,
                cool = 70,
                init_mode_changed_timer = true,
                has_timer = false,
                mode_updated = "low"
            },
            description = "low cool wrong not requested with mode_changed timer test"
        },
        -- moderate mode
        {
            value = {
                mode = "moderate",
                heat = 70,
                heat_requested = true,
                cool = 78,
                has_timer = true,
                mode_updated = "moderate"
            },
            description = "moderate heat wrong requested test"
        },
        {
            value = {
                mode = "moderate",
                heat = 66,
                cool = 70,
                cool_requested = true,
                has_timer = true,
                mode_updated = "moderate"
            },
            description = "moderate cool wrong requested test"
        },
        {
            value = {
                mode = "moderate",
                heat = 70,
                cool = 78,
                has_timer = true,
                mode_updated = "moderate_postponed"
            },
            description = "moderate heat wrong not requested test"
        },
        {
            value = {
                mode = "moderate",
                heat = 66,
                cool = 70,
                has_timer = true,
                mode_updated = "moderate_postponed"
            },
            description = "moderate cool wrong not requested test"
        },
        {
            value = {
                mode = "moderate",
                heat = 66,
                cool = 70,
                init_start_timer = true,
                has_timer = false,
                mode_updated = "moderate"
            },
            description = "moderate cool wrong not requested with startup timer test"
        },
        {
            value = {
                mode = "moderate",
                heat = 66,
                cool = 70,
                init_mode_changed_timer = true,
                has_timer = false,
                mode_updated = "moderate"
            },
            description = "moderate cool wrong not requested with mode changed timer test"
        },
        -- aggressive mode
        {
            value = {
                mode = "aggressive",
                heat = 70,
                heat_requested = true,
                cool = 82,
                has_timer = true,
                mode_updated = "aggressive"
            },
            description = "aggressive heat wrong requested test"
        },
        {
            value = {
                mode = "aggressive",
                heat = 62,
                cool = 70,
                cool_requested = true,
                has_timer = true,
                mode_updated = "aggressive"
            },
            description = "aggressive cool wrong requested test"
        },
        {
            value = {
                mode = "aggressive",
                heat = 70,
                cool = 82,
                has_timer = true,
                mode_updated = "aggressive_postponed"
            },
            description = "aggressive heat wrong not requested test"
        },
        {
            value = {
                mode = "aggressive",
                heat = 62,
                cool = 70,
                has_timer = true,
                mode_updated = "aggressive_postponed"
            },
            description = "aggressive cool wrong not requested test"
        },
        {
            value = {
                mode = "aggressive",
                heat = 62,
                cool = 70,
                init_start_timer = true,
                has_timer = false,
                mode_updated = "aggressive"
            },
            description = "aggressive cool wrong not requested with startup timer test"
        },
        {
            value = {
                mode = "aggressive",
                heat = 62,
                cool = 70,
                init_mode_changed_timer = true,
                has_timer = false,
                mode_updated = "aggressive"
            },
            description = "aggressive cool wrong not requested with mode changed timer test"
        }
    }
)
