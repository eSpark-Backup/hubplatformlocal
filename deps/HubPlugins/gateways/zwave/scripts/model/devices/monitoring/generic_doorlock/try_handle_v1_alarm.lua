local self, node_id, channel_id, class_id, v1_alarm_type, v1_alarm_level = ...


local alarm_type_handlers = {
    [0x09] = "handle_lock_jammed",
    [0x11] = "handle_manual_not_fully_locked_operation",
    [0x12] = "handle_keypad_lock_operation",
    [0x13] = "handle_keypad_unlock_operation",
    [0x15] = "handle_manual_lock_operation",
    [0x16] = "handle_manual_unlock_operation",
    [0x17] = "handle_rf_not_fully_locked_operation",
    [0x18] = "handle_rf_lock_operation",
    [0x19] = "handle_rf_unlock_operation",
    [0x1A] = "handle_auto_lock_not_fully_locked_operation",
    [0x1B] = "handle_auto_lock_locked_operation",
    [0x21] = "handle_single_user_code_deleted",
    [0x22] = "handle_new_user_code_not_added_due_to_duplicated_code",
    [0x26] = "handle_non_access",
    [0x29] = "handle_lever_rotated",
    [0x2A] = "handle_deadbolt_unsecured",
    [0x2B] = "handle_dw_state",
    [0x60] = "handle_daily_repeating_schedule_set",
    [0x61] = "handle_daily_repeating_schedule_enable",
    [0x62] = "handle_year_day_schedule_set",
    [0x63] = "handle_year_day_schedule_enable",
    [0x64] = "handle_all_schedule_types_erased",
    [0x65] = "handle_all_schedule_type_enable",
    [0x70] = "handle_new_user_code_added",
    [0x71] = "handle_new_user_code_not_added_due_to_duplicated_code",
    [0x81] = "handle_lock_handling_completed",
    [0x82] = "handle_rf_module_power",
    [0x83] = "handle_disabled_user_entered_at_keypad",
    [0x84] = "handle_valid_user_but_outside_of_schedule",
    [0xA1] = "handle_manually_enter_access_code_exceeds_code_limit",
    [0xA2] = "handle_manually_enter_master_code_disabled",
    [0xA7] = "handle_low_battery_level",
    [0xA8] = "handle_critical_battery_level",
    [0xA9] = "handle_battery_level_too_low_to_operate_lock",
}

local alarm_handler = assert(alarm_type_handlers[v1_alarm_type], "Unknown v1 alarm type " .. v1_alarm_type)

if not alarm_handler then
    print("[generic_doorlock.try_handle_v1_alarm] unknown v1 alarm type " .. tostring(v1_alarm_type))
    return false
else
    self:call(alarm_handler, node_id, channel_id, class_id, math.floor(v1_alarm_level))
    return true
end
