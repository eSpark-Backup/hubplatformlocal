local params = ...

local zwave = require "zwave"
if not zwave then
    print("Fail to load the zwave functionality")
    return
end

if params.default ~= nil then
    zwave.reset_configuration_value(params.node_id, params.param_number)
else
    zwave.set_configuration_value(params.node_id, params.param_number, params.param_size, params.value)
end
