-- Qubino ZMNHBD3 Flush 2 Relay

local self, node_id, device_info = ...

self:_init_base("default", "HUB:zwave/scripts/model/devices/monitoring", node_id, device_info)

self:_register("345_2_81", "HUB:zwave/scripts/model/devices/monitoring")
