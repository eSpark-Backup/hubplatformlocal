local icon_type = ...

if icon_type == "ICON_TYPE_SPECIFIC_LIGHT_DIMMER_SWITCH_WALL_LAMP"
or icon_type == "ICON_TYPE_SPECIFIC_LIGHT_DIMMER_SWITCH_LAMP_POST_HIGH"
or icon_type == "ICON_TYPE_SPECIFIC_LIGHT_DIMMER_SWITCH_LAMP_POST_LOW"
or icon_type == "ICON_TYPE_SPECIFIC_ON_OFF_POWER_SWITCH_WALL_LAMP"
or icon_type == "ICON_TYPE_SPECIFIC_ON_OFF_POWER_SWITCH_LAMP_POST_HIGH"
or icon_type == "ICON_TYPE_SPECIFIC_ON_OFF_POWER_SWITCH_LAMP_POST_LOW" then
    return {
        name = "Bulb Dimmable",
        category = "dimmable_light",
        subcategory = "dimmable_bulb"
    } 
end
