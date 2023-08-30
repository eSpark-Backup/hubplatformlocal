local generic_type, specific_type, ccs = ...

local cc_ids = require("HUB:zwave/scripts/resources/class_ids")

if (generic_type == "GENERIC_TYPE_SWITCH_BINARY" 
    and (  (specific_type == "SPECIFIC_TYPE_NOT_USED" and ccs[cc_ids.irrigation])
         or specific_type == "SPECIFIC_TYPE_VALVE_OPEN_CLOSE"
         or specific_type == "SPECIFIC_TYPE_IRRIGATION_CONTROLLER"
    )
) then
    return {
        name = "Valve Switch",
        category = "switch",
        subcategory = "valve"
    }
end
