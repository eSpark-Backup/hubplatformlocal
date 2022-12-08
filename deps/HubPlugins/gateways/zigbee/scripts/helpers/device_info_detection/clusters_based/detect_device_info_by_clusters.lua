local zigbee_node = ...

return hubplugins.weak_loadfile("HUB:zigbee/scripts/helpers/device_info_detection/clusters_based/by_ias_zone_cluster")(
    zigbee_node
)
