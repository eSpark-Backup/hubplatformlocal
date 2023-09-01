local self, node_id, device_info = ...

self:_init_base("dimmable_light", "HUB:zwave/scripts/model/devices/monitoring", node_id, device_info)

self:_register("shutter_handler", "HUB:zwave/scripts/model/devices/monitoring")
