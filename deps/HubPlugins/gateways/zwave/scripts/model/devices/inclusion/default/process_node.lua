local self, zwave_node = ...


local inclusion_context = {
    -- zwave_node
    -- device_tree_model
    -- processed_command_class_ids
    -- nodes_to_merge
}

--[[
processed_command_class_ids {
    [channel_id] = {
        [class_id] = true,
        ...
    },
    ...
}
]]

self:call("preprocess_node", zwave_node)

print("memory_consumption: process node 1: " .. collectgarbage("count")*1024)

inclusion_context.zwave_node = zwave_node
inclusion_context.device_tree_model = self:call("create_device_tree_model", inclusion_context)
inclusion_context.processed_command_class_ids = {}
inclusion_context.nodes_to_merge = {}

print("memory_consumption: process node 2: " .. collectgarbage("count")*1024)

self:call("build_tree", inclusion_context)
hubplugins.try_collectgarbage()

print("memory_consumption: process node 3: " .. collectgarbage("count")*1024)

self:call("merge_tree", inclusion_context)
hubplugins.try_collectgarbage()

print("memory_consumption: process node 4: " .. collectgarbage("count")*1024)

hubplugins.weak_loadfile("HUB:zwave/scripts/helpers/validate_device_info")({
    tree_model = inclusion_context.device_tree_model,
    zwave_node = inclusion_context.zwave_node
})

self:call("ensure_device_names_unique", inclusion_context)
hubplugins.try_collectgarbage()

print("memory_consumption: process node 5: " .. collectgarbage("count")*1024)

return inclusion_context
