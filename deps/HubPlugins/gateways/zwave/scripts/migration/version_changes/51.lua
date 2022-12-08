local info = hubplugins.weak_loadfile("HUB:zwave/scripts/migration/version_changes/default")()

info.devices_to_check = {
    ["265_1_0"] = true,
    ["297_4_0"] = true,
    ["297_4_43520"] = true,
    ["297_4_521"] = true
}

return info
