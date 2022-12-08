local self, device_info = ...

self:_init_base("default", "HUB:zwave/scripts/model/devices/inclusion", device_info)

self:_register("channel_merge", "HUB:zwave/scripts/model/devices/inclusion")
