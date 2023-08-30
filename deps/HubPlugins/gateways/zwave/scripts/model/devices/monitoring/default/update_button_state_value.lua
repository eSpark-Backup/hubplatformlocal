local self, node_id, channel_id, class_id, button_id, value = ...

local item_names = require("HUB:zwave/scripts/resources/item_names")
local button_states = hubplugins.weak_require("HUB:zwave/scripts/resources/enums/button_actions")

local item_id = self:call("find_item_id", item_names.button_state, node_id, channel_id, class_id, button_id)
assert(item_id, "[handler.update_button_state_value]: " ..
        self:call("create_item_id_not_found_error_message", item_names.button_state, node_id, channel_id, class_id, button_id))

self:call("update_value_in_core",
    item_names.button_state,
    item_id,
    { button_number = button_id, button_state = value }
)

if value ~= button_states.held_down then
    self:call("update_value_in_core",
        item_names.button_state,
        item_id,
        { button_number = button_id, button_state = button_states.idle }
    )
end
