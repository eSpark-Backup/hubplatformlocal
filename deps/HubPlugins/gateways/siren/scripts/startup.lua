local speaker = assert(require "speaker", "Failed to load speaker functionality")
local storage_keys_getter = loadfile("HUB:siren/scripts/utils/storage_keys_getter")()
local storage = require("storage")
local core = require("core")
local logger = require("HUB:siren/scripts/utils/log")

local item_names = assert(loadfile("HUB:siren/scripts/items/names")(), "Failed to load item_names")

local function device_cleanup(main_item_name, dev_id)
    -- remove all items of the device
    local items = {}
    local has_device = core.get_device(dev_id)

    if has_device then
        items = core.get_items_by_device_id(dev_id) or {}
    end
    logger.debug("number of items to drop:", #items)

    -- remove all entries from storage
    for _, item in ipairs(items) do
        core.remove_item(item.id)
        if #(item.name) > 0 then
            local item_storage_key = storage_keys_getter.get_item_key(item.name or "")
            storage.delete(item_storage_key)
        end
    end

    for _, item_name in pairs(item_names) do
        storage.delete(storage_keys_getter.get_item_key(item_name))
    end

    if has_device then
        core.remove_device(dev_id)
    end
    local device_storage_key = storage_keys_getter.get_device_key(main_item_name, nil)
    storage.delete(device_storage_key)
end

local function add_device_if_needed(parent_dev_id, main_item_name, index)
    local device_storage_key = storage_keys_getter.get_device_key(main_item_name, index)
    local dev_id = storage.get_string(device_storage_key)

    if dev_id == nil then
        logger.trace("Start device creation " .. device_storage_key)

        dev_id = loadfile("HUB:siren/scripts/device/create_device")(parent_dev_id, main_item_name, index)
        storage.set_string(device_storage_key, dev_id)
    else
        -- device celanup
        logger.info("Device " .. device_storage_key .. " is already present")
        -- ensure it exists
        local device = core.get_device(dev_id)
        local siren = loadfile("HUB:siren/scripts/device/siren")()
        local is_good =
            device ~= nil and device.category == siren.category and device.device_type_id == siren.device_type_id
        if not is_good then
            logger.error("Existing device has different required field values. Clean up and recreate!")

            device_cleanup(main_item_name, dev_id)

            return add_device_if_needed(parent_dev_id, main_item_name)
        end
    end

    return dev_id
end

local function add_item_if_needed(dev_id, item_name, index)
    local item_storage_key = storage_keys_getter.get_item_key(item_name, index)
    local item_id = storage.get_string(item_storage_key)

    if item_id == nil then
        item_id = loadfile("HUB:siren/scripts/items/create_item")(dev_id, item_name, index)
        storage.set_string(item_storage_key, item_id)
    end
    return item_id
end

local function startup_main()
    local playback_dev_id = add_device_if_needed(nil, item_names.switch)
    logger.trace("add_device_if_needed returned: ", playback_dev_id)

    local item_ids = {}
    logger.trace_dump(item_names, "item_names")

    for _, item_name in pairs(item_names) do
        logger.trace("Looping item_names: " .. item_name)
        local item_id = add_item_if_needed(playback_dev_id, item_name)
        item_ids[item_name] = item_id
        logger.trace("add_item_if_needed(" .. playback_dev_id .. ", " .. item_name .. ") returned: ", item_id)
    end

    -- in case of previous crash, reset storage command_id to null and switch item value to false ('stop')
    local key_command_id = storage_keys_getter.get_speaker_command_id()
    if storage.exists(key_command_id) then
        local command_id = storage.get_string(key_command_id)
        storage.delete(key_command_id)

        local status = pcall(speaker.stop, command_id)
        if not status then
            logger.error("startup.lua: could not stop speaker command: ", command_id)
        end
    end

    local switch_enum = require("HUB:siren/scripts/items/switch_enum")

    local item_id = item_ids[item_names.switch]
    local item_value = core.get_item(item_id).value
    if item_value == switch_enum.play then
        core.update_item_value(item_id, switch_enum.stop)
    end

    -- TODO: clarify is this needed or not
    core.update_reachable_state(playback_dev_id, true)
end

local status, retVal = pcall(startup_main)
if not status then
    logger.error("FINISH: startup.lua failed: ", retVal)
    return status, retVal
else
    logger.info("FINISH: startup.lua succeeded")
    return true
end
