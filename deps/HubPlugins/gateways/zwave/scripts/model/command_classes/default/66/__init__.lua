-- Thermostat operating state

local self = ...

self:_init_base("default/base_command_class", "HUB:zwave/scripts/model/command_classes")

self:_register("default/66", "HUB:zwave/scripts/model/command_classes")
