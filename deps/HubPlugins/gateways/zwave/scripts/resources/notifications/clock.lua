local base_token_item_handler_mt = require("HUB:zwave/scripts/resources/notifications/item_handlers/base_token_item_handler_mt")

return {
    item_tag = require("HUB:zwave/scripts/resources/item_descriptor_tags").clock,

    items_data = {
        clock_state = setmetatable( {
                should_reset_on_timeout = true, 
                value_after_reset = "no_state",
            }, 
            base_token_item_handler_mt ),
        remaining_time = {
            get_value = function (event_type, notification_value)
                return {
                    value = notification_value.hours*3600 + notification_value.minutes*60 + notification_value.seconds,
                    scale = "seconds",
                }
            end,
        },
    },

    events_to_item_names = {
        wake_up_alert = "clock_state",
        time_ended = "clock_state",
        time_remaining = "remaining_time",
    },
}
