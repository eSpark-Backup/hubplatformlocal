local self, node_id, device_info = ...

self:_init_base("delayed_color_switch_device", "HUB:zwave/scripts/model/devices/monitoring", node_id, device_info)

self:_register("104_3_11", "HUB:zwave/scripts/model/devices/monitoring")
