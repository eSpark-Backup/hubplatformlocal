local helper = require("HUB:zwave/scripts/resources/enums/enum_helper")

local token_to_value = {
    ["idle_off"] = 0,
    ["running_low"] = 1,
    ["running_high"] = 2,
    ["running_medium"] = 3,
    ["circulation_mode"] = 4,
    ["humidity_circulation_mode"] = 5,
    ["right_left_circulation_mode"] = 6,
    ["up_down_circulation_mode"] = 7,
    ["quiet_circulation_mode"] = 8
}

return helper.make_enum(token_to_value)
