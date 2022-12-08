-- Aeon Labs Door/Window Sensor Gen5

local self, node_id, device_info = ...

self:_init_base("default", "HUB:zwave/scripts/model/devices/monitoring", node_id, device_info)

self:_register("134_2_29", "HUB:zwave/scripts/model/devices/monitoring")
