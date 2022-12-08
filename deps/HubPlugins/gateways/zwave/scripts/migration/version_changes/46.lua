local info = hubplugins.weak_loadfile("HUB:zwave/scripts/migration/version_changes/default")()

info.devices_to_check = {
    ["12_4_1"] = true
}

return info
