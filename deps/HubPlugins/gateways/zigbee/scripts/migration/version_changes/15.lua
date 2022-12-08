local info = hubplugins.weak_loadfile("HUB:zigbee/scripts/migration/version_changes/default")()

info.devices_to_check = {
    ["LUMI_lumi.remote.b1acn01"] = true
}

return info
