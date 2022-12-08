local handler, node, device_id, endpoint_id, cluster_name, attribute = ...

local item_names = hubplugins.weak_require("HUB:zigbee/scripts/resources/item_names")

if attribute.formatted_value == "undefined" then return end;
if attribute.name ~= "MeasuredValue" then return end

local item_id = handler:find_item_id(item_names.lux, node.id, endpoint_id, cluster_name, attribute.name)
if not item_id then return end

local min_value, max_value = handler:calculate_min_max_value(
    node, 
    cluster_name, 
    item_names.lux,
    function()
        local min_attr_value = handler:find_attribute(node, endpoint_id, cluster_name, "MinMeasuredValue").formatted_value
        local max_attr_value = handler:find_attribute(node, endpoint_id, cluster_name, "MaxMeasuredValue").formatted_value
        
        if min_attr_value ~= "undefined" and max_attr_value ~= "undefined" and min_attr_value ~= max_attr_value then
            return handler:to_illuminance(min_attr_value), handler:to_illuminance(max_attr_value)
        end
    end
)

handler:update_value_with_min_max_in_core(
    item_id, 
    handler:to_illuminance(attribute.formatted_value),
    min_value,
    max_value
)
