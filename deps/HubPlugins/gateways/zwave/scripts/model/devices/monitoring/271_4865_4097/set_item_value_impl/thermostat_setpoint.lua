local self, item_descriptor, value = ...

local info = self:call("set_item_value_impl/_thermostat_setpoint_set", item_descriptor, value)

self:call("request_setpoints_value", info.node_id, info.channel_id, info.setpoints, 2000)
