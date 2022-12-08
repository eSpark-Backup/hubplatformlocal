local value = ...

local value_type = type(value)

if value_type == "boolean" then
    return value

elseif value_type == 'number' then
    if value == 0 then
        return false
    else
        return true
    end

elseif value_type == 'string' then
    local true_values = { "yes", "1", "true", "on" }
    local false_values = { "no", "0", "false", "off" }

    local function has_value(search_value, values)
        for _, k in ipairs(values) do
            if k == search_value then
                return true
            end
        end
        return false
    end

    if has_value(value, true_values) then
        return true
    elseif has_value(value, false_values) then
        return false
    end
end

error(("don't know how to convert '%s' to boolean"):format(tostring(value)))
