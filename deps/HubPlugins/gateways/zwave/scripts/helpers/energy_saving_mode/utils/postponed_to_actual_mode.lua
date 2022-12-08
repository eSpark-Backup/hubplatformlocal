local self, postponed_mode = ...

local mode_enum = hubplugins.weak_require("HUB:zwave/scripts/resources/enums/thermostat_energy_saving_mode")

local map = {
    [mode_enum.low_postponed] = mode_enum.low,
    [mode_enum.moderate_postponed] = mode_enum.moderate,
    [mode_enum.aggressive_postponed] = mode_enum.aggressive
}

return assert(map[postponed_mode])
