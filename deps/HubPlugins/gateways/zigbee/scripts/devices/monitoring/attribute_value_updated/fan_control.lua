local handler, node, device_id, endpoint_id, cluster_name, attribute = ...

if attribute.name ~= "FanMode" then return end

local item_names = hubplugins.weak_require("HUB:zigbee/scripts/resources/item_names")
local item_id = handler:find_item_id(item_names.thermostat_fan_mode, node.id, endpoint_id, cluster_name, attribute.name)
if item_id then
    local helper = hubplugins.weak_loadfile("HUB:zigbee/scripts/helpers/fan_control_helper")()
    assert(helper.attr_value_to_enum[attribute.formatted_value], "Unsupported fan mode attr value: " .. attribute.formatted_value)

    handler:update_value_in_core(item_id, helper.attr_value_to_enum[attribute.formatted_value])
end
