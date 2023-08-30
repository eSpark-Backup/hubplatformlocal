local self, node_id, device_info = ...

self:_init_base("schlage_doorlock", "HUB:zwave/scripts/model/devices/monitoring", node_id, device_info)

self:_register("59_25409_20548", "HUB:zwave/scripts/model/devices/monitoring")
