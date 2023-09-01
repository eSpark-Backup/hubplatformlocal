local self, node_id, parameter_number, value = ...

local handler = self:get_method("handle_configuration_update_impl/" .. math.floor(parameter_number))
if handler ~= nil then
    handler(self, node_id, value)
end
