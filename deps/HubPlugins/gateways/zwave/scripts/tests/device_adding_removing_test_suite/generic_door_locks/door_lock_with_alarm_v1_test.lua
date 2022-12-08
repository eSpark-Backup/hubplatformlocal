local tests = require( "tests" )
local zwave = require( "zwave" )
local class_ids = require( "HUB:zwave/scripts/resources/class_ids" )
local item_names = require( "HUB:zwave/scripts/resources/item_names" )
local core = require( "core" )
local lock_operation_enum = hubplugins.weak_require("HUB:zwave/scripts/resources/enums/lock_operation_event")

local fixture = loadfile("HUB:zwave/scripts/tests/fixtures/generic_door_lock_fixture")()

zwave.request_node_values = tests.create_mock()

local test_suite_dir = "HUB:zwave/scripts/tests/device_adding_removing_test_suite/"
local check_zwave_node_adding_result = hubplugins.weak_loadfile( test_suite_dir .. "check_zwave_node_adding_result" )
local check_zwave_node_removing_result = hubplugins.weak_loadfile( test_suite_dir .. "check_zwave_node_removing_result" )
local append_to_table = hubplugins.weak_loadfile("HUB:zwave/scripts/helpers/append_to_table")


local expected_door_lock_item_info = {
    name = item_names.door_lock,
    cc_id = class_ids.door_lock,
    channel_id = 0,
    name = item_names.door_lock,
    cc_id = class_ids.door_lock,
    value = "unknown",
}

local expected_user_code_item_info = {
    name = item_names.user_codes,
    cc_id = class_ids.user_codes,
    channel_id = 0,
    value_type = "dictionary.userCode",
    value = {},
    maxElementsCount = 250
}

local expected_battery_item_info = {
    name = item_names.battery,
    cc_id = class_ids.battery,
    channel_id = 0,
    value_type = "int",
    value = 100
}

local expected_user_codes_scan_progress_item_info = {
    name = item_names.user_codes_scan_progress,
    cc_id = class_ids.user_codes,
    channel_id = 0,
    value_type = "int",
    value = 0
}

local expected_user_lock_operation_item_info = {
    name = item_names.user_lock_operation,
    cc_id = class_ids.notification,
    channel_id = 0,
    value_type = "user_lock_operation",
    value = tests.skipped_param,
    tag = "access_control"
}

local expected_schedule_item_info = {
    name = item_names.weekly_user_code_intervals,
    cc_id = class_ids.schedule_entry_lock,
    channel_id = 0,
    value_type = tests.skipped_param,
    value = tests.skipped_param
}

local expected_virtual_week_schedule_item_info = {
    name = item_names.daily_user_code_intervals,
    cc_id = class_ids.user_codes,
    channel_id = 0,
    value_type = tests.skipped_param,
    value = tests.skipped_param
}

local expected_virtual_day_schedule_item_info = {
    name = item_names.weekly_user_code_intervals,
    cc_id = class_ids.user_codes,
    channel_id = 0,
    value_type = tests.skipped_param,
    value = tests.skipped_param
}

local expected_keypad_state_item_info = {
    name = item_names.keypad_state,
    cc_id = class_ids.notification,
    channel_id = 0,
    value = tests.skipped_param,
    tag = "access_control"
}

local function get_expected_user_code_operation_item_info(...)
    local expected_user_code_operation_item_info = {
        name = item_names.user_code_operation,
        cc_id = class_ids.notification,
        channel_id = 0,
        value_type = "token",
        value = "no_operation",
        enum = {
            "no_operation",
            "all_user_codes_deleted",
            "single_user_code_deleted",
            "new_user_code_added",
            "new_user_code_not_added_due_to_duplicated_code"
        },
        tag = "access_control"
    }

    append_to_table(expected_user_code_operation_item_info.enum, ...)

    return expected_user_code_operation_item_info
end

local function get_expected_lock_operation_item_info(...)
    local expected_lock_operation_item_info = {
        name = item_names.lock_operation,
        cc_id = class_ids.notification,
        channel_id = 0,
        value_type = "token",
        value = "unknown",
        enum = {
            "unknown",
            "manual_lock_operation",
            "manual_unlock_operation",
            "keypad_lock_operation",
            "keypad_unlock_operation",
            "rf_lock_operation",
            "rf_unlock_operation",
            "lock_jammed",
            "auto_lock_locked_operation"
        },
        tag = "access_control"
    }

    append_to_table(expected_lock_operation_item_info.enum, ...)

    return expected_lock_operation_item_info
end


local function test_impl(device_info, cc_config)
    -- alarm v1, schedule
    do
        fixture:create_and_add_zwave_node(
                    device_info,
                    {
                        notification_version = 1,
                        alarm_v1_supported = true,
                        has_schedule = true,
                        has_power_management_notifications = false
                    })

        local expected_devices = {
            {
                name = tests.skipped_param,
                category = "door_lock",
                items = {
                    expected_door_lock_item_info,
                    expected_user_code_item_info,
                    expected_battery_item_info,
                    expected_user_codes_scan_progress_item_info,
                    expected_schedule_item_info,
                    expected_user_lock_operation_item_info,
                    get_expected_user_code_operation_item_info(),
                    get_expected_lock_operation_item_info("valid_user_but_outside_of_schedule_operation")
                },
                node_id = "9000",
                battery_powered = true
            }
        }

        local check_context = {}
        check_zwave_node_adding_result(expected_devices, check_context)

        fixture:remove_node()

        check_zwave_node_removing_result(check_context)
    end

    -- notification v8, has_schedule
    do
        fixture:create_and_add_zwave_node(
                    device_info,
                    {
                        notification_version = 8,
                        alarm_v1_supported = true,
                        has_schedule = true,
                        has_power_management_notifications = false
                    })

        local expected_devices = {
            {
                name = tests.skipped_param,
                category = "door_lock",
                items = {
                    expected_door_lock_item_info,
                    expected_user_code_item_info,
                    expected_battery_item_info,
                    expected_user_codes_scan_progress_item_info,
                    expected_schedule_item_info,
                    expected_user_lock_operation_item_info,
                    expected_keypad_state_item_info,
                    get_expected_user_code_operation_item_info("new_program_code_entered"),
                    get_expected_lock_operation_item_info("valid_user_but_outside_of_schedule_operation")
                },
                node_id = "9000",
                battery_powered = true
            }
        }

        local check_context = {}
        check_zwave_node_adding_result(expected_devices, check_context)

        fixture:remove_node()

        check_zwave_node_removing_result(check_context)
    end

    -- alarm v1, no schedule
    do
        fixture:create_and_add_zwave_node(
                    device_info,
                    {
                        notification_version = 1,
                        alarm_v1_supported = true,
                        has_schedule = false,
                        has_power_management_notifications = false
                    })

        local expected_devices = {
            {
                name = tests.skipped_param,
                category = "door_lock",
                items = {
                    expected_door_lock_item_info,
                    expected_user_code_item_info,
                    expected_battery_item_info,
                    expected_user_codes_scan_progress_item_info,
                    expected_user_lock_operation_item_info,
                    get_expected_user_code_operation_item_info(),
                    get_expected_lock_operation_item_info(),
                    expected_virtual_week_schedule_item_info,
                    expected_virtual_day_schedule_item_info
                },
                node_id = "9000",
                battery_powered = true
            }
        }

        local check_context = {}
        check_zwave_node_adding_result(expected_devices, check_context)

        fixture:remove_node()

        check_zwave_node_removing_result(check_context)
    end

    -- no notification CC, no schedule
    do
        fixture:create_and_add_zwave_node(
                    device_info,
                    {
                        has_schedule = false,
                        has_power_management_notifications = false
                    })

        local expected_devices = {
            {
                name = tests.skipped_param,
                category = "door_lock",
                items = {
                    expected_door_lock_item_info,
                    expected_user_code_item_info,
                    expected_battery_item_info,
                    expected_user_codes_scan_progress_item_info,
                    expected_virtual_week_schedule_item_info,
                    expected_virtual_day_schedule_item_info
                },
                node_id = "9000",
                battery_powered = true
            }
        }

        local check_context = {}
        check_zwave_node_adding_result(expected_devices, check_context)

        fixture:remove_node()

        check_zwave_node_removing_result(check_context)
    end
end

tests.run_with_params(
    test_impl,
    fixture:get_manufacturer_specifics()
)
