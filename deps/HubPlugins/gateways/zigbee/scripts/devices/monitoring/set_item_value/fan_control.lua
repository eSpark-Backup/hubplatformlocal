local handler, item_id, descriptor, value = ...

local zigbee = require("zigbee")

if descriptor.attribute_name == "FanMode" then
    local helper = hubplugins.weak_loadfile("HUB:zigbee/scripts/helpers/fan_control_helper")()
    assert(helper.enum_to_attr_value[value], "Unsupported fan mode item value: " .. value)

    zigbee.set_attribute_value(
        descriptor.node_id,
        descriptor.endpoint_id,
        descriptor.cluster_name,
        descriptor.attribute_name,
        helper.enum_to_attr_value[value]
    )
end
