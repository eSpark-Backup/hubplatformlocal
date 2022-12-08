local self, node_id, parameter_number = ...

local zwave = require("zwave")

zwave.request_configuration_value(node_id, parameter_number)
