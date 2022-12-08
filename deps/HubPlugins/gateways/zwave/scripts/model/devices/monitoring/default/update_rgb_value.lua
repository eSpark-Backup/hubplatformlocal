local self, node_id, channel_id, colors, target_colors, target_durations = ...

local class_id = require("HUB:zwave/scripts/resources/class_ids").switch_color

target_colors = target_colors or {}

local item_names = require("HUB:zwave/scripts/resources/item_names")

local item_id = self:call("find_item_id", item_names.rgb_color, node_id, channel_id, class_id)
if not item_id then
    error("[handler.update_rgb_value]: " .. self:call("create_item_id_not_found_error_message", item_names.rgb_color, node_id, channel_id, class_id))
end

local core = require "core"

local item = core.get_item(item_id)
local result_colors = item.value

local transition_done = true
for component, value in pairs(colors) do
    local target_value = target_colors[component]
    if ( target_value ~= nil and target_value ~= value ) or ( target_value == nil and value ~= result_colors[component] ) then
        transition_done = false
    end

    result_colors[component] = value
end

self:call("update_value_in_core", item_names.rgb_color, item_id, result_colors)

if not transition_done then
    print("color switch: transition is not done, continue requesting")

    local request_value_timer = hubplugins.object("request_value_timer", "HUB:zwave/scripts/timer_handlers", node_id, channel_id, class_id)

    request_value_timer:call("start", 2000)
end

return transition_done

