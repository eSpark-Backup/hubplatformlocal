local item_tag_name =  require("HUB:zwave/scripts/resources/item_descriptor_tags").water_alarm
hubplugins.try_collectgarbage()

local base_token_item_handler_mt = require("HUB:zwave/scripts/resources/notifications/item_handlers/base_token_item_handler_mt")

return {
    item_tag = item_tag_name,

    items_data = {
        water_leak_alarm = setmetatable({
                can_be_reset_by_idle_event = true,
                value_after_reset = "no_water_leak",
            }, 
            base_token_item_handler_mt),
         water_filter_replacement_alarm = setmetatable({
                can_be_reset_by_idle_event = true,
                value_after_reset = "water_filter_ok",
            }, 
            base_token_item_handler_mt),
         water_flow_alarm = setmetatable({
                can_be_reset_by_idle_event = true,
                value_after_reset = "water_flow_ok",
                get_enum_values = function (event_type)
                    return { "water_flow_below_low_threshold", "water_flow_above_high_threshold", "water_flow_max" }
                end,
                get_value = function (event_type, notification_value) return notification_value end
            }, 
            base_token_item_handler_mt),
        water_pressure_alarm = setmetatable({
                can_be_reset_by_idle_event = true,
                value_after_reset = "water_pressure_ok",
                get_enum_values = function (event_type)
                    return { "water_pressure_below_low_threshold", "water_pressure_above_high_threshold", "water_pressure_max" }
                end,
                get_value = function (event_type, notification_value) return notification_value end
            }, 
            base_token_item_handler_mt),
         water_temperature_alarm = setmetatable({
                can_be_reset_by_idle_event = true,
                value_after_reset = "water_temperature_ok",
                get_enum_values = function (event_type)
                    return { "water_temperature_below_low_threshold", "water_temperature_above_high_threshold" }
                end,
                get_value = function (event_type, notification_value) return notification_value end
            }, 
            base_token_item_handler_mt),
         water_level_alarm = setmetatable({
                can_be_reset_by_idle_event = true,
                value_after_reset = "water_level_ok",
                get_enum_values = function (event_type)
                    return { "water_level_below_low_threshold", "water_level_above_high_threshold" }
                end,
                get_value = function (event_type, notification_value) return notification_value end
            }, 
            base_token_item_handler_mt),
         water_pump_state = setmetatable({
                can_be_reset_by_idle_event = true,
                value_after_reset = "pump_idle",
            }, 
            base_token_item_handler_mt)
    },

    events_to_item_names = {
        water_leak_detected = "water_leak_alarm",
        replace_water_filter = "water_filter_replacement_alarm",
        water_flow_alarm = "water_flow_alarm",
        water_pressure_alarm = "water_pressure_alarm",
        water_temperature_alarm = "water_temperature_alarm",
        water_level_alarm = "water_level_alarm",
        -- water_level_dropped
        sump_pump_active = "water_pump_state",
        sump_pump_failure = "water_pump_state"
    },
}
