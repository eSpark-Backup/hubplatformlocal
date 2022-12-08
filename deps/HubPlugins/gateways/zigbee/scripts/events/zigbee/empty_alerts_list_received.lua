local params = ...

local zigbee = assert(require "zigbee", "Fail to load zigbee functionality")
local node = assert(zigbee.get_node(params.node_id), "Node not found by id: " .. params.node_id)

local dev_handler = hubplugins.weak_loadfile("HUB:zigbee/scripts/helpers/get_device_handler_monitoring")(node)

dev_handler:empty_alerts_list_received(
    node, 
    params.cluster_name, 
    params.command_id,
    params.command_name,
    params.endpoint_id
)
