-- AEOTEC Door/Window Sensor 6 ZW112-A

local self = ...

self:_init_base("default/base_command_class", "HUB:zwave/scripts/model/command_classes")

self:_register("134_258_112/48", "HUB:zwave/scripts/model/command_classes")
