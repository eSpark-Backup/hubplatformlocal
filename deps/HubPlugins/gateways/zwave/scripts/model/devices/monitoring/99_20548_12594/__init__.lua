local self, node_id, device_info = ...

self:_init_base("multichannel_switch", "HUB:zwave/scripts/model/devices/monitoring", "dimmable_light", node_id, device_info)

self:_register("99_20548_12594", "HUB:zwave/scripts/model/devices/monitoring")
