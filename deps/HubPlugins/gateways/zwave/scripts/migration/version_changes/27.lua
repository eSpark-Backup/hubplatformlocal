local info = hubplugins.weak_loadfile("HUB:zwave/scripts/migration/version_changes/default")()

info.devices_to_check = {
    ["134_2_100"] = true,
    ["134_258_100"] = true,
    ["134_514_100"] = true,
    ["134_770_100"] = true,
    ["134_2562_100"] = true
}

return info
