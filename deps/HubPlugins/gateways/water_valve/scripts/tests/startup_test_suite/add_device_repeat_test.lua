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

core.add_device = tests.create_mock()
core.add_item = tests.create_mock()

-- Repeat startup
loadfile("HUB:water_valve/scripts/startup")()

tests.assert_called(core.add_device, "Check device not added", {})
tests.assert_called(core.add_item, "Check item not added", {})
