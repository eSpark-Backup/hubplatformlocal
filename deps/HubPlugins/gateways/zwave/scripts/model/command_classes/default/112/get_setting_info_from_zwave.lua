local self, zwave_parameter_info = ...

local format_to_setting_type = {
    signed_integer = "int",
    unsigned_integer = "int",
    enumerated = "token",
    bit_field = "array.token"
}

if zwave_parameter_info.size == 0 then
    -- Do not add unspecified setting
    return
end

local result = {
    name = zwave_parameter_info.name,
    description = zwave_parameter_info.info,
    type = format_to_setting_type[zwave_parameter_info.format],
    has_setter = not zwave_parameter_info.read_only,
    configuration = {
        number = zwave_parameter_info.id,
        size = zwave_parameter_info.size
    }
}

if result.type == "int" then
    if zwave_parameter_info.default_value then
        result.configuration.value = zwave_parameter_info.default_value
    end
    
    if zwave_parameter_info.min_value then
        result.value_min = zwave_parameter_info.min_value
    end
    
    if zwave_parameter_info.max_value then
        result.value_max = zwave_parameter_info.max_value
    end
end

if result.type == "token" then
    if zwave_parameter_info.default_value then
        result.configuration.value = zwave_parameter_info.default_value
    end
    
    result.enum = {}
    for value = zwave_parameter_info.min_value, zwave_parameter_info.max_value do
        result.enum[tostring(value)] = { value = value, text = tostring(value) }
    end
end

if result.type == "array.token" then
    if zwave_parameter_info.default_value then
        result.configuration.value = zwave_parameter_info.default_value
    end
    
    result.enum = {}
    for bit_number = 0, (zwave_parameter_info.size * 8) - 1 do
        local mask = (1 << bit_number)
        if zwave_parameter_info.max_value >= mask and (zwave_parameter_info.max_value & mask) ~= 0 then
            result.enum[tostring(bit_number)] = { value = mask, text = tostring(bit_number) }
        end
    end
end

return result
