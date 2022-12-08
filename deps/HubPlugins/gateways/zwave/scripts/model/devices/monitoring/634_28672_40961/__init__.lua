-- Zooz ZEN71 700 Series Z-Wave Plus On/Off Switch
local self, node_id, device_info = ...

self:_init_base("default", "HUB:zwave/scripts/model/devices/monitoring", node_id, device_info)

self:_register("634_28672_40961", "HUB:zwave/scripts/model/devices/monitoring")
