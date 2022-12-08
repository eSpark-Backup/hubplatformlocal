-- AEOTEC Door/Window Sensor 6 ZW112-A

local self, node_id, device_info = ...

self:_init_base("default", "HUB:zwave/scripts/model/devices/monitoring", node_id, device_info)

self:_register("134_258_112", "HUB:zwave/scripts/model/devices/monitoring")
