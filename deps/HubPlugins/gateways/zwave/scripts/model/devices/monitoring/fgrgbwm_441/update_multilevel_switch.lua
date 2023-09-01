local self, node_id, channel_id, class_id, value, target_value, target_duration = ...

local transition_done = self:_super_call("delayed_color_switch_device", "update_multilevel_switch", node_id, channel_id, class_id, value, target_value, target_duration)

if transition_done and channel_id >= 2 and channel_id <= 5 then
    local class_ids = require("HUB:zwave/scripts/resources/class_ids")
    local item_names = require("HUB:zwave/scripts/resources/item_names")

    local storage = hubplugins.object("storage_helper", "HUB:zwave/scripts/utils")

    local color_switch_item_id = storage:call("find_item_id", item_names.rgb_color, node_id, class_ids.switch_color, 0)
    assert(color_switch_item_id)

    local core = require "core"
    local color_item = core.get_item(color_switch_item_id)

    local components = { "red", "green", "blue", "wwhite" }
    local component = components[channel_id - 1]
    local color_value = color_item.value[ component ]

    if value ~= self:call("_color_to_multilevel_switch_value", color_value) then
        print("start color timer, switch value " .. value .. " doesn't match existing " .. component .. " color value " .. color_value)

        local request_value_timer = hubplugins.object("request_value_timer", "HUB:zwave/scripts/timer_handlers", node_id, 0, class_ids.switch_color)
        request_value_timer:call("start", 3000)
    end
end

return transition_done