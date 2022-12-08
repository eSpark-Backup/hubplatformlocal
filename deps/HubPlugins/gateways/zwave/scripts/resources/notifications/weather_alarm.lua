local function convert_event_to_value(event)
    local data = {
        rain = "rain_detected",
        moisture = "moisture_detected",
        freeze = "freeze_detected",
    }

    local value = assert(data[event], "Unsupported weather alarm event: " .. tostring(event))
    return value
end

return {
    item_tag = require("HUB:zwave/scripts/resources/item_descriptor_tags").weather_alarm,

    items_data = {
        rain_alarm = {
            can_be_reset_by_idle_event = true,
            value_after_reset = "no_rain",
            initial_enum = { "no_rain" },
            get_enum_values = function (event_type) return { convert_event_to_value(event_type) } end,
            get_value = function (event_type, notification_value) return convert_event_to_value(event_type) end
        },
        moisture_alarm = {
            can_be_reset_by_idle_event = true,
            value_after_reset = "no_moisture",
            initial_enum = { "no_moisture" },
            get_enum_values = function (event_type) return { convert_event_to_value(event_type) } end,
            get_value = function (event_type, notification_value) return convert_event_to_value(event_type) end
        },
        freeze_alarm = {
            can_be_reset_by_idle_event = true,
            value_after_reset = "no_freeze",
            initial_enum = { "no_freeze" },
            get_enum_values = function (event_type) return { convert_event_to_value(event_type) } end,
            get_value = function (event_type, notification_value) return convert_event_to_value(event_type) end
        },
    },

    events_to_item_names = {
        rain = "rain_alarm",
        moisture = "moisture_alarm",
        freeze = "freeze_alarm",
    },
}
