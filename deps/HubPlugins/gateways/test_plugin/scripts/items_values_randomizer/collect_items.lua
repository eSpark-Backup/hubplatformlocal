local item_names, item_ids = ...

local function array_to_map( array )
    local map = {}
    for _, data in ipairs( array ) do
        map[ data ] = true
    end

    return map
end

local function is_scalable_type( item )
    if type(item.value) == "table" then
        if item.value.scale then
            return true
        end
    end

    return false
end

local function is_supported_type( item )
    return item.value_type == "bool"
            or item.value_type == "token"
            or item.value_type == "rgb"
            or item.value_type == "int"
            or item.value_type == "float"
            or is_scalable_type( item )
end

local core = require "core"

item_names = item_names or {}
local item_names_map = array_to_map( item_names )

item_ids = item_ids or {}
local item_ids_map = array_to_map( item_ids )

local devices = core.get_devices()

local result = {}

local gateway = core.get_gateway()
if gateway then
    for _, device in ipairs( devices ) do
        if device.gateway_id == gateway.id then
            local items = core.get_items_by_device_id( device.id )

            for _, item in ipairs( items ) do
                if ( #item_names == 0 and #item_ids == 0 )
                        or item_names_map[ item.name ]
                        or item_ids_map[ item.id ] then

                    if is_supported_type( item )  then
                        table.insert( result, item.id )
                    end
                end
            end
        end
    end
end

return result
