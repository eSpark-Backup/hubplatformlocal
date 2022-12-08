local icon_type = ...

if icon_type == "ICON_TYPE_SPECIFIC_ON_OFF_POWER_SWITCH_PLUGIN"
or icon_type == "ICON_TYPE_GENERIC_ON_OFF_POWER_SWITCH" then
    return {
        name = "Switch Interrior",
        category = "switch",
        subcategory = "interior_plugin"
    } 
end
