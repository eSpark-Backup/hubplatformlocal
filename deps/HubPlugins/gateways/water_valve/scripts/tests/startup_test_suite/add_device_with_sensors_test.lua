local water_valve = assert(require "water_valve", "Fail to load water_valve functionality")
local storage = assert(require "storage", "Fail to load storage functionality")
local core = assert(require "core", "Fail to load core functionality")
local tests = require("tests")

local storage_keys_getter = loadfile("HUB:water_valve/scripts/utils/storage_keys_getter")()
local item_names = loadfile("HUB:water_valve/scripts/resources/item_names")()
local fixture = loadfile("HUB:water_valve/scripts/tests/fixtures/base_fixture")()

water_valve.sensors_count = 2

-- Run startup
loadfile("HUB:water_valve/scripts/startup")()

local function check_sensor_device(base_device_id, index)
    local device = fixture.get_device(item_names.water_leak_alarm, index)
    fixture.check_device_structure(
        device,
        {
            type = "sensor",
            category = "security_sensor",
            subcategory = "leak",
            name = index == 0 and "Water Leak Sensor" or "Water Leak Sensor" .. " " .. (index + 1),
            battery_powered = false,
            persistent = true,
            parent_device_id = base_device_id
        }
    )

    local items = core.get_items_by_device_id(device.id)
    tests.assert_eq(#items, 1)

    local item = fixture.get_item(item_names.water_leak_alarm, index)
    fixture.check_item_structure(
        item,
        {
            value_type = "token",
            enum = { "unknown", "no_water_leak", "water_leak_detected" },
            value = "unknown",
            has_getter = true,
            has_setter = false,
            name = item_names.water_leak_alarm,
            device_id = device.id
        }
    )
end

-- Check added devices/items
local device = fixture.get_device(item_names.switch)
fixture.check_device_structure(
    device,
    {
        name = "Water Valve",
        type = "switch.valve",
        device_type_id = "water_valve",
        category = "switch",
        subcategory = "valve",
        battery_powered = false,
        persistent = true,
        parent_device_id = ""
    }
)

local items = core.get_items_by_device_id(device.id)
tests.assert_eq(#items, 2)

local switch_item = fixture.get_item(item_names.switch)
fixture.check_item_structure(
    switch_item,
    {
        has_setter = true,
        has_getter = true,
        value_type = "bool",
        name = item_names.switch,
        value = false,
        device_id = device.id
    }
)

local water_valve_state_item = fixture.get_item(item_names.water_valve_state)
fixture.check_item_structure(
    water_valve_state_item,
    {
        value_type = "token",
        enum = { "unknown", "valve_is_opened", "valve_is_closed" },
        value = "unknown",
        has_getter = true,
        has_setter = false,
        name = item_names.water_valve_state,
        device_id = device.id
    }
)

check_sensor_device(device.id, 0)
check_sensor_device(device.id, 1)
