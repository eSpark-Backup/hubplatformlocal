local info = hubplugins.weak_loadfile("HUB:zwave/scripts/migration/version_changes/default")()

info.devices_to_check = {
    ["271_2304_8192"] = true,
    ["271_2304_4096"] = true
}

return info
