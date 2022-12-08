local self, item_descriptor, direction = ...

local zwave = require("zwave")
local timer = require("timer")

local item_names = require("HUB:zwave/scripts/resources/item_names")

local function get_shuttter_id(item_descriptor)
    return self:call("find_item_id",
        item_names.shutter_state,
        item_descriptor.node_id,
        item_descriptor.channel_id,
        item_descriptor.class_id
    )
end

local function is_in_learning_mode(item_descriptor)
    local shutter_states_enum = hubplugins.weak_require("HUB:zwave/scripts/resources/enums/shutter_states")

    local state_item = self:call("find_item",
        item_names.shutter_state,
        item_descriptor.node_id,
        item_descriptor.channel_id,
        item_descriptor.class_id
    )
    assert(state_item, "[shutter_handler.set_value_item] no item with shutter state")

    return state_item.value == shutter_states_enum.learn_upper or state_item.value == shutter_states_enum.learn_lower
end

local function switch_multilevel_level_change_in_learning_mode(item_descriptor, direction)
    local state_item_id = get_shuttter_id(item_descriptor)

    -- Note: in shutter's learning mode SWITCH_MULTILEVEL_START_LEVEL_CHANGE changes the device's state once instead of continuously changing it
    -- To simulate the normal mode behavior use timer to repeatedly send requests

    if direction == "stop" then
        timer.cancel(state_item_id)
    else
        local script_args = {
            node_id = item_descriptor.node_id,
            channel_id = item_descriptor.channel_id,
            command = direction
        }
        timer.set_interval_with_id(400, state_item_id, "HUB:zwave/scripts/timer_handlers/interval/shutter_move_border", script_args)
    end
end

if is_in_learning_mode(item_descriptor) then
    switch_multilevel_level_change_in_learning_mode(item_descriptor, direction)
else
    zwave.switch_multilevel_level_change(item_descriptor.node_id, item_descriptor.channel_id, direction)
    hubplugins.weak_loadfile("HUB:zwave/scripts/request_value")(item_descriptor)
end
