local base_token_item_handler_mt = require("HUB:zwave/scripts/resources/notifications/item_handlers/base_token_item_handler_mt")

return {
    item_tag = require("HUB:zwave/scripts/resources/item_descriptor_tags").access_control,

    items_data = {
        user_code_operation = setmetatable( {
                should_reset_immediately = true,
                value_after_reset = "no_operation",
                initial_enum = {},
                callback = function (handler, node_id, channel_id, event_type, notification_value)
                    handler:call("process_user_code_notification", node_id, channel_id, event_type, notification_value)
                end
            },
            base_token_item_handler_mt ),
        keypad_state = setmetatable( {
                can_be_reset_by_idle_event = true,
                value_after_reset = "keypad_idle",
            },
            base_token_item_handler_mt ),
        lock_operation = setmetatable( {
                callback = function (handler, node_id, channel_id, event_type, notification_value)
                    handler:call("request_door_lock_mode", node_id, channel_id)
                end
            },
            base_token_item_handler_mt ),
        dw_handle_state = setmetatable({}, base_token_item_handler_mt),
        dw_state = setmetatable({}, base_token_item_handler_mt),
        barrier_problem_sensors = {
            can_be_reset_by_idle_event = true,
            dictionary_value_after_reset = function(old_value, opt_event_type) return {} end,
            get_value = function (event_type, notification_value)
                local sensors_enum = hubplugins.weak_require("HUB:zwave/scripts/resources/enums/barrier_problem_sensors")
                local value
                if event_type == "barrier_sensor_not_detected" then
                    value = sensors_enum.not_detected
                elseif event_type == "barrier_sensor_low_battery_warning" then
                    value = sensors_enum.low_battery
                else
                    error("Unknown barrier sensor event")
                end
                return { [notification_value] = value }
            end
        },
        barrier_fail_events = setmetatable( {
                should_reset_immediately = true,
                value_after_reset = "no_barrier_fails",
                initial_enum = {},
            },
            base_token_item_handler_mt ),
        barrier_unattended_operation = {
            can_be_reset_by_idle_event = true,
            value_after_reset = "unattended_operation_enabled",
            get_enum_values = function (event_type)
                local barrier_unattended_operation_enum = hubplugins.weak_require("HUB:zwave/scripts/resources/enums/barrier_unattended_operation")
                if event_type == "barrier_unattended_operation_has_been_disabled_per_ul_requirements" then
                    return { barrier_unattended_operation_enum.unattended_operation_disabled_per_ul_requirements }
                else
                    error("Unknown barrier unattended operation event")
                end
            end,
            get_value = function (event_type, notification_value)
                local barrier_unattended_operation_enum = hubplugins.weak_require("HUB:zwave/scripts/resources/enums/barrier_unattended_operation")
                if event_type == "barrier_unattended_operation_has_been_disabled_per_ul_requirements" then
                    return barrier_unattended_operation_enum.unattended_operation_disabled_per_ul_requirements
                else
                    error("Unknown barrier unattended operation event")
                end
            end,
        },
        user_lock_operation = {
            should_reset_on_timeout = true,
            value_after_reset = {
                userId = -1,
                action = "unknown"
            },
            get_value = function (event_type, notification_value)
                local user_lock_operation_enum = hubplugins.weak_require("HUB:zwave/scripts/resources/enums/user_lock_operation")
                return {
                    userId = notification_value,
                    action = user_lock_operation_enum[event_type]
                }
            end,
            callback = function (handler, node_id, channel_id, event_type, notification_value)
                if event_type == "valid_user_but_outside_of_schedule_operation" then
                    local zwave = require("zwave")
                    zwave.synchronize_device_time(node_id)
                end
            end
        },
    },

    events_to_item_names = {
        manual_lock_operation = "lock_operation",
        manual_unlock_operation = "lock_operation",
        rf_lock_operation = "lock_operation",
        rf_unlock_operation = "lock_operation",
        keypad_lock_operation = { "lock_operation", "user_lock_operation" },
        keypad_unlock_operation = { "lock_operation", "user_lock_operation" },
        valid_user_but_outside_of_schedule_operation = { "lock_operation", "user_lock_operation" },
        manual_not_fully_locked_operation = "lock_operation",
        rf_not_fully_locked_operation = "lock_operation",
        auto_lock_locked_operation = "lock_operation",
        auto_lock_not_fully_locked_operation = "lock_operation",
        all_user_codes_deleted = "user_code_operation",
        single_user_code_deleted = "user_code_operation",
        new_program_code_entered = "user_code_operation",
        new_user_code_added = "user_code_operation",
        keypad_busy = "keypad_state",
        keypad_temporary_disabled = "keypad_state",
        new_user_code_not_added_due_to_duplicated_code = "user_code_operation",
        unlock_by_rf_with_invalid_user_code = "lock_operation",
        lock_by_rf_with_invalid_user_code = "lock_operation",
        lock_jammed = "lock_operation",
        dw_is_opened = "dw_state",
        dw_is_closed = "dw_state",
        dw_handle_is_opened = "dw_handle_state",
        dw_handle_is_closed = "dw_handle_state",
        barrier_sensor_not_detected = "barrier_problem_sensors",
        barrier_sensor_low_battery_warning = "barrier_problem_sensors",
        barrier_operation_force_has_been_exceeded = "barrier_fail_events",
        barrier_motor_exceeded_operational_time_limit = "barrier_fail_events",
        barrier_operation_exceeded_physical_mechanical_limits = "barrier_fail_events",
        barrier_failed_to_perform_requested_operation_device_malfunction = "barrier_fail_events",
        barrier_unable_to_perform_requested_operation_due_to_ul_requirements = "barrier_fail_events",
        barrier_unattended_operation_has_been_disabled_per_ul_requirements = "barrier_unattended_operation",
    },
}
