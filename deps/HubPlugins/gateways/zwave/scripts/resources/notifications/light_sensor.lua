local base_token_item_handler_mt = require("HUB:zwave/scripts/resources/notifications/item_handlers/base_token_item_handler_mt")

return {
    item_tag = require("HUB:zwave/scripts/resources/item_descriptor_tags").light_sensor,

    items_data = {
        light_alarm = setmetatable( { 
                value_after_reset = "no_light",
                can_be_reset_by_idle_event = true,
            }, 
            base_token_item_handler_mt ),
        light_color_transition = setmetatable( { 
                value_after_reset = "no_light_color_transition",
                should_reset_on_timeout = true,
            }, 
            base_token_item_handler_mt ),
    },

    events_to_item_names = {
        light_detected = "light_alarm",
        light_color_transition_detected = "light_color_transition",
    },
}
