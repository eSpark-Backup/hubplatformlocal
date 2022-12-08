-- Qubino Relay common

local self, device_info = ...

self:_init_base("default", "HUB:zwave/scripts/model/devices/inclusion", device_info)

self:_register("qubino_relay", "HUB:zwave/scripts/model/devices/inclusion")
