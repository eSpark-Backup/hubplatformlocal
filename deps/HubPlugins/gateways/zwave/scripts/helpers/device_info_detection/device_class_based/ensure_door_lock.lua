local generic_type, specific_type, ccs = ...

local cc_ids = require("HUB:zwave/scripts/resources/class_ids")

if (generic_type == "GENERIC_TYPE_ENTRY_CONTROL" 
    and (  (specific_type == "SPECIFIC_TYPE_NOT_USED" and (ccs[cc_ids.entry_control] == nil and ccs[cc_ids.barrier_operator] == nil))
         or specific_type == "SPECIFIC_TYPE_DOOR_LOCK"
         or specific_type == "SPECIFIC_TYPE_ADVANCED_DOOR_LOCK"
         or specific_type == "SPECIFIC_TYPE_SECURE_KEYPAD_DOOR_LOCK"
         or specific_type == "SPECIFIC_TYPE_SECURE_KEYPAD_DOOR_LOCK_DEADBOLT"
         or specific_type == "SPECIFIC_TYPE_SECURE_LOCKBOX"
    )
) then
    return {
        name = "Door Lock",
        category = "door_lock"
    }
end
