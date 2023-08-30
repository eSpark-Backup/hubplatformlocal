local info = hubplugins.weak_loadfile("HUB:zwave/scripts/migration/version_changes/default")()

info.devices_to_check = {
    ["59_1_1129"] = true,
    ["59_25409_20548"] = true
}

return info
