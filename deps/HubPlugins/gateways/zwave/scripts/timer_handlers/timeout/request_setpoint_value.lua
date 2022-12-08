local params = ...

require("HUB:zwave/scripts/helpers/scripts_preload")

local zwave = assert(require("zwave"), "Fail to load the 'zwave' functionality")

zwave.request_setpoint_value(params.node_id, params.channel_id, params.type)
