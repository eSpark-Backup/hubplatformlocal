local base_token_item_handler_mt = require("HUB:zwave/scripts/resources/notifications/item_handlers/base_token_item_handler_mt")

return {
    item_tag = require("HUB:zwave/scripts/resources/item_descriptor_tags").siren,

    items_data = {
        siren_alarm = setmetatable( { 
                value_after_reset = "siren_inactive",
                can_be_reset_by_idle_event = true,
            }, 
            base_token_item_handler_mt )
    },

    events_to_item_names = {
        siren_active = "siren_alarm",
    },
}
