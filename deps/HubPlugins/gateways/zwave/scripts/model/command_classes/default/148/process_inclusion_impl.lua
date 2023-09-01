local self, inclusion_context, build_tree_context, device_node = ...

local item_names = require("HUB:zwave/scripts/resources/item_names")
local button_actions = hubplugins.weak_require("HUB:zwave/scripts/resources/enums/button_actions")

local cc = build_tree_context.command_class
if cc.simple_av_control_actions == nil then
    print("[default.148.process_inclusion_impl] no info on simple av control actions")
    return
end

for _, action in ipairs(cc.simple_av_control_actions) do
    local item = self:call(
        "create_item",
        inclusion_context,
        build_tree_context,
        device_node,
        item_names.button_state,
        tostring(action.button_number)
    )

    item.enum = {
        button_actions.idle,
        button_actions.press_1_time,
        button_actions.held_down,
        button_actions.released
    }

    item.value = {
        button_number = action.button_number,
        button_state = button_actions.idle
    }

    self:call("add_item", inclusion_context, build_tree_context, device_node, item)
end
