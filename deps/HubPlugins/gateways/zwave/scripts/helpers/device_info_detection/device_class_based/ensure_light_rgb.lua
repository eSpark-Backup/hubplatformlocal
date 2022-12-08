local generic_type, specific_type, ccs = ...

local cc_ids = require("HUB:zwave/scripts/resources/class_ids")

if (generic_type == "GENERIC_TYPE_SWITCH_MULTILEVEL" 
    and (    specific_type == "SPECIFIC_TYPE_COLOR_TUNABLE_MULTILEVEL"
         or (specific_type == "SPECIFIC_TYPE_POWER_SWITCH_MULTILEVEL" and ccs[cc_ids.switch_color])
    )
) 
or (generic_type == "GENERIC_TYPE_SWITCH_BINARY" 
    and (specific_type == "SPECIFIC_TYPE_COLOR_TUNABLE_BINARY"
    )
) then
    return {
        name = "RGB Dimmable",
        category = "dimmable_light",
        subcategory = "dimmable_colored"
    }
end
