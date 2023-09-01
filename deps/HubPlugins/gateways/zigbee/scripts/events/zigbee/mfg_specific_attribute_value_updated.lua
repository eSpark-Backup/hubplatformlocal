local params = ...

print("LUA: Mfg specific attribute value updated: node_id: " .. params.node_id)

local zigbee = assert(require "zigbee", "Fail to load zigbee functionality")
local node = assert(zigbee.get_node(params.node_id), "Node not found by id: " .. params.node_id)

local dev_handler = hubplugins.weak_loadfile("HUB:zigbee/scripts/helpers/get_device_handler_monitoring")(node)

dev_handler:mfg_specific_attribute_value_updated(node, params.endpoint_id, params.cluster_id, params.attribute)
