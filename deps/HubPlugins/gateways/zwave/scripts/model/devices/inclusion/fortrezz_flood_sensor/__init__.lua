-- Fortrezz flood sensor common

local self, device_info = ...

self:_init_base("default", "HUB:zwave/scripts/model/devices/inclusion", device_info)

self:_register("fortrezz_flood_sensor", "HUB:zwave/scripts/model/devices/inclusion")
