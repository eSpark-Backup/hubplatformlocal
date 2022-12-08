local item_names = require("HUB:zwave/scripts/resources/item_names")

return {
    name = item_names.button_state,
    value_type = "button_state",
    -- value = `released`
    has_getter = true,
    has_setter = false
}
