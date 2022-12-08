-- Yale YRD110 Doorlock

local self, node_id, device_info = ...

self:_init_base("generic_doorlock", "HUB:zwave/scripts/model/devices/monitoring", node_id, device_info)

self:_register("297_4_2048", "HUB:zwave/scripts/model/devices/monitoring")
