local item = ...
local item_names = require("HUB:zwave/scripts/resources/item_names")
-- Thermostat Fan Mode
local commands = {
    [item_names.thermostat_fan_state] = {"THERMOSTAT_FAN_MODE_SET"},
}

return commands[item.descriptor.item_name]
