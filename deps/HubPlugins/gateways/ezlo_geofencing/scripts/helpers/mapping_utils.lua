local core = assert(require "core", "Failed to load core")

local function remove_fence_if_does_not_exist( values, item )
    local item_id = item.id
    for item_key,item_val in pairs(item.value) do
        value = values[item_key]
        if value == nil then -- if not exist in input params
        -- remove item dictionary value [item_key] from item_values
            core.remove_item_dictionary_value( item_id, item_key )
        end
    end
end

local function add_or_update_fences( values, item )
    local item_id = item.id
    for key,value in pairs(values) do
       core.update_item_dictionary_value( item_id, key, value )
    end
end

local function update_item_dictionary_values(values, item)
     remove_fence_if_does_not_exist( values, item )
     add_or_update_fences( values, item )
end

local mapping_utils = {}
function mapping_utils.get_value_type(mapping)
    local dictionaryType = mapping[ 'dictionaryType' ]
    if dictionaryType ~= nil and dictionaryType then
        return 'dictionary.' .. mapping.valueType
    end
    return mapping.valueType
end

function mapping_utils.create_abstract_device(abstract_uuid)
    local gateway = core.get_gateway()
    local device_id = core.add_device({ gateway_id = gateway.id,
                                      name = abstract_uuid,
                                      type = "abstract",
                                      device_type_id = "abstract",
                                      category = "abstract",
                                      subcategory = "geofencing",
                                      battery_powered = false })
    return device_id
end

function mapping_utils.create_abstract_item(device_id, mapping)
    local name = mapping.capability .. "." .. mapping.variable
    local value_type = mapping_utils.get_value_type(mapping)
    local item_id = core.add_item({ device_id = device_id,
                                    name = name,
                                    value_type = value_type,
                                    has_getter = true,
                                    has_setter = false,
                                    value = mapping.value })
    return item_id
end

function mapping_utils.update_abstract_item(item, mapping)
    local name = mapping.capability .. "." .. mapping.variable
    local value_type = mapping_utils.get_value_type(mapping)

    if item.value_type == value_type then
        update_item_dictionary_values(mapping.value, item)
    else
        logging.error("Failed to update dictionary values: incorrect value type" .. value_type)
    end
end

return mapping_utils

