-- POPP Thermostatic Radiator Valve

local self, device_info = ...

self:_init_base("default", "HUB:zwave/scripts/model/devices/inclusion", device_info)

self:_register("2_277_40976", "HUB:zwave/scripts/model/devices/inclusion")
