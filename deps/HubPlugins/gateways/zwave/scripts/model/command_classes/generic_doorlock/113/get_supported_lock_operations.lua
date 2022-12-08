local self, has_schedule = ...

local lock_operation_enum = hubplugins.weak_require("HUB:zwave/scripts/resources/enums/lock_operation_event")

local result = {
    lock_operation_enum.manual_lock,
    lock_operation_enum.manual_unlock,
    lock_operation_enum.keypad_lock,
    lock_operation_enum.keypad_unlock,
    lock_operation_enum.rf_lock,
    lock_operation_enum.rf_unlock,
    lock_operation_enum.lock_jammed,
    lock_operation_enum.auto_lock_locked
}

if has_schedule then
    table.insert(result, lock_operation_enum.valid_user_but_outside_of_schedule)
end

return result
