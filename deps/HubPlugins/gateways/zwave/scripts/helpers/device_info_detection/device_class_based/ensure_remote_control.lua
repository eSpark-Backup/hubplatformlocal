local generic_type, specific_type = ...

if (generic_type == "GENERIC_TYPE_GENERIC_CONTROLLER" 
    and (   specific_type == "SPECIFIC_TYPE_NOT_USED"
         or specific_type == "SPECIFIC_TYPE_PORTABLE_REMOTE_CONTROLLER"
         or specific_type == "SPECIFIC_TYPE_PORTABLE_SCENE_CONTROLLER"
         or specific_type == "SPECIFIC_TYPE_PORTABLE_INSTALLER_TOOL"
         or specific_type == "SPECIFIC_TYPE_REMOTE_CONTROL_AV"
         or specific_type == "SPECIFIC_TYPE_REMOTE_CONTROL_SIMPLE"
    )
)
or (generic_type == "GENERIC_TYPE_WALL_CONTROLLER" 
    and (  specific_type == "SPECIFIC_TYPE_NOT_USED"
        or specific_type == "SPECIFIC_TYPE_BASIC_WALL_CONTROLLER"
    )
) then
    return {
        name = "Remote Control",
        category = "remote_control"
    }
end
