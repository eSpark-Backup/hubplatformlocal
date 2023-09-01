local water_valve = assert(require "water_valve", "Fail to load water_valve functionality")
local core = assert(require "core", "Fail to load core functionality")
local tests = require("tests")

local item_names = loadfile("HUB:water_valve/scripts/resources/item_names")()
local fixture = loadfile("HUB:water_valve/scripts/tests/fixtures/base_fixture")()
local events_handler = loadfile("HUB:water_valve/scripts/events_handling")

-- Run startup
loadfile("HUB:water_valve/scripts/startup")()

local switch_item_id = fixture.get_item(item_names.switch).id
local water_valve_state_item_id = fixture.get_item(item_names.water_valve_state).id
water_valve.is_control_inverted = true

-- test opened
do
    core.update_item_value = tests.create_mock()
    events_handler({ event = "valve_switch_value_updated", value = true })
    tests.assert_called(core.update_item_value, "Check items updated", {{switch_item_id, false}, {water_valve_state_item_id, "valve_is_opened"}})
end

-- test closed
do
    core.update_item_value = tests.create_mock()
    events_handler({ event = "valve_switch_value_updated", value = false })
    tests.assert_called(core.update_item_value, "Check items updated", {{switch_item_id, true}, {water_valve_state_item_id, "valve_is_closed"}})
end
