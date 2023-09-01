local params = ...

local tree_model = params.tree_model
local zigbee_node = params.zigbee_node

local function is_category_specified(node)
    return tree_model:call("get_data", node).info.category ~= "unknown"
end

local function is_info_specified(node)
    return tree_model:call("get_data", node).info.info
end


-- main

local root_node = tree_model:call("get_root_node")

if not is_category_specified(root_node) then
    local info = hubplugins.weak_loadfile("HUB:zigbee/scripts/helpers/device_info_detection/detect_device_info")({
        zigbee_node = zigbee_node,
        root_node = root_node,
        tree_model = tree_model
    })

    if info then
        tree_model:call("update_base_service_info", info)
    end
end

if not is_info_specified(root_node) then
    print("Detect device \"info\" field from zigbee information")
    tree_model:call("update_base_service_info",
        {
            info = {
                manufacturer = zigbee_node.manufacturer_id,
                model = zigbee_node.model_id
            }
        }
    )
end