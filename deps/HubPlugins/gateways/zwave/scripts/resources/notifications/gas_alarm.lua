local base_token_item_handler_mt = require("HUB:zwave/scripts/resources/notifications/item_handlers/base_token_item_handler_mt")

return {
    item_tag = require("HUB:zwave/scripts/resources/item_descriptor_tags").gas_alarm,

    items_data = {
        gas_alarm = setmetatable( {
                can_be_reset_by_idle_event = true, 
                value_after_reset = "no_gas",
            }, 
            base_token_item_handler_mt ),
        test_state = setmetatable( {
                can_be_reset_by_idle_event = true, 
                value_after_reset = "no_test",
                get_enum_values = function (event_type) return { "test_in_progress" } end,
                get_value = function (event_type, notification_value) return "test_in_progress" end,
            }, 
            base_token_item_handler_mt ),
        maintenance_state = hubplugins.weak_require("HUB:zwave/scripts/resources/notifications/item_handlers/maintenance_state"),
    },

    events_to_item_names = {
        combustible_gas_detected = "gas_alarm",
        toxic_gas_detected = "gas_alarm",
        gas_alarm_test = "test_state",
        replacement_required = "maintenance_state",
    },
}
