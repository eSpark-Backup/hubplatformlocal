local helper = require("HUB:zwave/scripts/resources/enums/enum_helper")

local token_to_value = {
    ["auto_low"] = 0x00,
    ["low"] = 0x01,
    ["auto_high"] = 0x02,
    ["high"] = 0x03,
    ["auto_medium"] = 0x04,
    ["medium"] = 0x05,
    ["circulation"] = 0x06,
    ["humidity_circulation"] = 0x07,
    ["left_and_right"] = 0x08,
    ["up_and_down"] = 0x09,
    ["quiet"] = 0x0A
}

return helper.make_enum(token_to_value)
