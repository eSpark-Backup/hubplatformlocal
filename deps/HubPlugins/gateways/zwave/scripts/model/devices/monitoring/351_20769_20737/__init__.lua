-- MCO HOME Touch Panel Switch MH-S512

local self, node_id, device_info = ...

self:_init_base("default", "HUB:zwave/scripts/model/devices/monitoring", node_id, device_info)

self:_register("351_20769_20737", "HUB:zwave/scripts/model/devices/monitoring")
