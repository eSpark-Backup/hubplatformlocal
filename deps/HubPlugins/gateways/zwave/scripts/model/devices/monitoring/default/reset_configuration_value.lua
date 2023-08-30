local self, delay, node_id, number, size, value = ...

local zwave = require("zwave")

if value ~= nil then
    zwave.set_configuration_value(node_id, number, size, value)
else
    zwave.reset_configuration_value(node_id, number)
end

zwave.request_configuration_value(node_id, number)

return delay + 500
