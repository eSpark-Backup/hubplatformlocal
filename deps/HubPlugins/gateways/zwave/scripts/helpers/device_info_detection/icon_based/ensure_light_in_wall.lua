local icon_type = ...

if icon_type == "ICON_TYPE_SPECIFIC_LIGHT_DIMMER_SWITCH_WALL_OUTLET"
or icon_type == "ICON_TYPE_SPECIFIC_LIGHT_DIMMER_SWITCH_CEILING_OUTLET"
or icon_type == "ICON_TYPE_GENERIC_FAN_SWITCH"
or icon_type == "ICON_TYPE_GENERIC_DIMMER_WALL_SWITCH"
or icon_type == "ICON_TYPE_SPECIFIC_DIMMER_WALL_SWITCH_ONE_BUTTON"
or icon_type == "ICON_TYPE_SPECIFIC_DIMMER_WALL_SWITCH_TWO_BUTTONS"
or icon_type == "ICON_TYPE_SPECIFIC_DIMMER_WALL_SWITCH_THREE_BUTTONS"
or icon_type == "ICON_TYPE_SPECIFIC_DIMMER_WALL_SWITCH_FOUR_BUTTONS"
or icon_type == "ICON_TYPE_SPECIFIC_DIMMER_WALL_SWITCH_ONE_ROTARY" then
    return {
        name = "In Wall Dimmable",
        category = "dimmable_light",
        subcategory = "dimmable_in_wall"
    } 
end
