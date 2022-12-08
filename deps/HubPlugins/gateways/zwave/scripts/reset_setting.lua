local params = ...

require("HUB:zwave/scripts/helpers/scripts_preload")

local core = require("core")
local storage = hubplugins.object("storage_helper", "HUB:zwave/scripts/utils")

-- Note: we may have multiple settings for the same parameter, this ensures that we don't send the reset request twice
local unique_parameter_numbers = {}
local function should_reset_configuration(number)
    if unique_parameter_numbers[number] == nil then
        unique_parameter_numbers[number] = true
        return true
    end

    return false
end

if params.setting_id ~= nil then
    local descriptor = storage:call("find_setting_descriptor", params.setting_id)
    local dev_handler = hubplugins.weak_loadfile("HUB:zwave/scripts/helpers/get_device_handler_monitoring")(descriptor.node_id)
    local class_ids = require("HUB:zwave/scripts/resources/class_ids")

    if descriptor.class_id == class_ids.configuration then
        dev_handler:call("reset_configuration_setting", params.setting_id, descriptor.node_id, should_reset_configuration)
    else
        dev_handler:call("reset_specific_setting", params.setting_id, descriptor)
    end
    
elseif params.device_id ~= nil then
    local node_id = storage:call("find_node_id", params.device_id)
    local dev_handler = hubplugins.weak_loadfile("HUB:zwave/scripts/helpers/get_device_handler_monitoring")(node_id)
    
    dev_handler:call("reset_all_device_settings", params.device_id, node_id, should_reset_configuration)
end
