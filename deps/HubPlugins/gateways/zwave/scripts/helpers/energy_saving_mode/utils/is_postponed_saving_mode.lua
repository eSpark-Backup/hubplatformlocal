local self, mode = ...

local mode_enum = hubplugins.weak_require("HUB:zwave/scripts/resources/enums/thermostat_energy_saving_mode")

return mode == mode_enum.low_postponed or
       mode == mode_enum.moderate_postponed or 
       mode == mode_enum.aggressive_postponed
