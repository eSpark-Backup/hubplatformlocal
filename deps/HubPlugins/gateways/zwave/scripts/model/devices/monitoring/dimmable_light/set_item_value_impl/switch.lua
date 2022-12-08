local self, item_descriptor, value, item_id = ...

value = value == false and 0x00 or 0xFF

self:call("set_item_value_impl/dimmer", item_descriptor, value, item_id)
