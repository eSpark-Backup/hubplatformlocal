local self, inclusion_context, build_tree_context, device_node, item = ...

local item_names = require("HUB:zwave/scripts/resources/item_names")

local root_node = assert(inclusion_context.device_tree_model:call("get_root_node"))
local root_node_data = inclusion_context.device_tree_model:call("get_data", root_node)
if root_node_data.info.panic_triggers and root_node_data.info.panic_button_policy == "separate_device" then
    for _, trigger in ipairs(root_node_data.info.panic_triggers) do
        if (trigger.item_name and trigger.item_name == item.name) or
           (item.name == item_names.button_state and item.value.button_number == trigger.button_number) then
            device_node = assert(inclusion_context.device_tree_model:call("get_child", root_node, "panic_button"))
        end
    end
end

local device_node_data = inclusion_context.device_tree_model:call("get_data", device_node)
table.insert(device_node_data.items, item)
