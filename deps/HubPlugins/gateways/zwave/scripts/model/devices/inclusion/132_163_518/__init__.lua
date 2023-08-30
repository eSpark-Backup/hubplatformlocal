-- Fortrezz Flood sensor FTS05

local self, device_info = ...

self:_init_base("fortrezz_flood_sensor", "HUB:zwave/scripts/model/devices/inclusion", device_info)

self:_register("132_163_518", "HUB:zwave/scripts/model/devices/inclusion")
