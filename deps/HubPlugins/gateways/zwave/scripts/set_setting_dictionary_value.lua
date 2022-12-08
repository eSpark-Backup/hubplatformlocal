local params = ...

require("HUB:zwave/scripts/helpers/scripts_preload")

local core = assert(require("core"), "Failed to load core functionality")
local storage = hubplugins.object("storage_helper", "HUB:zwave/scripts/utils")

local setting = core.get_setting(params.setting_id)
local setting_descriptor = storage:call("find_setting_descriptor", params.setting_id)
local existing_value = setting.value[params.key]

local node_id = storage:call("find_node_id", setting.device_id)

local dev_handler = hubplugins.weak_loadfile("HUB:zwave/scripts/helpers/get_device_handler_monitoring")(node_id)

if params.value ~= nil then
    dev_handler:call("ensure_setting_dictionary_value", params.setting_id, setting_descriptor, node_id, params.key, params.value)
    dev_handler:call("set_setting_dictionary_value", node_id, setting_descriptor, params.key, existing_value, params.value)
else
    dev_handler:call("remove_setting_dictionary_value", params.setting_id, node_id, setting_descriptor, params.key, existing_value)
end
