local self, cc_data, node_id, channel_id, value, target_value = ...

local cc_ids = require("HUB:zwave/scripts/resources/class_ids")

local cc_id = cc_data and cc_data.id

if cc_id == cc_ids.sensor_binary and cc_data.sensor_type then
    self:call("update_binary_sensor_value", node_id, channel_id, cc_id, cc_data.sensor_type, value)
elseif cc_id == cc_ids.alarm_sensor and cc_data.sensor_type then
    self:call("update_alarm_sensor_value", node_id, channel_id, cc_id, cc_data.sensor_type, value)
elseif cc_id == cc_ids.dimmer then
    self:call("update_multilevel_switch", node_id, channel_id, cc_id, value, target_value)
elseif cc_id == cc_ids.switch_binary then
    self:call("update_value", node_id, channel_id, cc_id, value)
else
    return false
end

return true
