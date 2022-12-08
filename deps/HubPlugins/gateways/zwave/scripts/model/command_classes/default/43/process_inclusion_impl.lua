local self, inclusion_context, build_tree_context, device_node = ...

local item_names = require("HUB:zwave/scripts/resources/item_names")
local button_actions = hubplugins.weak_require("HUB:zwave/scripts/resources/enums/button_actions")

local cc = build_tree_context.command_class
if cc.scene_actions == nil then
    print("[default.43.process_inclusion_impl] no info on scene actions")
    return
end

local button_states = {}
for _, scene_info in ipairs(cc.scene_actions) do
    if not button_states[scene_info.button_number] then
        button_states[scene_info.button_number] = { button_actions.idle }
    end

    table.insert(button_states[scene_info.button_number], scene_info.button_state)
end

for button_number, states in pairs(button_states) do
    local item = self:call(
        "create_item",
        inclusion_context,
        build_tree_context,
        device_node,
        item_names.button_state,
        tostring(button_number)
    )

    item.enum = states

    item.value = {
        button_number = button_number,
        button_state = button_actions.idle
    }

    self:call("add_item", inclusion_context, build_tree_context, device_node, item)
end
