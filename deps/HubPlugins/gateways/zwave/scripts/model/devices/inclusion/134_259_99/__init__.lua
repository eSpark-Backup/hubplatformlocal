-- Aeon Labs ZW099 Smart Dimmer 6
-- http://store.zwavecenter.com/image/catalog/PDF/AEON_ZW099.pdf

local self, device_info = ...

self:_init_base("default", "HUB:zwave/scripts/model/devices/inclusion", device_info)

self:_register("134_259_99", "HUB:zwave/scripts/model/devices/inclusion")