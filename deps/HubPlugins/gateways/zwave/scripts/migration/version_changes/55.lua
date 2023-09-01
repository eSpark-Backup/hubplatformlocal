local info = hubplugins.weak_loadfile("HUB:zwave/scripts/migration/version_changes/default")()
local class_ids = require( "HUB:zwave/scripts/resources/class_ids" )

info.cc_to_check = {
    [class_ids.notification] = true,
    [class_ids.sound_switch] = true
}

return info
