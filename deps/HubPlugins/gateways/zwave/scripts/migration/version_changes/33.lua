local info = hubplugins.weak_loadfile("HUB:zwave/scripts/migration/version_changes/default")()
local class_ids = require("HUB:zwave/scripts/resources/class_ids")

info.devices_to_check = {
    ["316_2_12"] = true,
    ["316_2_13"] = true,
    ["316_2_14"] = true
}

return info
