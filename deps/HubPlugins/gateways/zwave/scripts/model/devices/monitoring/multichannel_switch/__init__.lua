local self, base, node_id, device_info = ...

self:_init_base(base, "HUB:zwave/scripts/model/devices/monitoring", node_id, device_info)

self:_register("multichannel_switch", "HUB:zwave/scripts/model/devices/monitoring")

self.base = base