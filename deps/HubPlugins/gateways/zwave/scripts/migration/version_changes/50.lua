local info = hubplugins.weak_loadfile("HUB:zwave/scripts/migration/version_changes/default")()

info.devices_to_check = {
    ["0_3_2"] = true
}

return info
