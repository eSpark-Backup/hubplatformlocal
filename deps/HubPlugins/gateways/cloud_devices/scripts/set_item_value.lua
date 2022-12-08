local params = ...
-- params = {
--     item_id = <core_item_id>
--     value = <new_item_value>
--     operation_id = <request_operation_id>
-- }

local cloud_devices = assert(require "cloud_devices", "Fail to load cloud devices functionality")
local core = assert(require "core", "Fail to load core functionality")
local storage = assert(require "storage", "Fail to load storage functionality")

local storage_keys = require("HUB:cloud_devices/scripts/resources/storage_keys")

local core_item = core.get_item(params.item_id)
local core_device_id = core_item.device_id

if core_item.name == "text_command" then
    local commands_counter = storage.get_number(storage_keys.commands_counter);
    commands_counter = ( commands_counter or 0 ) + 1
    storage.set_number(storage_keys.commands_counter, commands_counter);
    local command_id = tostring(commands_counter)
    local cloud_device_id = storage.get_string(storage_keys.cloud_id_by_core_id_prefix .. core_device_id)

    local status, error_msg = pcall(cloud_devices.send_command, command_id, cloud_device_id, "text_command", "send", { text = params.value })
    if not status then
        print("Error sending command: " .. error_msg)
        return
    end

    local response_info = {
        core_device_id = core_device_id,
        operation_id = params.operation_id
    }
    storage.set_table(storage_keys.response_info_by_command_id_prefix .. command_id, response_info);
    core.update_ready_state(core_device_id, false)
end
