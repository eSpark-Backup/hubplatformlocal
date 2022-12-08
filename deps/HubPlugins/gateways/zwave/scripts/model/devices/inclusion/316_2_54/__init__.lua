-- Philio PSM08-2

local self, device_info = ...

self:_init_base("default", "HUB:zwave/scripts/model/devices/inclusion", device_info)

self:_register("316_2_54", "HUB:zwave/scripts/model/devices/inclusion")
