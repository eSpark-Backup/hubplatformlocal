local item_names = require("HUB:zwave/scripts/resources/item_names")

return {
    name = item_names.master_code,
    value_type = "string",
    value = "****",
    has_getter = true,
    has_setter = true
}
