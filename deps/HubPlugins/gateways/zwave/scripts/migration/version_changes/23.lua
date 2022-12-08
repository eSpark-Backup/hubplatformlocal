local info = hubplugins.weak_loadfile("HUB:zwave/scripts/migration/version_changes/default")()
local class_ids = require("HUB:zwave/scripts/resources/class_ids")

info.cc_to_check = {
    [class_ids.thermostat_setpoint] = true,
}

info.scene_modifications = {
    {
        commands = {
            {
                command = "replace_token",
                to_replace = "aux",
                replace_by = "aux_mode"
            }
        },
        block_ids = { "0001-00000054" }
    }
}

return info
