local core_device, device_node_data, items_old_names = ...

local core = require("core")
local jaccard_index = hubplugins.weak_loadfile("HUB:zwave/scripts/helpers/jaccard_index")
local jaro_winkler_distance = hubplugins.weak_loadfile("HUB:zwave/scripts/helpers/jaro_winkler_distance")

local function get_name_factor(name1, name2)
    return jaro_winkler_distance(name1, name2)
end

local function get_type_factor(type1, type2)
    return (type1 == type2) and 1 or 0
end

local function get_category_factor(category1, category2)
    return (category1 == category2) and 1 or 0
end

local function get_subcategory_factor(subcategory1, subcategory2)
    return (subcategory1 == subcategory2) and 1 or 0
end

local function get_items_factor(dev_core, dev_node_data, items_old_names)
    local items = core.get_items_by_device_id(dev_core.id)

    local items_set1 = {}
    if items then
        for _, item in ipairs(items) do
            local name = items_old_names[item.name] or item.name
            items_set1[name] = true
        end
    end

    local items_set2 = {}
    if dev_node_data.items then
        for _, item in ipairs(dev_node_data.items) do
            items_set2[item.name] = true
        end
    end

    if next(items_set1) and next(items_set2) then
        return jaccard_index(items_set1, items_set2)
    else
        return 0
    end
end

local config = hubplugins.weak_require("HUB:zwave/scripts/migration/config")

local name_factor = get_name_factor(core_device.name, device_node_data.info.name) * config.name_coefficient
local type_factor = get_type_factor(core_device.type, device_node_data.info.type) * config.type_coefficient
local category_factor = get_category_factor(core_device.category, device_node_data.info.category) * config.category_coefficient
local subcategory_factor = get_subcategory_factor(core_device.subcategory, device_node_data.info.subcategory) * config.subcategory_coefficient
local items_factor = get_items_factor(core_device, device_node_data, items_old_names) * config.item_coefficient

return name_factor + type_factor + category_factor + subcategory_factor + items_factor
