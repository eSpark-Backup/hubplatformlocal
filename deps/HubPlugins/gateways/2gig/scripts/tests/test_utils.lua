local twogig = require("2gig")
local core = require("core")
local tests = require("tests")
local add_device = loadfile("HUB:2gig/scripts/add_device")

twogig.configure_device = tests.create_mock()

local test_utils = {}

local function get_index(id)
    return tonumber(id:sub(id:find("_[^_]*$") + 1))
end

local function compare_ids(l, r)
    return get_index(l.id) < get_index(r.id)
end

local function check_device(device_idx, device_id, actual, expected)
    local keys = { "name", "category", "subcategory", "device_type_id", "battery_powered", "info" }

    for _, key in ipairs(keys) do
        tests.assert_eq(actual[key], expected[key], ('[#%d %s] %s'):format(device_idx, device_id, key))
    end
end

local function check_item(device_idx, device_id, item_idx, item_id, actual, expected)
    local keys = { "name", "value" }

    for _, key in ipairs(keys) do
        tests.assert_eq(actual[key], expected[key], ('[#%d %s][#%d %s] %s'):format(device_idx, device_id, item_idx, item_id, key))
    end
end

local function check_setting(device_idx, device_id, setting_idx, setting_id, actual, expected)
    local keys = { "info", "value", "value_default", "value_min", "value_max" }

    for _, key in ipairs(keys) do
        tests.assert_eq(actual[key], expected[key], ('[#%d %s][#%d %s] %s'):format(device_idx, device_id, setting_idx, setting_id, key))
    end
end

function test_utils.check_devices(expected_devices)
    local devices = core.get_devices()
    
    if devices == nil then
        tests.assert_eq(expected_devices, nil, "devices count = 0")
        return
    end
    
    tests.assert_eq(#devices, #expected_devices, "devices count")

    table.sort(devices, compare_ids)

    for device_idx = 1, #devices do
        local actual_device = devices[device_idx]
        local expected_device = expected_devices[device_idx]

        check_device(device_idx, actual_device.id, actual_device, expected_device)

        test_utils.check_items(device_idx, actual_device.id, expected_device.items)
        test_utils.check_settings(device_idx, actual_device.id, expected_device.settings)
    end
end

function test_utils.check_items(device_idx, device_id, expected_items)
    local items = core.get_items_by_device_id(device_id)
    tests.assert_eq(#items, #expected_items, "items count")

    table.sort(items, compare_ids)

    for item_idx = 1, #items do
        local actual_item = items[item_idx]
        local expected_item = expected_items[item_idx]

        check_item(device_idx, device_id, item_idx, actual_item.id, actual_item, expected_item)
    end
end

function test_utils.check_settings(device_idx, device_id, expected_settings)
    local setting_ids = core.get_setting_ids_by_device_id(device_id)
    if expected_settings == nil then
        tests.assert_eq(setting_ids, expected_settings)
        return
    end

    tests.assert_eq(#setting_ids, #expected_settings, "settings count")

    local settings = {}
    for _, setting_id in pairs(setting_ids) do
        local setting = core.get_setting(setting_id)
        table.insert(settings, setting)
    end
    table.sort(settings, compare_ids)

    for setting_idx = 1, #settings do
        local actual_setting = settings[setting_idx]
        local expected_seting = expected_settings[setting_idx]

        check_setting(device_idx, device_id, setting_idx, actual_setting.id, actual_setting, expected_seting)
    end
end

function test_utils.test_device_adding(manufacturer, model, device_type, base_device)
    local full_device_model = manufacturer .. "-" .. model

    add_device({
        model = full_device_model,
        serial = 123
    })

    add_device({
        serial = 456,
        device_type = device_type
    })

    if base_device.settings ~= nil then
        local configuration = {}
        for _, setting in ipairs(base_device.settings) do
            configuration[setting.info.tag] = setting.value
        end
        tests.assert_called(twogig.configure_device, "Check twogig.configure_device call", {
            { { 123 }, configuration },
            { { 456 }, configuration }
        })
    end

    local expected_device_from_model = tests.deep_copy(base_device)
    expected_device_from_model.device_type_id = full_device_model
    expected_device_from_model.info = { manufacturer = manufacturer, model = model }
    expected_device_from_model.battery_powered = true

    local expected_device_from_device_type = tests.deep_copy(base_device)
    expected_device_from_device_type.device_type_id = device_type
    expected_device_from_device_type.battery_powered = true

    test_utils.check_devices({
        expected_device_from_model,
        expected_device_from_device_type
    })
end

local function flatten_device_tree(root)
    if root.child_devices == nil then
        return { tests.deep_copy(root) }
    end

    local result = { tests.deep_copy(root) }

    for _, device in ipairs(result[1].child_devices) do
        result[#result + 1] = tests.deep_copy(device)
    end

    result[1].child_devices = nil

    return result
end

function test_utils.test_device_tree_adding(manufacturer, model, device_type, base_device)
    local full_device_model = manufacturer .. "-" .. model

    -- Note: don't test device adding through device_type, since supported device types are comprised of 1 device (for now)

    add_device({
        model = full_device_model,
        serial = 123
    })

    local expected_devices_from_model = flatten_device_tree(base_device)
    for _, device in pairs(expected_devices_from_model) do
        device.info = { manufacturer = manufacturer, model = model }
        device.device_type_id = full_device_model
        device.battery_powered = true
    end

    test_utils.check_devices(expected_devices_from_model)
end

function test_utils.get_item_id(serial, item_name)
    local storage_utils = require("HUB:2gig/scripts/helpers/storage_utils")
    local core_device_id = assert(storage_utils.find_device_id(serial), "Device not found")
    return assert(storage_utils.find_item_id(core_device_id, item_name))
end

return test_utils
