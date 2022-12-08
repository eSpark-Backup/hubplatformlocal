local tests = require("tests")
local core = assert(require "core", "Fail to load core functionality")
local storage = assert(require "storage", "Fail to load storage functionality")
local storage_keys_getter = loadfile("HUB:water_valve/scripts/utils/storage_keys_getter")()

local fixture = {}

function fixture.init()
end

function fixture.get_device(item_name, index)
    local device_id = storage.get_string(storage_keys_getter.get_device_key(item_name, index))
    tests.assert_ne(device_id, nil)
    local device = core.get_device(device_id)
    tests.assert_ne(device, nil)
    return device
end

function fixture.get_item(item_name, index)
    local item_id = storage.get_string(storage_keys_getter.get_item_key(item_name, index))
    tests.assert_ne(item_id, nil)
    local item = core.get_item(item_id)
    tests.assert_ne(item, nil)
    return item
end

function fixture.check_device_structure(device, expectation)
    for key, value in pairs(expectation) do
        tests.assert_eq(device[key], value, "Check device field: " .. key)
    end
end

function fixture.check_item_structure(item, expectation)
    for key, value in pairs(expectation) do
        tests.assert_eq(item[key], value, "Check item field: " .. key)
    end
end

return fixture
