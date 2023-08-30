local water_valve = assert(require "water_valve", "Fail to load water_valve functionality")
local core = assert(require "core", "Fail to load core functionality")
local tests = require("tests")

local item_names = loadfile("HUB:water_valve/scripts/resources/item_names")()
local fixture = loadfile("HUB:water_valve/scripts/tests/fixtures/base_fixture")()
local events_handler = loadfile("HUB:water_valve/scripts/events_handling")

water_valve.sensors_count = 2

-- Run startup
loadfile("HUB:water_valve/scripts/startup")()

local item_1_id = fixture.get_item(item_names.water_leak_alarm, 0).id
local item_2_id = fixture.get_item(item_names.water_leak_alarm, 1).id

-- test leak sensor 1
do
    core.update_item_value = tests.create_mock()
    events_handler(
        {
            event = "water_leak_value_updated",
            value = true,
            sensor_index = 0
        }
    )
    tests.assert_called(core.update_item_value, "Check items updated", {{item_1_id, "water_leak_detected"}})
end

-- test no leak sensor 1
do
    core.update_item_value = tests.create_mock()
    events_handler(
        {
            event = "water_leak_value_updated",
            value = false,
            sensor_index = 0
        }
    )
    tests.assert_called(core.update_item_value, "Check items updated", {{item_1_id, "no_water_leak"}})
end

-- test leak sensor 2
do
    core.update_item_value = tests.create_mock()
    events_handler(
        {
            event = "water_leak_value_updated",
            value = true,
            sensor_index = 1
        }
    )
    tests.assert_called(core.update_item_value, "Check items updated", {{item_2_id, "water_leak_detected"}})
end

-- test no leak sensor 2
do
    core.update_item_value = tests.create_mock()
    events_handler(
        {
            event = "water_leak_value_updated",
            value = false,
            sensor_index = 1
        }
    )
    tests.assert_called(core.update_item_value, "Check items updated", {{item_2_id, "no_water_leak"}})
end
