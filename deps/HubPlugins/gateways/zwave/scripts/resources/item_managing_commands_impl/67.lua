local item = ...
local item_names = require("HUB:zwave/scripts/resources/item_names")
-- Thermostat Setpoint
local commands = {
    [item_names.thermostat_setpoint] = {"THERMOSTAT_SETPOINT_SET"},
    [item_names.thermostat_setpoint_cooling] = {"THERMOSTAT_SETPOINT_SET"},
    [item_names.thermostat_setpoint_heating] = {"THERMOSTAT_SETPOINT_SET"},
    [item_names.thermostat_energy_saving_mode] = {"THERMOSTAT_SETPOINT_SET"}
}

return commands[item.descriptor.item_name]
