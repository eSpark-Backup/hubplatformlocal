local helper = {}

function helper.value_to_ui(value)
    if (value == 99) or (value == 0xFF) then
        return 100
    end

    return value
end

function helper.value_from_ui(value)
    return value == 100 and 99 or value
end

return helper
