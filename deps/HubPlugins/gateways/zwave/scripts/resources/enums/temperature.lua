local helper = require("HUB:zwave/scripts/resources/enums/enum_helper")

local token_to_value = {
    ["celsius"] = 0,
    ["fahrenheit"] = 1
}

return helper.make_enum(token_to_value)
