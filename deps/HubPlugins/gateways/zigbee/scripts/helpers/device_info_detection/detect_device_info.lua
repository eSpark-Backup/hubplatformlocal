local params = ...

local root_node = params.root_node
local tree_model = params.tree_model
local zigbee_node = params.zigbee_node


-- main

local info = hubplugins.weak_loadfile("HUB:zigbee/scripts/helpers/device_info_detection/clusters_based/detect_device_info_by_clusters")(
    zigbee_node
)

if info then return info end

return hubplugins.weak_loadfile("HUB:zigbee/scripts/helpers/device_info_detection/items_based/detect_device_info_by_items")({
    node = root_node,
    tree_model = tree_model
})
