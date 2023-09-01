local info = hubplugins.weak_loadfile("HUB:zigbee/scripts/migration/version_changes/default")()

info.clusters_to_check = {
    electrical_measurement = true,
}

return info
