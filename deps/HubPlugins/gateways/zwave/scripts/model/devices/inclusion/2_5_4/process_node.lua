local self, zwave_node = ...

local class_ids = require("HUB:zwave/scripts/resources/class_ids")
local mode_enum = require("HUB:zwave/scripts/resources/enums/thermostat_mode")

table.insert(zwave_node.channels[1].classes, {
    id = class_ids.thermostat_mode,
    modes = {mode_enum.token_to_value["heat"]}
})

return self:_super_call("default", "process_node", zwave_node)
