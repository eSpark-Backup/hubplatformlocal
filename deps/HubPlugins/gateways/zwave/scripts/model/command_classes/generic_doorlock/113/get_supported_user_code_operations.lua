local self = ...

local lock_operation_enum = hubplugins.weak_require("HUB:zwave/scripts/resources/enums/user_code_operation_event")

return {
    lock_operation_enum.all_user_codes_deleted,
    lock_operation_enum.single_user_code_deleted,
    lock_operation_enum.new_user_code_added,
    lock_operation_enum.new_user_code_not_added_due_to_duplicated_code
}
