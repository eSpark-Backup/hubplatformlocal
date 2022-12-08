-- Aeotec ZW116 Nano Switch

local self, node_id, device_info = ...

self:_init_base("default", "HUB:zwave/scripts/model/devices/monitoring", node_id, device_info)

self:_register("134_259_116", "HUB:zwave/scripts/model/devices/monitoring")
