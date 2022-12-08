return {
    manual_lock = "manual_lock_operation",         -- Door locked manually using the key or inside thumb-turn
    manual_unlock = "manual_unlock_operation",     -- Door un-locked manually by key or inside thumb-turn
    keypad_lock = "keypad_lock_operation",         -- Door locked using the touch function
    keypad_unlock = "keypad_unlock_operation",     -- Door un-locked using PIN code
    rf_lock = "rf_lock_operation",                 -- Door locked from the UI
    rf_unlock = "rf_unlock_operation",             -- Door un-locked from the UI

    manual_not_fully_locked = "manual_not_fully_locked_operation",
    rf_not_fully_locked = "rf_not_fully_locked_operation",
    auto_lock_not_fully_locked = "auto_lock_not_fully_locked_operation",
    auto_lock_locked = "auto_lock_locked_operation",

    unlock_by_rf_with_invalid_user_code = "unlock_by_rf_with_invalid_user_code",
    lock_by_rf_with_invalid_user_code = "lock_by_rf_with_invalid_user_code",
    lock_jammed = "lock_jammed",

    -- Specific alarms (notification v1 events)
    valid_user_but_outside_of_schedule = "valid_user_but_outside_of_schedule_operation"
}
