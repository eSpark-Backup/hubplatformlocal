local helper = require("HUB:zwave/scripts/resources/enums/enum_helper")

local token_to_value = {
    ["off"] = 0x00,
    ["heat"] = 0x01,
    ["cool"] = 0x02,
    ["auto"] = 0x03,
    ["aux_mode"] = 0x04,
    ["resume"] = 0x05,
    ["fan_only"] = 0x06,
    ["furnace"] = 0x07,
    ["dry_air"] = 0x08,
    ["moist_air"] = 0x09,
    ["auto_change_over"] = 0x0A,
    ["energy_saving_heat"] = 0x0B,
    ["energy_saving_cool"] = 0x0C,
    ["away"] = 0x0D,
    ["reserved"] = 0x0E,
    ["full_power"] = 0x0F,
    ["manufacturer_specific"] = 0x1F
}

return helper.make_enum(token_to_value)
