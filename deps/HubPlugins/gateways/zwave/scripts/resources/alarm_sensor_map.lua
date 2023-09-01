local item_names = require("HUB:zwave/scripts/resources/item_names")
local item_tags = require("HUB:zwave/scripts/resources/item_descriptor_tags")

local sensor_type_to_item_name = {
    water_alarm = item_names.water_leak_alarm,
    smoke_alarm = item_names.smoke_alarm,
    co_alarm = item_names.co_alarm,
    co2_alarm = item_names.co2_alarm,
    heat_alarm = item_names.heat_alarm
}

local sensor_type_to_item_tag = {
    water_alarm = item_tags.water_alarm,
    smoke_alarm = item_tags.smoke_alarm,
    co_alarm = item_tags.co_alarm,
    co2_alarm = item_tags.co2_alarm,
    heat_alarm = item_tags.heat_alarm
}

return {
    sensor_type_to_item_name = sensor_type_to_item_name,
    sensor_type_to_item_tag = sensor_type_to_item_tag
}
