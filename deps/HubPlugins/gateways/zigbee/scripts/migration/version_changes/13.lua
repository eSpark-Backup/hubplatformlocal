local info = hubplugins.weak_loadfile("HUB:zigbee/scripts/migration/version_changes/default")()

info.devices_to_check = {
    ["WAXMAN_leakSMART Water Sensor V2"] = true
}

return info
