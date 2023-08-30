local base_token_item_handler_mt = require("HUB:zwave/scripts/resources/notifications/item_handlers/base_token_item_handler_mt")

return {
    item_tag = require("HUB:zwave/scripts/resources/item_descriptor_tags").co2_alarm,

    items_data = {
        co2_alarm = setmetatable( {
                can_be_reset_by_idle_event = true, 
                value_after_reset = "no_co2",
            }, 
            base_token_item_handler_mt ),
        test_state = setmetatable( {
                should_reset_on_timeout = true, 
                value_after_reset = "no_test",
                get_enum_values = function (event_type) return { "test_ok", "test_failed" } end,
                get_value = function (event_type, notification_value) return notification_value end,
            }, 
            base_token_item_handler_mt ),
        maintenance_state = hubplugins.weak_require("HUB:zwave/scripts/resources/notifications/item_handlers/maintenance_state"),
        sounding_mode = hubplugins.weak_require("HUB:zwave/scripts/resources/notifications/item_handlers/sounding_mode"),
        periodic_inspection_state = hubplugins.weak_require("HUB:zwave/scripts/resources/notifications/item_handlers/periodic_inspection_state")
    },

    events_to_item_names = {
        co2_detected = "co2_alarm",
        carbon_dioxide_test = "test_state",
        replacement_required = "maintenance_state",
        device_end_of_life = "maintenance_state",
        alarm_silenced = "sounding_mode",
        maintenance_required_planned_periodic_inspection = "periodic_inspection_state",
    },
}
