local self, inclusion_context, build_tree_context, device_node = ...

local item_names = require("HUB:zwave/scripts/resources/item_names")
local class_ids = require("HUB:zwave/scripts/resources/class_ids")

local item = self:call("create_item", inclusion_context, build_tree_context, device_node, item_names.door_lock)

local device_node_data = inclusion_context.device_tree_model:call("get_data", device_node)

local cc = build_tree_context.command_class

if cc.capabilities then
    item.enum = { "unknown" }
    for _, mode in ipairs(cc.capabilities.door_lock_modes) do
        if mode ~= "unknown" then
            table.insert(item.enum, mode)
        end
    end

    if cc.capabilities.door_supported then
        local dw_item = self:call("create_item", inclusion_context, build_tree_context, device_node, item_names.dw_state)
        local dw_enum = hubplugins.weak_require("HUB:zwave/scripts/resources/enums/dw_state_event")
        table.insert(dw_item.enum, dw_enum.dw_is_opened)
        table.insert(dw_item.enum, dw_enum.dw_is_closed)
        self:call("add_item", inclusion_context, build_tree_context, device_node, dw_item)
    end

    if cc.capabilities.latch_supported then
        local latch_item = self:call("create_item", inclusion_context, build_tree_context, device_node, item_names.latch_state)
        self:call("add_item", inclusion_context, build_tree_context, device_node, latch_item)
    end
end

self:call("add_item", inclusion_context, build_tree_context, device_node, item)

-- Add all door lock settings
self:call("process_door_lock_settings", inclusion_context, device_node_data, cc)
