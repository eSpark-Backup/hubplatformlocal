local value_mapping = ...

local converter = {
    value_mapping = value_mapping
}

function converter.from_ui(value, value_type)
    if value_type == "token" then
        return assert(converter.value_mapping[value], "Unknown value: " .. tostring(value))
    elseif value_type == "array.token" then
        local result = 0
        for _, token in pairs(value) do
            result = result + assert(converter.value_mapping[token], "Unknown value: " .. tostring(token))
        end
        return result
    elseif value_type == "rgb" then
        local result = 0
        for component, value in pairs(value) do
            local offset = converter.value_mapping[component]
            result = result + (value << offset * 8)
        end
        return result
    elseif value_type == "bool" then
        return value and 1 or 0
    elseif value_type == "int" then
        return value
    elseif value_type == "action" then
        return converter.value_mapping.value
    elseif type(value) == "table" and value.scale ~= nil then
        return value.value
    end
end

function converter.to_ui(value, value_type, scale)
    if value_type == "token" then
        for token_name, token_value in pairs(converter.value_mapping) do
            if token_value == value then
                return token_name
            end
        end
    elseif value_type == "array.token" then
        local result = {}
        for token_name, token_value in pairs(converter.value_mapping) do
            if value & token_value ~= 0 then
                result[#result + 1] = token_name
            end
        end
        table.sort(result, function(l, r) return converter.value_mapping[l] < converter.value_mapping[r] end)
        return result
    elseif value_type == "rgb" then
        local result = {}
        for component, offset in pairs(converter.value_mapping) do
            local mask = 0xFF << offset * 8
            result[component] = (value & mask) >> offset * 8
        end
        return result
    elseif value_type == "bool" then
        return value ~= 0
    elseif value_type == "int" then
        return value
    elseif scale ~= nil then
        return {
            value = value,
            scale = scale
        }
    end
end

function converter.get_default_value(value_type, scale)
    if value_type == "token" then
        return "unknown"
    elseif value_type == "array.token" then
        return {}
    elseif value_type == "rgb" then
        local result = {}
        for component, _ in pairs(converter.value_mapping) do
            result[component] = 0
        end
        return result
    elseif value_type == "bool" then
        return false
    elseif value_type == "int" then
        return 0
    elseif scale ~= nil then
        return {
            value = 0,
            scale = scale
        }
    end
end

return converter
