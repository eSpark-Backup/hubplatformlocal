local utils = {}

local tests = require("tests")
local core = require("core")
local storage = require "storage"
local storage_keys = require("HUB:cloud_devices/scripts/resources/storage_keys")

function utils.check_cloud_device_storage_associations(cloud_device_id)
    local core_device_id = storage.get_string(storage_keys.core_id_by_cloud_id_prefix .. cloud_device_id)
    tests.assert_ne(core_device_id, nil, "There should be non-nil core_device_id associated with cloud device:" .. cloud_device_id)
    tests.assert_eq(storage.get_string(storage_keys.cloud_id_by_core_id_prefix .. core_device_id), cloud_device_id, "There should be non-nil cloud_device_id associated with core device:" .. core_device_id)
end

function utils.check_no_storage_associations()
    tests.assert_eq(storage.empty(), true)
end

function utils.get_core_id(cloud_device_id)
    return storage.get_string(storage_keys.core_id_by_cloud_id_prefix .. cloud_device_id)
end

function utils.get_core_item_id(cloud_device_id, item_name)
    assert(cloud_device_id ~= nil, "cloud_device_id is nil")
    assert(item_name ~= nil, "item_name is nil")
    local core_device_id = utils.get_core_id(cloud_device_id)
    local items = require("core").get_items_by_device_id(core_device_id)
    for _, item in pairs(items) do
        if item.name == item_name then
            return item.id
        end
    end
    return nil
end


function utils.expect_item(device_id, expected)
    local description = expected.id or expected.name
    local actual
    if expected.id then
        actual = core.get_item(expected.id)
    end
    if expected.name then
        local items = core.get_items_by_device_id(device_id)
        for _, item in pairs(items) do
            if item.name == expected.name then
                actual = item
            end
        end
    end
    tests.assert_ne(actual, nil, "item " .. description .. " should exist")

    expected.device_id = device_id
    for key, value in pairs(expected) do
        tests.assert_eq(actual[key], value, "item " .. description .. "." .. key .. " should meet expectations")
    end
end

function utils.expect_items(device_id, items)
    local expected_items_set = {}
    for _, item in ipairs(items) do
        local description = item.id or item.name
        expected_items_set[description] = true
        utils.expect_item(device_id, item)
    end

    local actual_items = core.get_items_by_device_id(device_id)
    if actual_items then
        for _, item in pairs(actual_items) do
            local description = item.id or item.name
            local found = expected_items_set[item.id] or expected_items_set[item.name]
            tests.assert_eq(found, true, "item " .. description .. " should not exist")
        end
    end
end

function utils.expect_device(expected)
    local actual = tests.assert_ne(core.get_device(expected.id), nil, "device " .. expected.id .. " should exist")
    local expected_items = expected.items
    expected.items = nil

    for key, value in pairs(expected) do
        tests.assert_eq(actual[key], value, "device " .. expected.id .. "." .. key .. " should meet expectations")
    end

    if expected_items then
        utils.expect_items(expected.id, expected_items)
    end
end

function utils.expect_devices(devices)
    local expected_devices_set = {}
    for _, device in ipairs(devices) do
        expected_devices_set[device.id] = true
        utils.expect_device(device)
    end

    local actual_devices = core.get_devices()
    if actual_devices then
        for _, device in ipairs(actual_devices) do
            tests.assert_eq(expected_devices_set[device.id], true, "device " .. device.id .. " should not exist")
        end
    end
end

return utils
