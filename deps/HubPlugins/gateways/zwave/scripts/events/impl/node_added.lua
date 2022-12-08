local params = ...

print("Adding node with id: " .. params.node_id)

print("memory_consumption: node added 1: " .. collectgarbage("count")*1024)

local zwave = require("zwave")
print("memory_consumption: node added 2: " .. collectgarbage("count")*1024)

local node = zwave.get_node(params.node_id)

print("Node structure: " .. hubplugins.dump(node))
print("memory_consumption: node added 3: " .. collectgarbage("count")*1024)

do
    local dev_handler = hubplugins.weak_loadfile("HUB:zwave/scripts/helpers/get_device_handler_inclusion")(node)
    print("memory_consumption: node added 4: " .. collectgarbage("count")*1024)

    local inclusion_context = dev_handler:call("process_node", node)
    print("memory_consumption: node added 5: " .. collectgarbage("count")*1024)

    dev_handler:call("add_tree_to_core", inclusion_context)
    print("memory_consumption: node added 6: " .. collectgarbage("count")*1024)

    zwave.request_node_values(
        tostring(params.node_id), 
        dev_handler.device_info.request_value_delay, 
        hubplugins.weak_loadfile("HUB:zwave/scripts/helpers/get_unset_pin_codes_count_to_stop_requesting")(dev_handler.device_info), 
        true
    )
    print("memory_consumption: node added 7: " .. collectgarbage("count")*1024)
end

do
    hubplugins.weak_loadfile("HUB:zwave/scripts/helpers/start_daemon")(
            params.node_id, node.manufacturer_id, node.product_type, node.product_id)

    print("memory_consumption: node added 8: " .. collectgarbage("count")*1024)
end
