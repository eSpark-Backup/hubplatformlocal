-- Fibaro FGT-001

local self, node_id, device_info = ...

self:_init_base("default", "HUB:zwave/scripts/model/devices/monitoring", node_id, device_info)

self:_register("271_4865_4097", "HUB:zwave/scripts/model/devices/monitoring")
