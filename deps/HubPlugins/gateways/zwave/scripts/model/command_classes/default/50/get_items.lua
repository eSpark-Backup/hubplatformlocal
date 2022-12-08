local self, inclusion_context, build_tree_context, device_node = ...

local command_class = build_tree_context.command_class
local meter_info = hubplugins.weak_require("HUB:zwave/scripts/resources/meter_info")[command_class.type]

if not meter_info then
    error("Invalid meter type " .. command_class.type)
end

local items = {}
for _, scale in ipairs(command_class.scales) do
    local meter_item_info = meter_info[scale]
    if meter_item_info then
        local item = self:call("create_item", inclusion_context, build_tree_context, device_node, meter_item_info.item_name)

        if meter_item_info.ui_scale then
            if type(item.value) == "table" then
                item.value.scale = meter_item_info.ui_scale
            else
                print("Item value is of incompatible type " .. type(item.value))
            end
        end

        items[#items + 1] = item

        hubplugins.try_collectgarbage()
    else
        print("Unknown meter scale " .. scale .. " of meter " .. command_class.type)
    end
end

if command_class.reset_supported then
    local item_names = require("HUB:zwave/scripts/resources/item_names")

    local item = self:call("create_item", inclusion_context, build_tree_context, device_node, item_names.meter_reset)
    items[#items + 1] = item
end

return items
