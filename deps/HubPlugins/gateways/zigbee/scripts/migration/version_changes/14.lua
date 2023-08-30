local info = hubplugins.weak_loadfile("HUB:zigbee/scripts/migration/version_changes/default")()

info.devices_to_check = {
    ["AXIS_Gear"] = true
}

return info
