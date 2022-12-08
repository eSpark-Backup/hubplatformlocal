local self, item_descriptor, value = ...

local timer = require("timer")
local storage = hubplugins.object("storage_helper", "HUB:zwave/scripts/utils")

value.value = hubplugins.weak_loadfile("HUB:zwave/scripts/utils/round_value")(value.value, 1)

local thermostat_mode_update_timer = hubplugins.object("thermostat_mode_update_timer", "HUB:zwave/scripts/timer_handlers", item_descriptor.node_id, item_descriptor.channel_id)
if  thermostat_mode_update_timer:call("exists") then

    local thermostat_setpoint_set_timer = hubplugins.object("thermostat_setpoint_set_timer", "HUB:zwave/scripts/timer_handlers", item_descriptor.node_id, item_descriptor.channel_id)
    thermostat_setpoint_set_timer:call("start", item_descriptor, value)

else
    local info = self:call("set_item_value_impl/_thermostat_setpoint_set", item_descriptor, value)
    if info ~= nil then
        self:call("request_setpoints_value", info.node_id, info.channel_id, info.setpoints, info.delay)
    end
end
