local handler, item_id, descriptor, value = ...

local zigbee = require("zigbee")
local item_names = hubplugins.weak_require("HUB:zigbee/scripts/resources/item_names")

local function request(attribute_name)
    handler:request_item_value_with_timeout(
        descriptor.node_id,
        descriptor.endpoint_id,
        descriptor.cluster_name,
        attribute_name
    )
end

local function set_attribute_value(attribute_name, value_to_set)
    zigbee.set_attribute_value(
        descriptor.node_id,
        descriptor.endpoint_id,
        descriptor.cluster_name,
        attribute_name,
        value_to_set
    )
end

local function convert_scale(value)
    if value.scale ~= "celsius" then
        local unit_converter = hubplugins.object("unit_converter", "HUB:zigbee/scripts/utils")
        return unit_converter:call("convert", value.value, value.scale, "celsius")
    end

    return value.value
end


-- main


if descriptor.attribute_name == "SystemMode" then
    set_attribute_value(descriptor.attribute_name, value)
else
    local new_value_in_correct_scale = convert_scale(value)
    if descriptor.attribute_name == "OccupiedCoolingSetpoint" or descriptor.attribute_name == "OccupiedHeatingSetpoint" then
        set_attribute_value(descriptor.attribute_name, new_value_in_correct_scale * 100)
    end
end
