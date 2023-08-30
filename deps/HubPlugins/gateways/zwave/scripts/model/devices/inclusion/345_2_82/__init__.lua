-- Qubino ZMNHAD3 Flush 1 Relay

local self, device_info = ...

self:_init_base("qubino_relay", "HUB:zwave/scripts/model/devices/inclusion", device_info)

self:_register("345_2_82", "HUB:zwave/scripts/model/devices/inclusion")
