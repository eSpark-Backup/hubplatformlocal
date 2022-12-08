local params = ...

local zwave_node = params.zwave_node
local tree_model = params.tree_model

local function is_category_specified(node)
    return tree_model:call("get_data", node).info.category ~= "unknown"
end

local function is_manufacturer_info_specified(node)
    local info = tree_model:call("get_data", node).info.info
    return info.manufacturer ~= nil or info.model ~= nil
end

-- main
local base_device_name = tree_model:call("get_base_device_name")
local is_integrated_processed_device = base_device_name ~= "Unknown"

local root_node = tree_model:call("get_root_node")

if not is_category_specified(root_node) then
    -- todo add other detections
    local info = hubplugins.weak_loadfile("HUB:zwave/scripts/helpers/device_info_detection/detect_device_info")({
        zwave_node = zwave_node,
        root_node = root_node,
        tree_model = tree_model
    })

    info = info or {}
    tree_model:call("update_base_service_info", info)
end

-- add manufacturer name prefix
local root_node_info = tree_model:call("get_data", root_node).info

if is_integrated_processed_device then
    root_node_info.name = base_device_name
else
    local manufacturer_name = hubplugins.weak_loadfile("HUB:zwave/scripts/resources/manufacturer_names")()[zwave_node.manufacturer_id]
    local prefix = manufacturer_name and (manufacturer_name .. " ") or ""

    root_node_info.name = prefix .. root_node_info.name
end

-- calculate security field and set it
local security = hubplugins.weak_loadfile("HUB:zwave/scripts/helpers/get_node_security")(zwave_node)
assert(security, "Can not calculate device security.")
print("Detected device security: " .. security)
local callback = function(dev_node)
    local dev_data = tree_model:call("get_data", dev_node)
    dev_data.info.security = security
end
tree_model:call("direct_walk", callback)


-- fill info field if needed
if not is_manufacturer_info_specified(root_node) then
    print("Detect device \"info\" field from zwave information")
    local zwave = require("zwave")
    tree_model:call("update_base_service_info", 
        {
            info = {
                manufacturer = zwave.get_manufacturer_name and 
                    zwave.get_manufacturer_name(zwave_node.manufacturer_id) or
                    "Unknown",
                model = "Unknown"
            }
        }
    )
end

-- Move all keys from the metatable, so that they can be read in core
tree_model:call("direct_walk", function(node)
    local info = rawget(node.data.info, "info")
    if info then
        local function copy_metatable(t)
            local mt = getmetatable(t)
            if mt ~= nil and mt.__index ~= nil then
                for key, value in pairs(mt.__index) do
                    info[key] = value
                end

                copy_metatable(mt.__index)
            end
        end

        copy_metatable(info)
    end
end)

