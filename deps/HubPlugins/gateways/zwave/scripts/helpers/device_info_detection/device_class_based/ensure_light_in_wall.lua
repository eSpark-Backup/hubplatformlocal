local generic_type, specific_type, ccs = ...

local cc_ids = require("HUB:zwave/scripts/resources/class_ids")

if (generic_type == "GENERIC_TYPE_SWITCH_MULTILEVEL" 
    and (   specific_type == "SPECIFIC_TYPE_NOT_USED"
         or specific_type == "SPECIFIC_TYPE_POWER_SWITCH_MULTILEVEL"
         or specific_type == "SPECIFIC_TYPE_SCENE_SWITCH_MULTILEVEL"
         or specific_type == "SPECIFIC_TYPE_FAN_SWITCH"
    )
) 
or (generic_type == "GENERIC_TYPE_SWITCH_REMOTE" 
    and ( (specific_type == "SPECIFIC_TYPE_NOT_USED" and (ccs[cc_ids.dimmer] or ccs[cc_ids.multilevel_toggle_switch]))
        or specific_type == "SPECIFIC_TYPE_SWITCH_REMOTE_MULTILEVEL"
        or specific_type == "SPECIFIC_TYPE_SWITCH_REMOTE_TOGGLE_MULTILEVEL"
    )
)
or (generic_type == "GENERIC_TYPE_SWITCH_TOGGLE" 
    and ( (specific_type == "SPECIFIC_TYPE_NOT_USED" and ccs[cc_ids.multilevel_toggle_switch])
        or specific_type == "SPECIFIC_TYPE_SWITCH_TOGGLE_MULTILEVEL"
    )
) then
    return {
        name = "In Wall Dimmable",
        category = "dimmable_light",
        subcategory = "dimmable_in_wall"
    }
end
