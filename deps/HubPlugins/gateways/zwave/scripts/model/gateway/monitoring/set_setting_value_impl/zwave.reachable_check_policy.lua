local self, setting_id, value = ...

self:call("set_setting_value_to_core", setting_id, value)
self:call("set_poll_intervals_to_zwave", value ~= "default")
