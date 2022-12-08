local handler, node, device_id, endpoint_id, cluster_name, attribute = ...

local item_names = hubplugins.weak_require("HUB:zigbee/scripts/resources/item_names")

if attribute.name ~= "CurrentLevel" then return end

local item_id = handler:find_item_id(item_names.dimmer, node.id, endpoint_id, cluster_name, attribute.name)
if not item_id then return end

-- check switch value
local switch_attribute = handler:find_attribute(node, endpoint_id, "on_off", "OnOff")
if switch_attribute then
    local switch_value = hubplugins.weak_loadfile("HUB:zigbee/scripts/helpers/to_boolean")(switch_attribute.formatted_value)
    if not switch_value then
        return
    elseif attribute.formatted_value == 0 then
        handler:update_value_in_core(item_id, 1) -- Corner case when bulb sends dimmer 0 with turned on switch
        return
    end
end

-- update dimmer value
-- convert value with base 254 to base 100
local newValue = math.floor((attribute.formatted_value / 2.54) + 0.5)
if attribute.formatted_value ~= 0 and newValue == 0 then newValue = 1 end -- TODO problems when formated value < 3

handler:update_value_in_core(item_id, newValue)
