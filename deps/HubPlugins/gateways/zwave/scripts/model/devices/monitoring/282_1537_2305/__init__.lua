-- Enerwave ZWN-BPC-PLUS Motion Sensor

local self, node_id, device_info = ...

self:_init_base("default", "HUB:zwave/scripts/model/devices/monitoring", node_id, device_info)

self:_register("282_1537_2305", "HUB:zwave/scripts/model/devices/monitoring")
