local params = ...

require("HUB:zwave/scripts/helpers/scripts_preload")
local handler = hubplugins.weak_loadfile("HUB:zwave/scripts/helpers/get_device_handler_monitoring")(params.node_id)

local storage = hubplugins.object("storage_helper", "HUB:zwave/scripts/utils")
local timer = require("timer")

local thermostat_mode_update_timer = hubplugins.object("thermostat_mode_update_timer", "HUB:zwave/scripts/timer_handlers", params.node_id, params.channel_id)

if  not thermostat_mode_update_timer:call("exists") and params.item_descriptor then
    local thermostat_setpoint_set_timer = hubplugins.object("thermostat_setpoint_set_timer", "HUB:zwave/scripts/timer_handlers", params.node_id, params.channel_id)
    thermostat_setpoint_set_timer:call("stop")

    local info = handler:call("set_item_value_impl/_thermostat_setpoint_set", params.item_descriptor, params.value)
    if info ~= nil then
        handler:call("request_setpoints_value", info.node_id, info.channel_id, info.setpoints, info.delay)
    end
end

params.counter = params.counter + 1
if params.counter >= params.max_counter  then
    thermostat_mode_update_timer:call("stop")
end
