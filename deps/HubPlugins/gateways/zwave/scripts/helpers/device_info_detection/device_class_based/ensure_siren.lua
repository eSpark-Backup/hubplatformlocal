local generic_type, specific_type = ...

if (generic_type == "GENERIC_TYPE_SWITCH_BINARY" 
    and (specific_type == "SPECIFIC_TYPE_SIREN"
    )
) then
    return {
        name = "Siren",
        category = "siren"
    }
end
