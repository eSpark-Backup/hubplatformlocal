local self, inclusion_context, build_tree_context, device_node = ...

local cc = build_tree_context.command_class

if not cc.buttons then
    print("[default.91.process_inclusion_impl] no supported buttons")
    return
end

local item_names = require("HUB:zwave/scripts/resources/item_names")
local button_states = hubplugins.weak_require("HUB:zwave/scripts/resources/enums/button_actions")

local items = {}
for _, button in ipairs(cc.buttons) do
    local item = self:call(
        "create_item",
        inclusion_context,
        build_tree_context,
        device_node,
        item_names.button_state,
        tostring(button.button_id)
    )

    item.enum = { button_states.idle }
    for _, attribute in ipairs(button.button_attributes) do
        table.insert(item.enum, attribute)
    end

    item.value = {
        button_number = button.button_id,
        button_state = button_states.idle
    }
    table.insert(items, item)

    hubplugins.try_collectgarbage()
end

self:call("add_simple_items", inclusion_context, build_tree_context, device_node, items)
