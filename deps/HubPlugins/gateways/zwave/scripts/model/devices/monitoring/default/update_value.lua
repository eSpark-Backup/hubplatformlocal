local self, node_id, channel_id, class_id, value, target_value, duration = ...

local class_ids = require("HUB:zwave/scripts/resources/class_ids")
local item_names = require("HUB:zwave/scripts/resources/item_names")

local class_id_to_item_name = {
    [class_ids.switch_binary] = item_names.switch,
    [class_ids.thermostat_setpoint] = item_names.thermostat_setpoint,
    [class_ids.thermostat_fan_mode] = item_names.thermostat_fan_mode,
    [class_ids.battery] = item_names.battery,
    [class_ids.barrier_operator] = item_names.barrier_state
}


local item_name = class_id_to_item_name[class_id]
if not item_name then
    print( ("[handler.update_value]: can't map class id to item name, id: 0x%X"):format(class_id))
    return
end

local item_id = self:call("find_item_id", item_name, node_id, channel_id, class_id)

if not item_id then -- support basic CC
    class_id = class_ids.basic
    item_id = self:call("find_item_id", item_name, node_id, channel_id, class_id)
end

if not item_id then
    print("[handler.update_item_value]: ERROR: " .. self:call("create_item_id_not_found_error_message", item_name, node_id, channel_id, class_id))
    return
end


local function update_request_value_timer(should_start_timer, delay)
    local request_value_timer = hubplugins.object("request_value_timer", "HUB:zwave/scripts/timer_handlers", node_id, channel_id, class_id)

    if should_start_timer then
        request_value_timer:call("start", math.max(delay, 1000))
    else
        request_value_timer:call("stop")
    end
end

local result_value

if item_name == item_names.thermostat_fan_mode then
    local fan_mode_enum = hubplugins.weak_require("HUB:zwave/scripts/resources/enums/thermostat_fan_mode")
    result_value = fan_mode_enum.value_to_token[value]

elseif item_name == item_names.thermostat_fan_state then
    local fan_state_enum = hubplugins.weak_require("HUB:zwave/scripts/resources/enums/thermostat_fan_state")
    result_value = fan_state_enum.value_to_token[value]

elseif item_name == item_names.thermostat_setpoint then
    local thermostat_setpoint_set_timer = hubplugins.object("thermostat_setpoint_set_timer", "HUB:zwave/scripts/timer_handlers", node_id, channel_id)
    thermostat_setpoint_set_timer:call("stop")
    result_value = value

elseif item_name == item_names.switch then
    if target_value ~= nil then
        update_request_value_timer(target_value ~= value, duration or 3000)
    end

    result_value = value ~= 0

elseif item_name == item_names.battery then
    local battery_level = hubplugins.weak_require("HUB:zwave/scripts/resources/battery_level")
    result_value = (value == 0xFF) and battery_level.low or value

elseif item_name == item_names.barrier_state then
    local barrier_enum = hubplugins.weak_require("HUB:zwave/scripts/resources/enums/barrier_event")

    update_request_value_timer(value == barrier_enum.operating_opening or value == barrier_enum.operating_closing, 4000)

    result_value = value

else
    result_value = value
end

self:call("update_value_in_core", item_name, item_id, result_value)
