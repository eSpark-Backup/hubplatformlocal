local function convert_event_to_value(event)
    local battery_level = hubplugins.weak_require("HUB:zwave/scripts/resources/battery_level")

    local data = {
        power_state = "power_applied",
        power_has_been_applied = "power_applied",
        ac_mains_disconnected = "disconnected",
        ac_mains_reconnected = "connected",
        surge_detected = "surge_detected",
        voltage_drop_drift = "drop_drift_detected",
        over_current_detected = "over_current_detected",
        over_voltage_detected = "over_voltage_detected",
        over_load_detected = "over_load_detected",
        load_error = "load_error",
        replace_battery_soon = "replace_battery_soon",
        replace_battery_now = "replace_battery_now",
        battery_fluid_is_low = "battery_fluid_is_low",
        battery_is_charging = "in_progress",
        battery_is_fully_charged = battery_level.fully_charged,
        charge_battery_soon = battery_level.upper_low,
        charge_battery_now = battery_level.low,
        backup_battery_is_low = battery_level.low,
        backup_battery_is_disconnected = battery_level.uncharged,
    }

    local value = assert(data[event], "Unsupported power_manager alarm event: " .. tostring(event))
    return value
end


return {
    item_tag = require("HUB:zwave/scripts/resources/item_descriptor_tags").power_management,

    items_data = {
        power_state = {
            can_be_reset_by_idle_event = true,
            value_after_reset = "no_power_applied",
            initial_enum = { "no_power_applied" },
            get_enum_values = function (event_type) return { convert_event_to_value(event_type) } end,
            get_value = function (event_type, notification_value) return convert_event_to_value(event_type) end,
            callback = function (handler, node_id, channel_id, event_type, notification_value)
                    handler:call("process_power_applied_notification", node_id, channel_id, event_type, notification_value)
            end
        },
        ac_state = {
            get_enum_values = function (event_type) return { convert_event_to_value(event_type) } end,
            get_value = function (event_type, notification_value) return convert_event_to_value(event_type) end
        },
        power_surge_state = {
            should_reset_immediately = true,
            value_after_reset = "no_surge",
            get_enum_values = function (event_type) return { convert_event_to_value(event_type) } end,
            get_value = function (event_type, notification_value) return convert_event_to_value(event_type) end
        },
        voltage_drop_drift_state = {
            should_reset_on_timeout = true,
            value_after_reset = "no_drop_drift",
            initial_enum = { "no_drop_drift" },
            get_enum_values = function (event_type) return { convert_event_to_value(event_type) } end,
            get_value = function (event_type, notification_value) return convert_event_to_value(event_type) end
        },
        over_current_state = {
            can_be_reset_by_idle_event = true,
            value_after_reset = "no_over_current",
            initial_enum = { "no_over_current" },
            get_enum_values = function (event_type) return { convert_event_to_value(event_type) } end,
            get_value = function (event_type, notification_value) return convert_event_to_value(event_type) end
        },
        over_voltage_state = {
            can_be_reset_by_idle_event = true,
            value_after_reset = "no_over_voltage",
            initial_enum = { "no_over_voltage" },
            get_enum_values = function (event_type) return { convert_event_to_value(event_type) } end,
            get_value = function (event_type, notification_value) return convert_event_to_value(event_type) end
        },
        over_load_state = {
            can_be_reset_by_idle_event = true,
            value_after_reset = "no_over_load",
            initial_enum = { "no_over_load" },
            get_enum_values = function (event_type) return { convert_event_to_value(event_type) } end,
            get_value = function (event_type, notification_value) return convert_event_to_value(event_type) end
        },
        load_error_state = {
            can_be_reset_by_idle_event = true,
            value_after_reset = "no_load_error",
            initial_enum = { "no_load_error" },
            get_enum_values = function (event_type) return { convert_event_to_value(event_type) } end,
            get_value = function (event_type, notification_value) return convert_event_to_value(event_type) end
        },
        battery_maintenance_state = {
            can_be_reset_by_idle_event = true,
            value_after_reset = "battery_maintenance_ok",
            initial_enum = { "battery_maintenance_ok" },
            get_enum_values = function (event_type) return { convert_event_to_value(event_type) } end,
            get_value = function (event_type, notification_value) return convert_event_to_value(event_type) end
        },
        battery_charging_state = {
            can_be_reset_by_idle_event = true,
            value_after_reset = "no_charging",
            initial_enum = { "no_charging" },
            get_enum_values = function (event_type) return { convert_event_to_value(event_type) } end,
            get_value = function (event_type, notification_value) return convert_event_to_value(event_type) end
        },
        battery = {
            can_be_reset_by_idle_event = true,
            value_after_reset = 70,
            get_value = function (event_type, notification_value) return convert_event_to_value(event_type) end
        },
        battery_backup = {
            can_be_reset_by_idle_event = true,
            value_after_reset = 100,
            get_value = function (event_type, notification_value) return convert_event_to_value(event_type) end
        },
    },

    events_to_item_names = {
        power_has_been_applied = "power_state",
        ac_mains_disconnected = "ac_state",
        ac_mains_reconnected = "ac_state",
        surge_detected = "power_surge_state",
        voltage_drop_drift = "voltage_drop_drift_state",
        over_current_detected = "over_current_state",
        over_voltage_detected = "over_voltage_state",
        over_load_detected = "over_load_state",
        load_error = "load_error_state",
        replace_battery_soon = "battery_maintenance_state",
        replace_battery_now = "battery_maintenance_state",
        battery_fluid_is_low = "battery_maintenance_state",
        battery_is_charging = "battery_charging_state",
        battery_is_fully_charged = "battery",
        charge_battery_soon = "battery",
        charge_battery_now = "battery",
        backup_battery_is_low = "battery_backup",
        backup_battery_is_disconnected = "battery_backup",
    },
}
