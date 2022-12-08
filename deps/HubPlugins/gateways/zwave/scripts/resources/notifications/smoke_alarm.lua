local base_token_item_handler_mt = require("HUB:zwave/scripts/resources/notifications/item_handlers/base_token_item_handler_mt")

return {
    item_tag = require("HUB:zwave/scripts/resources/item_descriptor_tags").smoke_alarm,

    items_data = {
        smoke_alarm = setmetatable(
            {
                can_be_reset_by_idle_event = true,
                value_after_reset = "no_smoke" 
            },
            base_token_item_handler_mt),
        test_state = setmetatable(
            {
                can_be_reset_by_idle_event = true,
                value_after_reset = "no_test",
                get_enum_values = function (event_type) return { "test_in_progress" } end,
                get_value = function (event_type, notification_value) return "test_in_progress" end,
            },
            base_token_item_handler_mt),
        sounding_mode = hubplugins.weak_require("HUB:zwave/scripts/resources/notifications/item_handlers/sounding_mode"),
        maintenance_state = hubplugins.weak_require("HUB:zwave/scripts/resources/notifications/item_handlers/maintenance_state"),
        periodic_inspection_state = hubplugins.weak_require("HUB:zwave/scripts/resources/notifications/item_handlers/periodic_inspection_state"),
        dust_in_device = setmetatable({
                can_be_reset_by_idle_event = true,
                value_after_reset = "no_dust",
            }, 
            base_token_item_handler_mt)
    },

    events_to_item_names = {
        smoke_detected = "smoke_alarm",
        smoke_alarm_test = "test_state",
        alarm_silenced = "sounding_mode",
        replacement_required = "maintenance_state",
        device_end_of_life = "maintenance_state",
        maintenance_required_planned_periodic_inspection = "periodic_inspection_state",
        dust_detected = "dust_in_device"
    },
}
