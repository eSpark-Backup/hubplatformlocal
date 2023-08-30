local self, node_id, channel_id, class_id, button_number, button_state = ...

local core = require("core")
local item_names = require("HUB:zwave/scripts/resources/item_names")
local button_states = hubplugins.weak_require("HUB:zwave/scripts/resources/enums/button_actions")


local item_id = self:call("find_item_id", item_names.button_state, node_id, channel_id, class_id, button_number)
assert(item_id, "[handler.update_activated_scene_value]: " ..
        self:call("create_item_id_not_found_error_message", item_names.button_state, node_id, channel_id, class_id, button_number))

local item = core.get_item(item_id)

local released_supported = false
for _, enum_value in ipairs(item.enum) do
    if enum_value == button_states.released then
        released_supported = true
        break
    end
end

self:call("update_value_in_core",
    item_names.button_state,
    item_id,
    { button_number = button_number, button_state = button_state }
)

if button_state ~= button_states.held_down or not released_supported then
    self:call("update_value_in_core",
        item_names.button_state,
        item_id,
        { button_number = button_number, button_state = button_states.idle }
    )
end
