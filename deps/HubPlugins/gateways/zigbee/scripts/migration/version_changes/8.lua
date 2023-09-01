local info = hubplugins.weak_loadfile("HUB:zigbee/scripts/migration/version_changes/default")()

info.clusters_to_check = {
    temperature_measurement = true,
    thermostat = true,
}

return info
