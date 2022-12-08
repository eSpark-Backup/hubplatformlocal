-- Switch multilevel

local self = ...

self:_init_base("dimmable_light/38", "HUB:zwave/scripts/model/command_classes")

self:_register("shutter/38", "HUB:zwave/scripts/model/command_classes")
