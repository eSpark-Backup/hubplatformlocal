local dev_handler_id, idx = ...

local path = table.concat({ "HUB:zwave/scripts/model/devices/device_settings", dev_handler_id, math.floor(idx) }, "/")

return hubplugins.weak_require(path)
