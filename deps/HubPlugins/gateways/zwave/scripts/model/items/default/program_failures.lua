local item_names = require("HUB:zwave/scripts/resources/item_names")
local program_failed_status_enum = hubplugins.weak_require("HUB:zwave/scripts/resources/enums/program_failed_status")

return {
    name = item_names.program_failures,
    value_type = "dictionary.token",
    enum = {program_failed_status_enum.ok, program_failed_status_enum.failed},
    value = {},
    has_getter = true,
    has_setter = false
}
