-- Yale YRD220-ZW Doorlock

local self, node_id, device_info = ...

self:_init_base("generic_doorlock", "HUB:zwave/scripts/model/devices/monitoring", node_id, device_info)

self:_register("265_1_0", "HUB:zwave/scripts/model/devices/monitoring")
