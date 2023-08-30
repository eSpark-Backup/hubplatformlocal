local water_valve = assert(require "water_valve", "Fail to load water_valve functionality")
local tests = require("tests")

local item_names = loadfile("HUB:water_valve/scripts/resources/item_names")()
local fixture = loadfile("HUB:water_valve/scripts/tests/fixtures/base_fixture")()
local set_item_value = loadfile("HUB:water_valve/scripts/set_item_value")

-- Run startup
loadfile("HUB:water_valve/scripts/startup")()

local switch_item_id = fixture.get_item(item_names.switch).id
water_valve.is_control_inverted = true

-- test close
do
    water_valve.set_water_switch_value = tests.create_mock()
    set_item_value({item_id = switch_item_id, value = true})
    tests.assert_called(water_valve.set_water_switch_value, "Check valve state changed", {{false}})
end

-- test open
do
    water_valve.set_water_switch_value = tests.create_mock()
    set_item_value({item_id = switch_item_id, value = false})
    tests.assert_called(water_valve.set_water_switch_value, "Check valve state changed", {{true}})
end
