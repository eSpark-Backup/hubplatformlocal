local water_valve = assert(require "water_valve", "Fail to load water_valve functionality")
local storage = assert(require "storage", "Fail to load storage functionality")
local core = assert(require "core", "Fail to load core functionality")
local tests = require("tests")

local storage_keys_getter = loadfile("HUB:water_valve/scripts/utils/storage_keys_getter")()
local item_names = loadfile("HUB:water_valve/scripts/resources/item_names")()
local fixture = loadfile("HUB:water_valve/scripts/tests/fixtures/base_fixture")()

-- Run startup
loadfile("HUB:water_valve/scripts/startup")()

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
