local info = hubplugins.weak_loadfile("HUB:zwave/scripts/migration/version_changes/default")()
local class_ids = require("HUB:zwave/scripts/resources/class_ids")

info.cc_to_check = {
    [class_ids.alarm_sensor] = true,
    [class_ids.sensor_binary] = true,
    [class_ids.notification] = true
}

info.items_to_change_value = {
    ["siren_alarm"] = true,
    ["heat_alarm"] = true
}

return info
