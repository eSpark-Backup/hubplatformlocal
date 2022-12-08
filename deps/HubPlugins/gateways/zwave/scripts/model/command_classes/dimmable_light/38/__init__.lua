-- Switch multilevel

local self = ...

self:_init_base("default/38", "HUB:zwave/scripts/model/command_classes")

self:_register("dimmable_light/38", "HUB:zwave/scripts/model/command_classes")
