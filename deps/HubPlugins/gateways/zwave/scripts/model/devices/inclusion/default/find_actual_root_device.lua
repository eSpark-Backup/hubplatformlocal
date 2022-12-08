local self, inclusion_context = ...


local tree_model = inclusion_context.device_tree_model

local root_device = tree_model:call("get_root_node")
if #inclusion_context.zwave_node.channels > 1 then
    local channel_0_node = tree_model:call("get_child", root_device, 0)
    return channel_0_node or root_device
end

local child_device_priority = hubplugins.weak_require("HUB:zwave/scripts/resources/child_device_priority")

local is_priority_device_less = function(lhs, rhs)
    local lhs_category = child_device_priority[lhs.category]
    local rhs_category = child_device_priority[rhs.category]

    if lhs_category.priority > rhs_category.priority then
        return true
    elseif lhs_category.priority == rhs_category.priority then
        local lhs_subcategory = lhs_category.subcategory[lhs.subcategory]
        local rhs_subcategory = rhs_category.subcategory[rhs.subcategory]

        return lhs_subcategory and rhs_category and (lhs_subcategory > rhs_subcategory)
    end

    return false
end

local new_root = root_device

local callback = function(dev_node)
    if new_root == root_device or is_priority_device_less(tree_model:call("get_data", new_root).info, tree_model:call("get_data", dev_node).info) then
        new_root = dev_node
    end
end
tree_model:call("for_each_child", root_device, callback)

return new_root
