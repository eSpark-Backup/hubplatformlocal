-- Qubino Dry Contact Switch ZMNHND3

local self, node_id, device_info = ...

self:_init_base("default", "HUB:zwave/scripts/model/devices/monitoring", node_id, device_info)

self:_register("345_2_83", "HUB:zwave/scripts/model/devices/monitoring")
