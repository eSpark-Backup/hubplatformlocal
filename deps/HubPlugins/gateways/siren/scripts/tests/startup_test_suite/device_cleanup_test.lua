local tests = require "tests"
local core = require("core")
local storage = require("storage")
local storage_keys_getter = loadfile("HUB:siren/scripts/utils/storage_keys_getter")()
local logger = require("HUB:siren/scripts/utils/log")
local item_names = assert(loadfile("HUB:siren/scripts/items/names")(), "Failed to load item_names")
local device_storage_key = storage_keys_getter.get_device_key(item_names.switch)

-- @return: an array of items (table) or nil in a case of an error.
local function core_get_items_by_device_id(device_id)
    -- should not fail
    local items = {}

    for _, item in pairs(core.items) do
        if item.device_id == device_id then
            table.insert(items, item)
        end
    end

    -- should return nil in case of empty table
    if #items == 0 then
        return nil
    end
    return items
end

-- replace method with one that matches the docs
core.get_items_by_device_id = core_get_items_by_device_id

local siren = loadfile("HUB:siren/scripts/device/siren")()
local wrong_siren_device = siren
wrong_siren_device.device_type_id = "e550_siren_!!!!!!!!!!!!! so wrong !!!!!!!!!!!"

local add_device_res = core.add_device(wrong_siren_device)

storage.set_string(device_storage_key, add_device_res.id)

core.add_device = tests.create_mock(add_device_res)

-- prepare
local dev_id = storage.get_string(device_storage_key)

if dev_id == nil then
    logger.trace("Start device creation " .. device_storage_key)

    dev_id = loadfile("HUB:siren/scripts/device/create_device")(nil, item_names.switch)
    storage.set_string(device_storage_key, dev_id)
end

-- test
assert(loadfile("HUB:siren/scripts/startup")())

--tests.assert_called(core.add_device, "add_device called", function() end, 1)
tests.assert_called(core.add_device, "add_device called", add_device_res, 1)

local devices = core.get_devices() or {}
local items = core.get_items() or {}
tests.assert_eq(#devices, 1)
tests.assert_eq(#items, 4)
tests.assert_eq(devices[1].device_type_id, siren.device_type_id, "Device should be recreated")

local function array_find(arr, pred)
    for _, v in ipairs(arr) do
        if pred(v) then
            return v
        end
    end
    return nil
end

local function make_pred(item_name)
    local function find_item_index(core_item)
        if item_name == core_item.name then
            return true
        end
        return false
    end
    return find_item_index
end

for _, the_name in pairs(item_names) do
    -- check if switch item in core and switch item in storage has same ids
    local item_storage_key = storage_keys_getter.get_item_key(the_name, nil)
    local stored_item_id = storage.get_string(item_storage_key)
    logger.debug("Stored item id:", stored_item_id, "")

    local found_item = array_find(items, make_pred(the_name))
    tests.assert_eq(
        found_item.id,
        stored_item_id,
        "Stored item id should match with core.item[].id for name " .. the_name
    )
end
