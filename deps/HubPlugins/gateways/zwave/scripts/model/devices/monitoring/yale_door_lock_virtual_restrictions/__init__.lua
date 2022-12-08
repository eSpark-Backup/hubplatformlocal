-- Yale Doorlock with virtual pin code restrictions

local self, node_id, device_info = ...

self:_init_base("generic_doorlock", "HUB:zwave/scripts/model/devices/monitoring", node_id, device_info)

self:_register("yale_door_lock_virtual_restrictions", "HUB:zwave/scripts/model/devices/monitoring")
