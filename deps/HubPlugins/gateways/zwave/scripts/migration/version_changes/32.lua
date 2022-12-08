local info = hubplugins.weak_loadfile("HUB:zwave/scripts/migration/version_changes/default")()
local class_ids = require("HUB:zwave/scripts/resources/class_ids")

info.devices_to_check = {
    ["881_258_7"] = true,
    ["881_0_7"] = true,
    ["881_258_12"] = true,
    ["881_2_12"] = true
}

return info
