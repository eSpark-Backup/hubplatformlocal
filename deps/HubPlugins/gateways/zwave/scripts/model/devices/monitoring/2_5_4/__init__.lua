local self, node_id, device_info = ...

self:_init_base("default", "HUB:zwave/scripts/model/devices/monitoring", node_id, device_info)

self:_register("2_5_4", "HUB:zwave/scripts/model/devices/monitoring")
