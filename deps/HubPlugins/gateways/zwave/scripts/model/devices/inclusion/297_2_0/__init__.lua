-- Yale YRD220-ZW

local self, device_info = ...

self:_init_base("yale_door_lock_virtual_restrictions", "HUB:zwave/scripts/model/devices/inclusion", device_info)

self:_register("297_2_0", "HUB:zwave/scripts/model/devices/inclusion")
