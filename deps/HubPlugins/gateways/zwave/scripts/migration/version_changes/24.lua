local info = hubplugins.weak_loadfile("HUB:zwave/scripts/migration/version_changes/default")()

info.devices_to_check = {
    ["132_115_257"] = true,
    ["134_3_11"] = true
}

return info
