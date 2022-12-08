-- Yale NF-YRD612 PBDB

local self, device_info = ...

self:_init_base("yale_door_lock_virtual_restrictions", "HUB:zwave/scripts/model/devices/inclusion", device_info)

self:_register("297_33034_56577", "HUB:zwave/scripts/model/devices/inclusion")
