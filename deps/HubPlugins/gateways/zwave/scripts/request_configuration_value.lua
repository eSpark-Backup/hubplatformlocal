local params = ...

local zwave = require "zwave"
if not zwave then
    print("Fail to load the zwave functionality")
    return
end

zwave.request_configuration_value(params.node_id, params.param_number)
