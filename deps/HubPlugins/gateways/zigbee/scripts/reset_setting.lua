local params = ...

require ("HUB:zigbee/scripts/helpers/scripts_preload")

local storage = hubplugins.object("storage_helper", "HUB:zigbee/scripts/utils")
local zigbee = require("zigbee")

local descriptor = storage:call("find_setting_descriptor", params.setting_id)
if not descriptor then
    print( "Setting descriptor not found with id: " .. params.setting_id)
    return
end

local node = zigbee.get_node(descriptor.node_id)
local dev_handler = hubplugins.weak_loadfile("HUB:zigbee/scripts/helpers/get_device_handler_monitoring")(node)
dev_handler:reset_setting_value(params.setting_id, descriptor)
