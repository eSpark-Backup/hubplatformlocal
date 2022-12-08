-- Thermostat setpoint

local self = ...

self:_init_base("default/67", "HUB:zwave/scripts/model/command_classes")

self:_register("2_5_4/67", "HUB:zwave/scripts/model/command_classes")
