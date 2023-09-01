local generic_type, specific_type, ccs = ...

local cc_ids = require("HUB:zwave/scripts/resources/class_ids")

if (generic_type == "GENERIC_TYPE_SWITCH_MULTILEVEL" 
    and (  (specific_type == "SPECIFIC_TYPE_NOT_USED" and ccs[cc_ids.window_covering])
         or specific_type == "SPECIFIC_TYPE_CLASS_A_MOTOR_CONTROL"
         or specific_type == "SPECIFIC_TYPE_CLASS_B_MOTOR_CONTROL"
         or specific_type == "SPECIFIC_TYPE_CLASS_C_MOTOR_CONTROL"
         or specific_type == "SPECIFIC_TYPE_MOTOR_MULTIPOSITION"
         or specific_type == "SPECIFIC_TYPE_SECURE_LOCKBOX"
    )
) 
or (generic_type == "GENERIC_TYPE_WINDOW_COVERING" 
    and (  specific_type == "SPECIFIC_TYPE_NOT_USED"
        or specific_type == "SPECIFIC_TYPE_SIMPLE_WINDOW_COVERING"
    )
) then
    return {
        name = "Window Covering",
        category = "window_cov",
        subcategory = "window_cov"
    }
end
