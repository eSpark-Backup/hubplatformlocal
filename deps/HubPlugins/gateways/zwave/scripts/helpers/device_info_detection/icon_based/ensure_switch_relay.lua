local icon_type = ...

if icon_type == "ICON_TYPE_SPECIFIC_LIGHT_DIMMER_SWITCH_DIN_RAIL_MODULE"
or icon_type == "ICON_TYPE_SPECIFIC_ON_OFF_POWER_SWITCH_DIN_RAIL_MODULE" then
    return {
        name = "Relay Switch",
        category = "switch",
        subcategory = "relay"
    } 
end
