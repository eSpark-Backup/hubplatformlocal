local params = ...

require("HUB:zwave/scripts/helpers/scripts_preload")

local zwave = assert(require("zwave"), "Fail to load the 'zwave' functionality")

if not params.method or params.method == "request_value" then
    zwave.request_value(params.node_id, params.class_id, params.channel_id)
else
    -- TODO: use and test this functionality
    zwave[params.method](params.node_id, params.channel_id)
end
