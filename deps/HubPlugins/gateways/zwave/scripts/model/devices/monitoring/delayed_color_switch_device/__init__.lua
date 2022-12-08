local self, node_id, device_info = ...

self:_init_base("default", "HUB:zwave/scripts/model/devices/monitoring", node_id, device_info)

self:_register("delayed_color_switch_device", "HUB:zwave/scripts/model/devices/monitoring")
