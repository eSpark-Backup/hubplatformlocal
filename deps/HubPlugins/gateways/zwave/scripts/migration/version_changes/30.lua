local info = hubplugins.weak_loadfile("HUB:zwave/scripts/migration/version_changes/default")()
local class_ids = require("HUB:zwave/scripts/resources/class_ids")

info.cc_to_check = {
    [class_ids.thermostat_setback] = true,
    -- [class_ids.door_lock] = true,
}

info.storage_setting_deleter = function(setting_id)
    local default_exclusion_handler = hubplugins.object("default", "HUB:zwave/scripts/model/devices/exclusion")
    default_exclusion_handler:call("remove_storage_setting", setting_id)
end

return info
