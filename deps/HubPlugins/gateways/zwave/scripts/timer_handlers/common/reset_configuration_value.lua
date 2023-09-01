local params = ...

require("HUB:zwave/scripts/helpers/scripts_preload")

local zwave = assert(require("zwave"), "Fail to load the 'zwave' functionality")

if params.value ~= nil then
    zwave.set_configuration_value(params.node_id, params.number, params.size, params.value)
else
    zwave.reset_configuration_value(params.node_id, params.number)
end
