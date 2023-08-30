-- Fibaro Wall Plug FGWPB-121

local self, device_info = ...

self:_init_base("default", "HUB:zwave/scripts/model/devices/inclusion", device_info)

self:_register("271_5121_8192", "HUB:zwave/scripts/model/devices/inclusion")
