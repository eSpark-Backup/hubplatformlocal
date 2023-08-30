-- Aeotec Multisensor 6 ZW100-A

local self, node_id, device_info = ...

self:_init_base("default", "HUB:zwave/scripts/model/devices/monitoring", node_id, device_info)

self:_register("aeotec_6_in_1_sensor", "HUB:zwave/scripts/model/devices/monitoring")
