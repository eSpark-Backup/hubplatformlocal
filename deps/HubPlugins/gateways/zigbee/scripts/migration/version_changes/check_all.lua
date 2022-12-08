local info = hubplugins.weak_loadfile("HUB:zigbee/scripts/migration/version_changes/default")()

info.check_all_nodes = true

return info
