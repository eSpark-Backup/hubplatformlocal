local self, item_descriptor, value = ...

self:call("set_item_value_impl/_thermostat_mode_set", item_descriptor, value)

-- Do not need request value. A thermostat send an actual value automatically
