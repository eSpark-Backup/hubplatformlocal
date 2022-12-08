local item_tag_name =  require("HUB:zwave/scripts/resources/item_descriptor_tags").home_security
hubplugins.try_collectgarbage()

local base_token_item_handler_mt = require("HUB:zwave/scripts/resources/notifications/item_handlers/base_token_item_handler_mt")

return {
    item_tag = item_tag_name,

    items_data = {
        intrusion_alarm = setmetatable( { 
                value_after_reset = "no_intrusion",
                can_be_reset_by_idle_event = true
            }, 
            base_token_item_handler_mt ),
        tampering_cover_alarm = hubplugins.weak_require("HUB:zwave/scripts/resources/notifications/item_handlers/tampering_cover_alarm"),
        tampering_move_alarm = setmetatable( { 
                value_after_reset = "no_moving",
                can_be_reset_by_idle_event = true,
            }, 
            base_token_item_handler_mt ),
        motion = {
            can_be_reset_by_idle_event = true,
            value_after_reset = false,
            get_value = function (event_type, notification_value) return event_type == "motion_detected" end
        },
        glass_breakage_alarm = setmetatable( { 
                value_after_reset = "no_glass_breakage",
                should_reset_on_timeout = true
            }, 
            base_token_item_handler_mt ),
        tampering_impact_alarm = setmetatable( { 
                value_after_reset = "no_impact",
                should_reset_on_timeout = true
            }, 
            base_token_item_handler_mt ),
        tampering_invalid_code_alarm = setmetatable( { 
                value_after_reset = "no_invalid_code",
                should_reset_on_timeout = true
            }, 
            base_token_item_handler_mt )
    },

    events_to_item_names = {
        intrusion = "intrusion_alarm",
        product_cover_removed = "tampering_cover_alarm",
        invalid_code = "tampering_invalid_code_alarm",
        glass_breakage = "glass_breakage_alarm",
        motion_detected = "motion",
        product_moved = "tampering_move_alarm",
        impact_detected = "tampering_impact_alarm"
    },
}
