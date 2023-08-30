local params = ...
-- params = {
--     command_id = "<command_id>",
--     result = {...},
--     error = {...}
-- }

local core = assert(require "core", "Fail to load core functionality")
local storage = assert(require "storage", "Fail to load storage functionality")

local storage_keys = require("HUB:cloud_devices/scripts/resources/storage_keys")

local response_info = storage.get_table(storage_keys.response_info_by_command_id_prefix .. params.command_id);
core.update_ready_state(response_info.core_device_id, true)

local result = {}
local error

if next(params.error) ~= nil then
    error = {
        code = 3740,
        data = "custom_error",
        message = "Cloud send command unknown error"
    }

    if params.error.code == -1 then
        error.message = "Cloud send command timeout"
    end
end

if error == nil then
    if params.result and
            params.result.capabilities and
            params.result.capabilities.text_command and
            params.result.capabilities.text_command.variables and
            params.result.capabilities.text_command.variables.text then
        result.text = params.result.capabilities.text_command.variables.text
    else
        error = {
            code = 3740,
            data = "custom_error",
            message = "Incorrect cloud command response result format"
        }
    end
end

core.send_response(response_info.operation_id, result, error)
