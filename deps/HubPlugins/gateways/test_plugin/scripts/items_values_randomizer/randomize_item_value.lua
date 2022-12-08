local item_id = ...

local core = require "core"

local function is_scalable_type( item )
    if type(item.value) == "table" then
        if item.value.scale then
            return true
        end
    end
    
    return false
end

local function get_min_max_values( item )
    local min, max
    
    if item.value_min then
        if is_scalable_type( item ) then
            min = item.value_min.value
        else
            min = item.value_min
        end
    else
        min = 0
    end
    
    if item.value_max then
        if is_scalable_type( item ) then
            max = item.value_max.value
        else
            max = item.value_max
        end
    else
        max = 255
    end
    
    return min, max
end

local function generate_random_value( min, max )
    return math.random( min, max )
end


-- main


local item = core.get_item( item_id )

if item.value_type == "bool" then
    local value = generate_random_value( 0, 1 )
    core.update_item_value( item.id, value == 1 )
    
elseif item.value_type == "token" then
    local value = generate_random_value( 1, #item.enum )
    core.update_item_value( item.id, item.enum[value] )
    
elseif item.value_type == "rgb" then
    local new_value = {}
    for color, _ in pairs( item.value ) do
        new_value[ color ] = generate_random_value( 0, 255 )
    end
    core.update_item_value( item.id, new_value )
    
else
    local min, max = get_min_max_values( item )
    local new_value = generate_random_value( min, max )
    
    if item.value_type == "int" or item.value_type == "float" then
        core.update_item_value( item.id, new_value )
    elseif is_scalable_type( item ) then
        local value = item.value
        value.value = new_value
        core.update_item_value( item.id, value )
    end
end
