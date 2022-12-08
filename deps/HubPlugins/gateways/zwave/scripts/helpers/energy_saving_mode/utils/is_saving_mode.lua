local self, mode = ...

local mode_enum = hubplugins.weak_require("HUB:zwave/scripts/resources/enums/thermostat_energy_saving_mode")

return mode == mode_enum.low or
       mode == mode_enum.moderate or 
       mode == mode_enum.aggressive
