local self, node_id, device_info = ...

self:_init_base("shutter_handler", "HUB:zwave/scripts/model/devices/monitoring", node_id, device_info)

self:_register("622_17235_23089", "HUB:zwave/scripts/model/devices/monitoring")
