local self, node_id, device_info = ...

self:_init_base("default", "HUB:zwave/scripts/model/devices/monitoring", node_id, device_info)

self:_register("1041_1_8465", "HUB:zwave/scripts/model/devices/monitoring")
