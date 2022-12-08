
local class_ids = require( "HUB:zwave/scripts/resources/class_ids" )
local events_handler = assert( loadfile( "HUB:zwave/scripts/events/events_handling" ) )
local zwave = require( "zwave" )
local tests = require("tests")

zwave.request_node_values = tests.create_mock()

local function test_impl( test_param )

    local channel_0 = {
        id = 0,
        classes = {
            { id = 115 },
            -- { id = class_ids.configuration },
            { id = class_ids.battery },
            { id = class_ids.door_lock },
            {
                id = class_ids.notification,
                notifications = {
                    {
                        type = "access_control",
                        event_types = { "manual_lock_operation",
                                       "manual_unlock_operation",
                                       "rf_lock_operation",
                                       "rf_unlock_operation",
                                       "keypad_lock_operation",
                                       "keypad_unlock_operation",
                                       "valid_user_but_outside_of_schedule_operation",
                                       "auto_lock_locked_operation",
                                       "lock_jammed",
                                       "all_user_codes_deleted",
                                       "single_user_code_deleted",
                                       "new_user_code_added",
                                       "new_user_code_not_added_due_to_duplicated_code",
                                       "keypad_temporary_disabled",
                                       "new_program_code_entered" }
                    },
                    {
                        type = "power_management",
                        event_types = { "power_has_been_applied",
                                       "replace_battery_soon",
                                       "replace_battery_now"
                        }
                    }
                }
            },
            { id = class_ids.user_codes, supported_users_count = 250 },
            { id = 122 },
        }
    }

    local node = {
        id = "9000",
        manufacturer_id = test_param.manufacturer_id,
        product_type = 32770,
        product_id = 1536,
        is_battery_powered = true,

        specific_type = "",
        generic_type = "",

        security = { "s0" },
        security_class_started = { "s0" },

        channels = { channel_0 }
    }

    -- prepare zwave
    zwave.add_node( node )

    -- add node
    events_handler( {
        event = "node_added",
        node_id = 9000
    } )

    -- check adding result
    local expected_door_lock_items = {
        {
            name = "user_codes",
            cc_id = class_ids.user_codes,
            channel_id = 0,
        },
        {
            name = "user_codes_scan_progress",
            cc_id = class_ids.user_codes,
            channel_id = 0,
        },
        {
            name = "battery",
            cc_id = class_ids.battery,
            channel_id = 0,
        },
        {
            name = "keypad_state",
            tag = "access_control",
            cc_id = class_ids.notification,
            channel_id = 0,
            is_merged = false,
            value_type = "token",
            enum = {
                "unknown",
                "keypad_idle",
                "keypad_temporary_disabled",
            }
        },
        {
            name = "power_state",
            tag = "power_management",
            cc_id = class_ids.notification,
            channel_id = 0,
            is_merged = false,
            value_type = "token",
            enum = {
                "unknown",
                "no_power_applied",
                "power_applied",
            }
        },
        {
            name = "door_lock",
            cc_id = class_ids.door_lock,
            channel_id = 0,
            is_merged = false,
            value_type = "token",
            enum = {
                "unsecured",
                "unknown",
                "secured",
            }
        },
        {
            name = "battery_maintenance_state",
            tag = "power_management",
            cc_id = class_ids.notification,
            channel_id = 0,
            is_merged = false,
            value_type = "token",
            enum = {
                "unknown",
                "battery_maintenance_ok",
                "replace_battery_soon",
                "replace_battery_now",
            }
        },
        {
            name = "lock_operation",
            tag = "access_control",
            cc_id = class_ids.notification,
            channel_id = 0,
            is_merged = false,
            value_type = "token",
            enum = {
                "unknown",
                "manual_lock_operation",
                "manual_unlock_operation",
                "rf_lock_operation",
                "rf_unlock_operation",
                "keypad_lock_operation",
                "keypad_unlock_operation",
                "valid_user_but_outside_of_schedule_operation",
                "auto_lock_locked_operation",
                "lock_jammed",
            }
        },
        {
            name = "user_code_operation",
            tag = "access_control",
            cc_id = class_ids.notification,
            channel_id = 0,
            is_merged = false,
            value_type = "token",
            enum = {
                "no_operation",
                "all_user_codes_deleted",
                "single_user_code_deleted",
                "new_user_code_added",
                "new_user_code_not_added_due_to_duplicated_code",
                "new_program_code_entered",
            }
        },
        {
            name = "user_lock_operation",
            tag = "access_control",
            cc_id = class_ids.notification,
            channel_id = 0,
        },
        {
            name = "daily_user_code_intervals",
            cc_id = class_ids.user_codes,
            channel_id = 0,
            value_type = "dictionary.array.daily_interval",
            value = {},
            elements_max_number_2 = 30,
            force_checking_fields = { "elements_max_number" }
        },
        {
            name = "weekly_user_code_intervals",
            cc_id = class_ids.user_codes,
            channel_id = 0,
            value_type = "dictionary.array.weekly_interval",
            value = {},
            elements_max_number_2 = 30,
            force_checking_fields = { "elements_max_number" }
        }
    }

    local expected_settings = {
        {
            value_type = "bool",
            value = false,
            cc_id = class_ids.door_lock,
            tag = "timed_operations_enabled"
        },
        {
            value_type = "int",
            value = 0,
            cc_id = class_ids.door_lock,
            tag = "lock_timeout"
        },
        {
            value_type = "array.token",
            value = { },
            enum = {
                ["1"] = { text = "Handle 1" },
                ["2"] = { text = "Handle 2" },
                ["3"] = { text = "Handle 3" },
                ["4"] = { text = "Handle 4" }
            },
            cc_id = class_ids.door_lock,
            tag = "enabled_inside_handle_ids"
        },
        {
            value_type = "array.token",
            value = { },
            enum = {
                ["1"] = { text = "Handle 1" },
                ["2"] = { text = "Handle 2" },
                ["3"] = { text = "Handle 3" },
                ["4"] = { text = "Handle 4" }
            },
            cc_id = class_ids.door_lock,
            tag = "enabled_outside_handle_ids"
        },
        {
            value_type = "action",
            value = {
                text = "Rediscover",
                lang_tag = "rediscover_tag"
            },
            tag = "rediscover"
        }
    }

    local expected_devices = {
        {
            name = test_param.device_name,
            category = "door_lock",
            items = expected_door_lock_items,
            settings = expected_settings,
            node_id = "9000",
            battery_powered = true,
        }
    }

    local check_context = {
        descriptors = {},
        item_ids = {},
        device_ids = {}
    }

    local test_suite_dir = "HUB:zwave/scripts/tests/device_adding_removing_test_suite/"

    loadfile( test_suite_dir .. "check_zwave_node_adding_result" )( expected_devices, check_context )

    -- remove node
    events_handler( {
        event = "node_removed",
        node_id = "9000",
        manufacturer_id = test_param.manufacturer_id,
        product_type = 32770,
        product_id = 1536,
    } )

    -- check node removing result
    loadfile( test_suite_dir .. "check_zwave_node_removing_result" )( check_context )
end

tests.run_with_params( test_impl,
{
    {
        value = {
            manufacturer_id = 297,
            device_name = "Touchscreen Deadbolt"
        },
        description = "YRD226"
    },
    {
        value = {
            manufacturer_id = 265,
            device_name = "Door Lock"
        },
        description = "YRD226/YRL226 device with doorlock type auto detection"
    }
} )

