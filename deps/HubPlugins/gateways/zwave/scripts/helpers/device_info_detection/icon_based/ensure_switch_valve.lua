local icon_type = ...

if icon_type == "ICON_TYPE_GENERIC_VALVE_OPEN_CLOSE"
or icon_type == "ICON_TYPE_GENERIC_IRRIGATION" then
    return {
        name = "Valve Switch",
        category = "switch",
        subcategory = "valve"
    } 
end
