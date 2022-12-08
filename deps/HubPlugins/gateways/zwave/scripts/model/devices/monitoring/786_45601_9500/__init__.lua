local self, node_id, device_info = ...

self:_init_base("multichannel_switch", "HUB:zwave/scripts/model/devices/monitoring", "default", node_id, device_info)

self:_register("786_45601_9500", "HUB:zwave/scripts/model/devices/monitoring")
