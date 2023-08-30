-- Fortrezz FTS05 Flood and Temperature Sensor

local self, node_id, device_info = ...

self:_init_base("default", "HUB:zwave/scripts/model/devices/monitoring", node_id, device_info)

self:_register("132_99_414", "HUB:zwave/scripts/model/devices/monitoring")
