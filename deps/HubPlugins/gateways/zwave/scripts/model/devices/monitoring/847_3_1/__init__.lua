-- Delaney doorlock

local self, node_id, device_info = ...

self:_init_base("generic_doorlock", "HUB:zwave/scripts/model/devices/monitoring", node_id, device_info)

self:_register("847_3_1", "HUB:zwave/scripts/model/devices/monitoring")
