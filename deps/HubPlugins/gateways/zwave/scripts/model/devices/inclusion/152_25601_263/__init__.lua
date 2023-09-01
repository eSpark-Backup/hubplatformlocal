-- RadioThermostat CT100

local self, device_info = ...

self:_init_base("default", "HUB:zwave/scripts/model/devices/inclusion", device_info)

self:_register("152_25601_263", "HUB:zwave/scripts/model/devices/inclusion")
