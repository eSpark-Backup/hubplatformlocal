local helper = require("HUB:zwave/scripts/resources/enums/enum_helper")

local token_to_value = {
    heating = 0x01,
    cooling = 0x02,
    furnace = 0x07,
    dry_air = 0x08,
    moist_air = 0x09,
    auto_changeover = 0x0A,
    energy_save_heating = 0x0B,
    energy_save_cooling = 0x0C,
    away_heating = 0x0D,
    away_cooling = 0x0E,
    full_power = 0x0F,
}

return helper.make_enum(token_to_value)
