local params = ...

local zwave_node = params.zwave_node
local root_node = params.root_node
local tree_model = params.tree_model


-- main

local function detect_device_info_by_icon()
    if zwave_node.icon_type ~= nil then
        return hubplugins.weak_loadfile("HUB:zwave/scripts/helpers/device_info_detection/icon_based/detect_device_info_by_icon")({
            icon_type = zwave_node.icon_type
        })
    end
end

local function detect_device_info_by_class()
    if zwave_node.generic_type ~= nil and zwave_node.specific_type ~= nil then
        assert(type(zwave_node.generic_type) == "string", "Generic type: " .. zwave_node.generic_type .. " is not string")
        assert(type(zwave_node.specific_type) == "string", "Specific type: " .. zwave_node.specific_type .. " is not string")
        return hubplugins.weak_loadfile("HUB:zwave/scripts/helpers/device_info_detection/device_class_based/detect_device_info_by_class")({
            generic_type = zwave_node.generic_type,
            specific_type = zwave_node.specific_type,
            channels = zwave_node.channels
        })
    end
end

local function detect_device_info_by_items()
    return hubplugins.weak_loadfile("HUB:zwave/scripts/helpers/device_info_detection/items_based/detect_device_info_by_items")({
        node = root_node,
        tree_model = tree_model
    })
end


local is_manufacturer_undefined = hubplugins.weak_loadfile("HUB:zwave/scripts/helpers/is_manufacturer_undefined")(zwave_node.manufacturer_id)

if is_manufacturer_undefined then
    -- At first, detect by items for unknown manufacturer (certification tests)
    return detect_device_info_by_items() or detect_device_info_by_icon() or detect_device_info_by_class()
end

return detect_device_info_by_icon() or detect_device_info_by_class() or detect_device_info_by_items()