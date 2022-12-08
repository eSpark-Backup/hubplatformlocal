local info = hubplugins.weak_loadfile("HUB:zwave/scripts/migration/version_changes/default")()

info.devices_to_check = {
    ["297_2_0"] = true,
    ["297_3_0"] = true
}

return info
