local generic_type, specific_type = ...

if (generic_type == "GENERIC_TYPE_STATIC_CONTROLLER" 
    and (   specific_type == "SPECIFIC_TYPE_NOT_USED"
         or specific_type == "SPECIFIC_TYPE_PC_CONTROLLER"
         or specific_type == "SPECIFIC_TYPE_GATEWAY"
    )
) then
    return {
        name = "Controller",
        category = "generic_io",
        subcategory = "controller"
    }
end
