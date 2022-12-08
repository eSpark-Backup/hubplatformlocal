local params = ...

local zwave = require "zwave"
if not zwave then
    print("Fail to load the zwave functionality")
    return
end
local thermostat_setpoint_cc_id = require("HUB:zwave/scripts/resources/class_ids").thermostat_setpoint

zwave.set_setpoint_value( params.node_id, thermostat_setpoint_cc_id, params.channel_id, params.type, params.value)
