local tests = require( "tests" )

local class_ids = require( "HUB:zwave/scripts/resources/class_ids" )
local item_names = require("HUB:zwave/scripts/resources/item_names")
local item_descriptor_tags = require("HUB:zwave/scripts/resources/item_descriptor_tags")

local node = {
    basic_type = 4,
    channels = {
        {
            classes = {
                {
                    id = 32,
                    is_supported = false,
                    version = 0
                },
                {
                    id = 112,
                    parameters = {},
                    version = 1
                },
                {
                    id = 94,
                    version = 2
                },
                {
                    id = 114,
                    version = 2
                },
                {
                    id = 152,
                    version = 1
                },
                {
                    firmware_versions = {
                        "1.78",
                        "13.0"
                    },
                    hardware_version = 255,
                    id = 134,
                    library_type = "enhanced_slave",
                    version = 2,
                    zwave_protocol_version = "4.24"
                },
                {
                    id = 128,
                    version = 1
                },
                {
                    id = 98,
                    version = 2
                },
                {
                    association_groups = {
                        {
                            group_id = 1,
                            group_nodes = {
                                1
                            },
                            group_nodes_max_count = 1
                        }
                    },
                    association_groups_count = 1,
                    id = 133,
                    version = 2
                },
                {
                    association_groups = {
                        {
                            commands = {
                                {
                                    class_id = 128,
                                    command_id = 3,
                                    name = "BATTERY_REPORT"
                                },
                                {
                                    class_id = 113,
                                    command_id = 5,
                                    name = "NOTIFICATION_REPORT"
                                },
                                {
                                    class_id = 90,
                                    command_id = 1,
                                    name = "DEVICE_RESET_LOCALLY_NOTIFICATION"
                                },
                                {
                                    class_id = 112,
                                    command_id = 6
                                }
                            },
                            group_id = 1,
                            group_name = "Lifeline",
                            profile = {
                                name = "General:Lifeline",
                                profile_lsb = 1,
                                profile_msb = 0
                            }
                        }
                    },
                    association_groups_count = 1,
                    id = 89,
                    version = 1
                },
                {
                    id = 113,
                    notifications = {
                        {
                            event_types = {
                                "manual_lock_operation",
                                "manual_unlock_operation",
                                "rf_lock_operation",
                                "rf_unlock_operation",
                                "keypad_lock_operation",
                                "keypad_unlock_operation",
                                "auto_lock_locked_operation",
                                "lock_jammed",
                                "all_user_codes_deleted",
                                "single_user_code_deleted",
                                "new_user_code_added",
                                "new_user_code_not_added_due_to_duplicated_code",
                                "keypad_temporary_disabled",
                                "new_program_code_entered"
                            },
                            type = "access_control",
                            unknown_event_types = {}
                        },
                        {
                            event_types = {
                                "power_has_been_applied",
                                "replace_battery_soon",
                                "replace_battery_now"
                            },
                            type = "power_management",
                            unknown_event_types = {}
                        }
                    },
                    v1_supported = true,
                    version = 4
                },
                {
                    id = 99,
                    supported_users_count = 244,
                    version = 1
                },
                {
                    id = 138,
                    version = 2
                },
                {
                    id = 139,
                    version = 1
                },
                {
                    daily_repeating_slots = 7,
                    id = 78,
                    version = 3,
                    week_day_slots = 0,
                    year_day_slots = 1
                },
                {
                    firmware_ids = {
                        769,
                        33028
                    },
                    id = 122,
                    version = 3
                }
            },
            id = 0
        }
    },
    command_span = 1000,
    generic_type = "GENERIC_TYPE_ENTRY_CONTROL",
    icon_type = "ICON_TYPE_GENERIC_DOOR_LOCK_KEYPAD",
    id = "9000",
    is_battery_powered = true,
    is_reachable = false,
    is_rediscover_required = false,
    is_smart_include = false,
    manufacturer_id = 297,
    product_id = 1489,
    product_type = 33028,
    security = {
        "s0"
    },
    security_class_started = {
        "s0"
    },
    specific_type = "SPECIFIC_TYPE_SECURE_KEYPAD_DOOR_LOCK"
}

local events_handler = assert( loadfile( "HUB:zwave/scripts/events/events_handling" ) )

local zwave = require( "zwave" )
zwave.request_node_values = tests.create_mock()
-- prepare zwave
zwave.add_node( node )


-- add node
events_handler( {
    event = "node_added",
    node_id = 9000
} )

local expected_devices = {
    {
        name = "Yale Assure 2 Lock",
        category = "door_lock",
        node_id = "9000",
        battery_powered = true,
        items = {
            {
                name = item_names.door_lock,
                cc_id = class_ids.door_lock,
                channel_id = 0,
            },
            {
                name = item_names.user_codes,
                cc_id = class_ids.user_codes,
                channel_id = 0,
            },
            {
                name = item_names.user_codes_scan_progress,
                cc_id = class_ids.user_codes,
                channel_id = 0,
            },
            {
                name = item_names.weekly_user_code_intervals,
                cc_id = class_ids.schedule_entry_lock,
                channel_id = 0,
            },
            {
                name = item_names.daily_user_code_intervals,
                cc_id = class_ids.schedule_entry_lock,
                channel_id = 0,
            },
            {
                name = item_names.user_lock_operation,
                cc_id = class_ids.notification,
                channel_id = 0,
                tag = "access_control"
            },
            {
                name = item_names.lock_operation,
                cc_id = class_ids.notification,
                channel_id = 0,
                tag = "access_control"
            },
            {
                name = item_names.user_code_operation,
                cc_id = class_ids.notification,
                channel_id = 0,
                tag = "access_control"
            },
            {
                name = item_names.keypad_state,
                cc_id = class_ids.notification,
                channel_id = 0,
                tag = "access_control"
            },
            {
                name = item_names.power_state,
                cc_id = class_ids.notification,
                channel_id = 0,
                tag = "power_management"
            },
            {
                name = item_names.battery_maintenance_state,
                cc_id = class_ids.notification,
                channel_id = 0,
                tag = "power_management"
            },
            {
                name = item_names.battery,
                cc_id = class_ids.battery,
                channel_id = 0,
                value_type = "int",
                value = 100
            }
        }
    }
}

local check_context = {
    descriptors = {},
    item_ids = {},
    device_ids = {},
    tests = tests
}

local test_suite_dir = "HUB:zwave/scripts/tests/device_adding_removing_test_suite/"

loadfile( test_suite_dir .. "check_zwave_node_adding_result" )( expected_devices, check_context )

-- remove node
events_handler( {
    event = "node_removed",
    node_id = "9000",
    manufacturer_id = 297,
    product_id = 1489,
    product_type = 33028
} )

-- check removing result
loadfile( test_suite_dir .. "check_zwave_node_removing_result" )( check_context )
