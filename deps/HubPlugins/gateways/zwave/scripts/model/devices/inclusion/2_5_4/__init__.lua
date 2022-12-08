-- Danfoss Electronic Radiator Termostat 014G0013

local self, device_info = ...

self:_init_base("default", "HUB:zwave/scripts/model/devices/inclusion", device_info)

self:_register("2_5_4", "HUB:zwave/scripts/model/devices/inclusion")
