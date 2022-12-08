local handler, node, device_id, endpoint_id, cluster_name, attribute = ...

if attribute.name ~= "ZoneStatus" then return end

local zone_type_attribute = handler:find_attribute(node, endpoint_id, cluster_name, "ZoneType")
assert(zone_type_attribute, "Could not find ZoneType attribute")

local helper = hubplugins.weak_loadfile("HUB:zigbee/scripts/helpers/ias_zone_helper")()

local value_updated = false
local item_name = helper.zone_type_to_item_name(zone_type_attribute.formatted_value.zone_type)
if item_name then
    local item_id = handler:find_item_id(item_name, node.id, endpoint_id, cluster_name, attribute.name)
    if item_id then
        local new_value = helper.alarm_statuses_to_item_value(
            item_name,
            attribute.formatted_value.alarm_1,
            attribute.formatted_value.alarm_2 )

        handler:update_value_in_core(item_id, new_value)
        value_updated = true
    end
end

if not value_updated then
    local callback = function(alarm_type)
        local item_name = helper.zone_type_to_item_name(zone_type_attribute.formatted_value.zone_type, alarm_type)
        if item_name then
            local item_id = handler:find_item_id(item_name, node.id, endpoint_id, cluster_name, attribute.name, alarm_type)
            if item_id then
                local new_value = helper.zone_status_to_item_value(item_name, attribute.formatted_value[alarm_type])
                handler:update_value_in_core(item_id, new_value)
            end
        end
    end

    callback("alarm_1")
    callback("alarm_2")
end

-- update tamper value
local item_names = hubplugins.weak_require("HUB:zigbee/scripts/resources/item_names")
local tampering_cover_enum = hubplugins.weak_loadfile("HUB:zigbee/scripts/resources/enums/tampering_cover_events")()
local tamper_item_id = handler:find_item_id(item_names.tampering_cover_alarm, node.id, endpoint_id, cluster_name, attribute.name)
if tamper_item_id then
    local new_value = attribute.formatted_value.tamper == "tampered" and
        tampering_cover_enum.tampering_cover or
        tampering_cover_enum.no_tampering_cover

    handler:update_value_in_core(tamper_item_id, new_value)
end