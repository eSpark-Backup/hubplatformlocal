local icon_type = ...

if icon_type == "ICON_TYPE_SPECIFIC_ON_OFF_POWER_SWITCH_WALL_OUTLET"
or icon_type == "ICON_TYPE_SPECIFIC_ON_OFF_POWER_SWITCH_CEILING_OUTLET"
or icon_type == "ICON_TYPE_GENERIC_ON_OFF_WALL_SWITCH"
or icon_type == "ICON_TYPE_SPECIFIC_ON_OFF_WALL_SWITCH_ONE_BUTTON"
or icon_type == "ICON_TYPE_SPECIFIC_ON_OFF_WALL_SWITCH_TWO_BUTTONS"
or icon_type == "ICON_TYPE_SPECIFIC_ON_OFF_WALL_SWITCH_THREE_BUTTONS"
or icon_type == "ICON_TYPE_SPECIFIC_ON_OFF_WALL_SWITCH_FOUR_BUTTONS"
or icon_type == "ICON_TYPE_SPECIFIC_ON_OFF_WALL_SWITCH_ONE_ROTARY" then
    return {
        name = "In Wall Switch",
        category = "switch",
        subcategory = "in_wall"
    } 
end
