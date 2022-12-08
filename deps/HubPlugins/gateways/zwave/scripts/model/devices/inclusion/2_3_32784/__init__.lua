-- Danfoss RS Radiator

local self, device_info = ...

self:_init_base("default", "HUB:zwave/scripts/model/devices/inclusion", device_info)

self:_register("2_3_32784", "HUB:zwave/scripts/model/devices/inclusion")
