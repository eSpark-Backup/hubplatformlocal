local params = ...

require("HUB:zwave/scripts/helpers/scripts_preload")

local zwave = assert(require("zwave"), "Fail to load the 'zwave' functionality")

zwave.request_configuration_value(params.node_id, params.number)
