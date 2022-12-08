local generic_type, specific_type, ccs = ...

local cc_ids = require("HUB:zwave/scripts/resources/class_ids")

if (generic_type == "GENERIC_TYPE_SWITCH_BINARY" 
    and (   specific_type == "SPECIFIC_TYPE_NOT_USED"
         or specific_type == "SPECIFIC_TYPE_POWER_SWITCH_BINARY"
         or specific_type == "SPECIFIC_TYPE_SCENE_SWITCH_BINARY"
    )
) 
or (generic_type == "GENERIC_TYPE_SWITCH_REMOTE" 
    and ( (specific_type == "SPECIFIC_TYPE_NOT_USED" and (ccs[cc_ids.switch_binary] or ccs[cc_ids.switch_binary_toggle]))
        or specific_type == "SPECIFIC_TYPE_SWITCH_REMOTE_BINARY"
        or specific_type == "SPECIFIC_TYPE_SWITCH_REMOTE_TOGGLE_BINARY"
    )
)
or (generic_type == "GENERIC_TYPE_SWITCH_TOGGLE" 
    and ( (specific_type == "SPECIFIC_TYPE_NOT_USED" and ccs[cc_ids.switch_binary_toggle])
        or specific_type == "SPECIFIC_TYPE_SWITCH_TOGGLE_BINARY"
    )
) then
    return {
        name = "In Wall Switch",
        category = "switch",
        subcategory = "in_wall"
    }
end
