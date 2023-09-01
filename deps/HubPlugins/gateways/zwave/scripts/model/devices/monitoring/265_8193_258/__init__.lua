-- Door/Window Sensor - VS-ZD2102-US

local self, node_id, device_info = ...

self:_init_base("default", "HUB:zwave/scripts/model/devices/monitoring", node_id, device_info)

self:_register("265_8193_258", "HUB:zwave/scripts/model/devices/monitoring")
