local item_names = require("HUB:zwave/scripts/resources/item_names")
local item_value_type = require("HUB:zwave/scripts/resources/item_value_type")

return {
    name = item_names.weekly_user_code_intervals,
    value_type = item_value_type.dictionary .. "." .. item_value_type.array .. "." .. item_value_type.weekly_interval,
    value = {},
    has_getter = true,
    has_setter = true
}
