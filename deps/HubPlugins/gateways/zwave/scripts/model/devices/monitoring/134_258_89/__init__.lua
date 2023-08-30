-- Aeon Labs Recessed Door Sensor Gen5

local self, node_id, device_info = ...

self:_init_base("default", "HUB:zwave/scripts/model/devices/monitoring", node_id, device_info)

self:_register("134_258_89", "HUB:zwave/scripts/model/devices/monitoring")
