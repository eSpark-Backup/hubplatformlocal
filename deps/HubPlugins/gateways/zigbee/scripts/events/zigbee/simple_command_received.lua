local params = ...

local zigbee = assert(require "zigbee", "Fail to load zigbee functionality")
local node = assert(zigbee.get_node(params.node_id), "Node not found by id: " .. params.node_id)

local dev_handler = hubplugins.weak_loadfile("HUB:zigbee/scripts/helpers/get_device_handler_monitoring")(node)

dev_handler:simple_command_received(node, params.endpoint_id, params.cluster_name, params.command_id, params.command_name)
