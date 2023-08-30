local enum_helper = {}


local function map_enum_from_to(source, target)
    for k, v in pairs(source) do
        target[v] = k
    end
end


function enum_helper.make_enum(source_enum)
    local value_to_token = {}
    map_enum_from_to(source_enum, value_to_token)
    return {
        token_to_value = source_enum,
        value_to_token = value_to_token
    }
end


return enum_helper
