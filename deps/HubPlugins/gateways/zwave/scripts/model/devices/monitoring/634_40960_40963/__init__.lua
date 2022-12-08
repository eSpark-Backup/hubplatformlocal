-- ZEN25 S2 Double Plug

local self, node_id, device_info = ...

self:_init_base("default", "HUB:zwave/scripts/model/devices/monitoring", node_id, device_info)

self:_register("634_40960_40963", "HUB:zwave/scripts/model/devices/monitoring")
