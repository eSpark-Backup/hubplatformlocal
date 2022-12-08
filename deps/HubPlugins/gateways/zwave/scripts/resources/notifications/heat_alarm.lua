local base_token_item_handler_mt = require("HUB:zwave/scripts/resources/notifications/item_handlers/base_token_item_handler_mt")

local item_tag_name = require("HUB:zwave/scripts/resources/item_descriptor_tags").heat_alarm
hubplugins.try_collectgarbage()

local maintenance_state_set = hubplugins.weak_require("HUB:zwave/scripts/resources/notifications/item_handlers/maintenance_state")
hubplugins.try_collectgarbage()

local sounding_mode_set = hubplugins.weak_require("HUB:zwave/scripts/resources/notifications/item_handlers/sounding_mode")
hubplugins.try_collectgarbage()

local periodic_inspection_state_set = hubplugins.weak_require("HUB:zwave/scripts/resources/notifications/item_handlers/periodic_inspection_state")
hubplugins.try_collectgarbage()

return {
    item_tag = item_tag_name,

    items_data = {
        heat_alarm = setmetatable( {
                can_be_reset_by_idle_event = true, 
                value_after_reset = "heat_ok",
            }, 
            base_token_item_handler_mt ),
        temperature_changes = setmetatable( {
                should_reset_on_timeout = true, 
                value_after_reset = "no_changes",
            }, 
            base_token_item_handler_mt ),
        test_state = setmetatable( {
                can_be_reset_by_idle_event = true, 
                value_after_reset = "no_test",
                get_enum_values = function (event_type) return { "test_in_progress" } end,
                get_value = function (event_type, notification_value) return "test_in_progress" end,
            }, 
            base_token_item_handler_mt ), 
        maintenance_state = maintenance_state_set,
        sounding_mode = sounding_mode_set,
        dust_in_device = setmetatable( {
                can_be_reset_by_idle_event = true, 
                value_after_reset = "no_dust",
            }, 
            base_token_item_handler_mt ),
        periodic_inspection_state = periodic_inspection_state_set,
    },

    events_to_item_names = {
        overheat_detected = "heat_alarm",
        under_heat_detected = "heat_alarm",
        rapid_temperature_rise = "temperature_changes",
        rapid_temperature_fail = "temperature_changes",
        heat_alarm_test = "test_state",
        device_end_of_life = "maintenance_state",
        alarm_silenced = "sounding_mode",
        dust_detected = "dust_in_device",
        maintenance_required_planned_periodic_inspection = "periodic_inspection_state",
    },
}
