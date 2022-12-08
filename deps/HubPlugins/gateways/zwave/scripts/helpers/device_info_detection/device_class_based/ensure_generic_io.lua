local generic_type, specific_type = ...

if (generic_type == "GENERIC_TYPE_STATIC_CONTROLLER" 
    and (   specific_type == "SPECIFIC_TYPE_SCENE_CONTROLLER"
         or specific_type == "SPECIFIC_TYPE_STATIC_INSTALLER_TOOL"
         or specific_type == "SPECIFIC_TYPE_SET_TOP_BOX"
         or specific_type == "SPECIFIC_TYPE_SUB_SYSTEM_CONTROLLER"
         or specific_type == "SPECIFIC_TYPE_TV"
    )
) then
    return {
        name = "Generic I/O",
        category = "generic_io",
        subcategory = "generic_io"
    }
end
