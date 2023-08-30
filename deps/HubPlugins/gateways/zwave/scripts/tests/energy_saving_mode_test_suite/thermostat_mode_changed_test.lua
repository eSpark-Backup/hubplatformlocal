local tests = require( "tests" )
local core = require( "core" )
local timer = require( "timer" )

local storage = hubplugins.object("storage_helper", "HUB:zwave/scripts/utils")
local events_handler = hubplugins.weak_loadfile("HUB:zwave/scripts/events/events_handling")
local class_ids = require("HUB:zwave/scripts/resources/class_ids")
local mode_enum = hubplugins.weak_require("HUB:zwave/scripts/resources/enums/thermostat_mode")

local fixture = hubplugins.weak_loadfile("HUB:zwave/scripts/tests/fixtures/energy_saving_mode_fixture")()
fixture:init()


events_handler({
    event = "thermostat_mode_updated",
    node_id = 9000,
    channel_id = 0,
    class_id = class_ids.thermostat_mode,
    mode = mode_enum.token_to_value["off"]
})

tests.assert_eq(timer.exists("zwave_thermostats_energy_saving_mode_changed_timer"), true)
