local icon_type = ...

if icon_type == "ICON_TYPE_GENERIC_REMOTE_CONTROL_MULTI_PURPOSE"
or icon_type == "ICON_TYPE_GENERIC_REMOTE_CONTROL_SIMPLE"
or icon_type == "ICON_TYPE_SPECIFIC_REMOTE_CONTROL_SIMPLE_KEYFOB"
or icon_type == "ICON_TYPE_GENERIC_WALL_CONTROLLER" then
    return {
        name = "Remote Control",
        category = "remote_control"
    } 
end
