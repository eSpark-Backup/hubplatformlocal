local function convert_event_to_value(event)
    local data = {
        system_hardware_failure = "hardware_failure",
        system_software_failure = "software_failure",
        emergency_shutoff = "emergency_shutoff",
        digital_input_high_state = "digital_input_high_state",
        digital_input_low_state = "digital_input_low_state",
        digital_input_open = "digital_input_open",
    }

    local value = assert(data[event], "Unsupported system notification event: " .. tostring(event))
    return value
end

return {
    item_tag = require("HUB:zwave/scripts/resources/item_descriptor_tags").system,

    items_data = {
        hw_state = {
            can_be_reset_by_idle_event = true,
            value_after_reset = "hardware_ok",
            initial_enum = { "hardware_ok" },
            get_enum_values = function (event_type) return { convert_event_to_value(event_type) } end,
            get_value = function (event_type, notification_value) return convert_event_to_value(event_type) end,
        },
        sw_state = {
            can_be_reset_by_idle_event = true,
            value_after_reset = "software_ok",
            initial_enum = { "software_ok" },
            get_enum_values = function (event_type) return { convert_event_to_value(event_type) } end,
            get_value = function (event_type, notification_value) return convert_event_to_value(event_type) end,
        },
        emergency_shutoff = {
            can_be_reset_by_idle_event = true,
            value_after_reset = "state_ok",
            initial_enum = { "state_ok" },
            get_enum_values = function (event_type) return { convert_event_to_value(event_type) } end,
            get_value = function (event_type, notification_value) return convert_event_to_value(event_type) end,
        },
        digital_input_state = {
            get_enum_values = function (event_type) return { convert_event_to_value(event_type) } end,
            get_value = function (event_type, notification_value) return convert_event_to_value(event_type) end,
        },
        tampering_cover_alarm = hubplugins.weak_require("HUB:zwave/scripts/resources/notifications/item_handlers/tampering_cover_alarm"),
    },

    events_to_item_names = {
        system_hardware_failure = "hw_state",
        system_software_failure = "sw_state",
        --heartbeat = "",
        product_cover_removed = "tampering_cover_alarm",
        emergency_shutoff = "emergency_shutoff",
        digital_input_high_state = "digital_input_state",
        digital_input_low_state = "digital_input_state",
        digital_input_open = "digital_input_state",
    },
}
