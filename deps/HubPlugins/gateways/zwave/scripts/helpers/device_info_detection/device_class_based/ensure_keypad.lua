local generic_type, specific_type, ccs = ...

local cc_ids = require("HUB:zwave/scripts/resources/class_ids")

if (generic_type == "GENERIC_TYPE_ENTRY_CONTROL" 
    and (  (specific_type == "SPECIFIC_TYPE_NOT_USED" and ccs[cc_ids.entry_control])
         or specific_type == "SPECIFIC_TYPE_SECURE_KEYPAD"
    )
) 
or (generic_type == "GENERIC_TYPE_SECURITY_PANEL" 
    and (  specific_type == "SPECIFIC_TYPE_NOT_USED"
        or specific_type == "SPECIFIC_TYPE_ZONED_SECURITY_PANEL"
    )
)then
    return {
        name = "Keypad",
        category = "keypad"
    }
end
