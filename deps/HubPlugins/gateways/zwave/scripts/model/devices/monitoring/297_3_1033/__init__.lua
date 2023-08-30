-- Yale YRL210

local self, node_id, device_info = ...

self:_init_base("yale_door_lock_virtual_restrictions", "HUB:zwave/scripts/model/devices/monitoring", node_id, device_info)

self:_register("297_3_1033", "HUB:zwave/scripts/model/devices/monitoring")
