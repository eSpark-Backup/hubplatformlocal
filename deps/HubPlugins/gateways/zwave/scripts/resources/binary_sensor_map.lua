local item_names = require("HUB:zwave/scripts/resources/item_names")

-- TODO: remove all '*_binary' items
local sensor_type_to_item_name = {
    [0x01] = item_names.sensor_binary,
    [0x02] = item_names.smoke_alarm,
    [0x03] = item_names.co_alarm,
    [0x04] = item_names.co2_alarm,
    [0x05] = item_names.heat_alarm,
    [0x06] = item_names.water_leak_alarm,
    [0x07] = item_names.freeze_alarm,
    [0x08] = item_names.tampering_cover_alarm,
    [0x09] = item_names.aux_binary,
    [0x0A] = item_names.dw_state,
    [0x0B] = item_names.tilt,
    [0x0C] = item_names.motion,
    [0x0D] = item_names.glass_breakage_alarm
}

local item_tags = require("HUB:zwave/scripts/resources/item_descriptor_tags")

-- TODO: define tags
local sensor_type_to_item_tag = {
    --[0x01] = ,
    [0x02] = item_tags.smoke_alarm,
    [0x03] = item_tags.co_alarm,
    [0x04] = item_tags.co2_alarm,
    [0x05] = item_tags.heat_alarm,
    [0x06] = item_tags.water_alarm,
    [0x07] = item_tags.weather_alarm,
    [0x08] = item_tags.home_security,
    --[0x09] = item_names.aux_binary,
    [0x0A] = item_tags.access_control,
    [0x0B] = item_tags.access_control,
    [0x0C] = item_tags.home_security,
    [0x0D] = item_tags.home_security
}

return {
    sensor_type_to_item_name = sensor_type_to_item_name,
    sensor_type_to_item_tag = sensor_type_to_item_tag,
}
