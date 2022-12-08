local self, inclusion_context, parent_device_node, internal_device_name = ...

local dev_info =
    assert(hubplugins.weak_loadfile("HUB:zwave/scripts/resources/child_devices_info/" .. internal_device_name),
        "need define child device info for item: " .. internal_device_name)


local current_dev_node = inclusion_context.device_tree_model:call("insert_child", parent_device_node, internal_device_name, dev_info())

return current_dev_node