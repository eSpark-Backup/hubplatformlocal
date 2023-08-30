-- MCO Home A-89 9-in-1 sensor

local self, node_id, device_info = ...

self:_init_base("default", "HUB:zwave/scripts/model/devices/monitoring", node_id, device_info)

self:_register("351_43011_4945", "HUB:zwave/scripts/model/devices/monitoring")
