local generic_type, specific_type, ccs = ...

local cc_ids = require("HUB:zwave/scripts/resources/class_ids")

if (generic_type == "GENERIC_TYPE_ENTRY_CONTROL" 
    and (  (specific_type == "SPECIFIC_TYPE_NOT_USED" and ccs[cc_ids.barrier_operator])
         or specific_type == "SPECIFIC_TYPE_SECURE_DOOR"
         or specific_type == "SPECIFIC_TYPE_SECURE_GATE"
         or specific_type == "SPECIFIC_TYPE_SECURE_BARRIER_ADDON"
         or specific_type == "SPECIFIC_TYPE_SECURE_BARRIER_OPEN_ONLY"
         or specific_type == "SPECIFIC_TYPE_SECURE_BARRIER_CLOSE_ONLY"
    )
) then
    return {
        name = "Garage Door",
        category = "garage_door"
    }
end
