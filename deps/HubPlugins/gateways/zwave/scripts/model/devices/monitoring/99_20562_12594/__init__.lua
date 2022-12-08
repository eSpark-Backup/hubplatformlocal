local self, node_id, device_info = ...

self:_init_base("multichannel_switch", "HUB:zwave/scripts/model/devices/monitoring", "default", node_id, device_info)

self:_register("99_20562_12594", "HUB:zwave/scripts/model/devices/monitoring")
