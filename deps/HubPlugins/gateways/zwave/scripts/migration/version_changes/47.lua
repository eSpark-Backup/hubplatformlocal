local info = hubplugins.weak_loadfile("HUB:zwave/scripts/migration/version_changes/default")()

info.devices_to_check = {
    ["647_4_113"] = true
}

return info
