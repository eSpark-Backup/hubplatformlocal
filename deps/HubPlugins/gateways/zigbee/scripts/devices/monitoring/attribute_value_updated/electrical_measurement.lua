local handler, node, device_id, endpoint_id, cluster_name, attribute = ...

local helper = hubplugins.weak_loadfile("HUB:zigbee/scripts/helpers/electrical_measurement_helper")()
local item_name = helper.attribute_to_item_name[attribute.name]

if not item_name or attribute.formatted_value == "undefined" then return end;

local divide_by = 1
local multiply_by = 1

-- Get divide value
local divide_attr_name = helper.value_modification_attributes[attribute.name].divisor
if divide_attr_name then
    local divide_attr = handler:find_attribute(node, endpoint_id, cluster_name, divide_attr_name)
    if divide_attr then divide_by = divide_attr.formatted_value end
end

-- Get multiply value
local multiply_attr_name = helper.value_modification_attributes[attribute.name].multiplier
if multiply_attr_name then
    local multiply_attr = handler:find_attribute(node, endpoint_id, cluster_name, multiply_attr_name)
    if multiply_attr then multiply_by = multiply_attr.formatted_value end
end

local item_id = handler:find_item_id(item_name, node.id, endpoint_id, cluster_name, attribute.name)
if item_id then
    handler:update_value_in_core(
        item_id,
        {
            value = (attribute.formatted_value * multiply_by) / divide_by,
            scale = helper.attribute_to_scale[attribute.name]
        }
    )
end
