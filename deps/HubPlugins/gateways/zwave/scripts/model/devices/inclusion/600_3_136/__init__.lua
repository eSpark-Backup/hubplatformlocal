-- NEO Coolcam\Zipato NE-NAS-AB02Z

local self, device_info = ...

self:_init_base("default", "HUB:zwave/scripts/model/devices/inclusion", device_info)

self:_register("600_3_136", "HUB:zwave/scripts/model/devices/inclusion")
