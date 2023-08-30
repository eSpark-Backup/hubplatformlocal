-- Fibaro FGSS-001 Smoke Sensor

local self, node_id, device_info = ...

self:_init_base("default", "HUB:zwave/scripts/model/devices/monitoring", node_id, device_info)

self:_register("271_3072_4096", "HUB:zwave/scripts/model/devices/monitoring")
