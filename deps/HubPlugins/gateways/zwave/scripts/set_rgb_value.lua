local params = ...

local zwave = require "zwave"
if not zwave then
    print("Fail to load the zwave functionality")
    return
end

zwave.set_rgb_value(params.node_id, params.class_id, params.channel_id, params.value)

