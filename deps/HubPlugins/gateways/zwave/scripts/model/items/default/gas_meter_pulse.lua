local item_names = require("HUB:zwave/scripts/resources/item_names")
local item_value_type = require("HUB:zwave/scripts/resources/item_value_type")

return {
    name = item_names.gas_meter_pulse,
    has_setter = false,
    has_getter = true,
    value_type = item_value_type.int,
    value = 0
}
