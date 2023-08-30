local self, item_descriptor, value = ...

value = value == "secured" and "unsecured" or
        value == "unsecured" and "secured" or
        value

self:_super_call("generic_doorlock", "set_item_value_impl/door_lock", item_descriptor, value)
