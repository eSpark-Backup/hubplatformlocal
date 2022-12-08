local item = ...
local item_names = require( "HUB:zwave/scripts/resources/item_names")
-- Thermostat Mode
local commands = {
    [item_names.thermostat_mode] = {"THERMOSTAT_MODE_SET"}
}

return commands[item.descriptor.item_name]
