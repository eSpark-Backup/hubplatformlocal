-- Yale door locks

local self, device_info = ...

self:_init_base("default", "HUB:zwave/scripts/model/devices/inclusion", device_info)

self:_register("yale_door_lock_virtual_restrictions", "HUB:zwave/scripts/model/devices/inclusion")
