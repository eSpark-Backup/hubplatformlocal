local handler, node, device_id, endpoint_id, cluster_name, attribute = ...

local item_names = hubplugins.weak_require("HUB:zigbee/scripts/resources/item_names")

if attribute.name ~= "OnOff" then return end
local new_value = hubplugins.weak_loadfile("HUB:zigbee/scripts/helpers/to_boolean")(attribute.formatted_value)

-- update dimmer value
local dimmer_attribute = handler:find_attribute(node, endpoint_id, "level_control", "CurrentLevel")
if dimmer_attribute then
    if not new_value then
        local dimmer_item_id = handler:find_item_id(item_names.dimmer, node.id, endpoint_id, "level_control", "CurrentLevel")
        if dimmer_item_id then
            handler:update_value_in_core(dimmer_item_id, 0)
        end
    else
        handler:request_item_value_with_timeout(node.id, endpoint_id, "level_control", "CurrentLevel", 1000)
    end
end

-- update switch value
local item_id = handler:find_item_id(item_names.switch, node.id, endpoint_id, cluster_name, attribute.name)
if item_id then
    handler:update_value_in_core(item_id, hubplugins.weak_loadfile("HUB:zigbee/scripts/helpers/to_boolean")(attribute.formatted_value))
end
