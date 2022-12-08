-- DSC11 ZWUS

local self, device_info = ...

self:_init_base("default", "HUB:zwave/scripts/model/devices/inclusion", device_info)

self:_register("134_3_11", "HUB:zwave/scripts/model/devices/inclusion")