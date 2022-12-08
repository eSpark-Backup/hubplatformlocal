-- NuTone TZ45 Thermostat

local self, node_id, device_info = ...

self:_init_base("default", "HUB:zwave/scripts/model/devices/monitoring", node_id, device_info)

self:_register("16_1_9", "HUB:zwave/scripts/model/devices/monitoring")
