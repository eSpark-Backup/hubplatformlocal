local descriptor = ...

local item_names = hubplugins.weak_require("HUB:zigbee/scripts/resources/item_names")

local data = {
    color_control = { 
        [item_names.rgb_color] = { "CurrentX", "CurrentY", "ColorTemperatureMireds", "ColorMode" }
    }
}

if data[descriptor.cluster_name] and data[descriptor.cluster_name][descriptor.item_name] then
    return data[descriptor.cluster_name][descriptor.item_name]
end

if descriptor.attribute_name ~= "" then
    return { descriptor.attribute_name }
end
