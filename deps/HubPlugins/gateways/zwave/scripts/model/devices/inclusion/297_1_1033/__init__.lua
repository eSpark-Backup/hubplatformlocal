-- Yale YRL220 Doorlock

local self, device_info = ...

self:_init_base("yale_door_lock_virtual_restrictions", "HUB:zwave/scripts/model/devices/inclusion", device_info)

self:_register("297_1_1033", "HUB:zwave/scripts/model/devices/inclusion")
