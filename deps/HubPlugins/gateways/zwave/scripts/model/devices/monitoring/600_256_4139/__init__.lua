-- Neo Coolcam Light Switch 3 Buttons

local self, node_id, device_info = ...

self:_init_base("default", "HUB:zwave/scripts/model/devices/monitoring", node_id, device_info)

self:_register("600_256_4139", "HUB:zwave/scripts/model/devices/monitoring")
