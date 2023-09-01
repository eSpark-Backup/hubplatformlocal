local params = ...

if not params then
    print("Parametrs is empty")
    return
end

require ("HUB:zigbee/scripts/helpers/scripts_preload")

local storage = hubplugins.object("storage_helper", "HUB:zigbee/scripts/utils")
local zigbee = require("zigbee")

local node_id = storage:call("find_node_id", params.device_id)
if not node_id then
    print( "Node id not found with id: " .. params.device_id)
    return
end

local node = zigbee.get_node(node_id)

local dev_handler = hubplugins.weak_loadfile("HUB:zigbee/scripts/helpers/get_device_handler_monitoring")(node)

dev_handler:stop_fast_poll(node)
