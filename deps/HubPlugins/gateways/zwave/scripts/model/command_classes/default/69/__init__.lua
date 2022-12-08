-- Thermostat fan state

local self = ...

self:_init_base("default/base_command_class", "HUB:zwave/scripts/model/command_classes")

self:_register("default/69", "HUB:zwave/scripts/model/command_classes")
