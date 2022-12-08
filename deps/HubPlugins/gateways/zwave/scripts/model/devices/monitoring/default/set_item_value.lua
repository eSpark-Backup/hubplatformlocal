local self, item_id, item_descriptor, value = ...

local setter = self:get_method("set_item_value_impl/" .. item_descriptor.item_name) or
               self:get_method("set_item_value_impl/default")

setter(self, item_descriptor, value, item_id)
