local basic_item = ...

local cc_ids = require( "HUB:zwave/scripts/resources/class_ids" )
local storage = hubplugins.object("storage_helper", "HUB:zwave/scripts/utils" )
local core = require( "core" )
local item_names = require( "HUB:zwave/scripts/resources/item_names" )

local item_names_to_priorities = {
    [ item_names.switch ] = 2,
    [ item_names.dimmer ] = 1,
    [ item_names.co_alarm ] = 3,
    [ item_names.co2_alarm ] = 3,
    [ item_names.smoke_alarm ] = 3,
    [ item_names.heat_alarm ] = 3,
    [ item_names.water_leak_alarm ] = 3,
    [ item_names.freeze_alarm ] = 3,
    [ item_names.dw_state ] = 3,
    [ item_names.tilt ] = 3,
    [ item_names.motion ] = 3,
    [ item_names.glass_breakage_alarm ] = 3
}

local function find_sensor_type_by_item_name(source_file, item_name)
    local sensor_type_to_item_name = hubplugins.weak_require("HUB:zwave/scripts/resources/" .. source_file).sensor_type_to_item_name
    for key, value in pairs(sensor_type_to_item_name) do
        if value == item_name then
            return key
        end
    end
    
    return nil
end

local siblings = core.get_items_by_device_id( basic_item.device_id )

local found_item
for _, item in ipairs(siblings) do
    if item_names_to_priorities[ item.name ] and 
    ( not found_item or item_names_to_priorities[ item.name ] < item_names_to_priorities[ found_item.name ] ) then
        found_item = item
    end
end

if found_item then
    local descriptors = storage:call( "find_item_descriptors", found_item.id )

    for _, descriptor in ipairs(descriptors) do
        if descriptor.class_id == cc_ids.switch_binary or descriptor.class_id == cc_ids.dimmer then
            return { id = descriptor.class_id }
        elseif descriptor.class_id == cc_ids.sensor_binary then
            return {
                id = descriptor.class_id,
                sensor_type = find_sensor_type_by_item_name( "binary_sensor_map", found_item.name )
            }
        elseif descriptor.class_id == cc_ids.alarm_sensor then
            return { 
                id = descriptor.class_id,
                sensor_type = find_sensor_type_by_item_name( "alarm_sensor_map", found_item.name )
            } 
        end
    end
end

return nil