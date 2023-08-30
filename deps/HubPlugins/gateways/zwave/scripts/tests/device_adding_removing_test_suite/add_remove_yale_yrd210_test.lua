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
                    version = 1
                },
                {
                    id = 112,
                    parameters = {},
                    version = 1
                },
                {
                    id = 114,
                    version = 1
                },
                {
                    firmware_versions = {
                        "83.32"
                    },
                    hardware_version = 0,
                    id = 134,
                    library_type = "enhanced_slave",
                    version = 1,
                    zwave_protocol_version = "3.34"
                },
                {
                    id = 152,
                    version = 1
                },
                {
                    id = 98,
                    version = 1
                },
                {
                    daily_repeating_slots = 1,
                    id = 78,
                    version = 3,
                    week_day_slots = 0,
                    year_day_slots = 0
                },
                {
                    id = 99,
                    supported_users_count = 250,
                    version = 1
                },
                {
                    id = 139,
                    version = 1
                },
                {
                    association_groups = {
                        {
                            group_id = 1,
                            group_nodes = {
                                1
                            }
                        }
                    },
                    association_groups_count = 1,
                    id = 133,
                    version = 1
                },
                {
                    id = 113,
                    notifications = {},
                    v1_supported = true,
                    version = 1
                },
                {
                    id = 128,
                    version = 1
                },
                {
                    id = 138,
                    version = 2
                }
            },
            id = 0
        }
    },
    generic_type = "GENERIC_TYPE_ENTRY_CONTROL",
    id = "9000",
    is_battery_powered = true,
    is_reachable = false,
    is_rediscover_required = false,
    is_smart_include = false,
    manufacturer_id = 297,
    product_id = 521,
    product_type = 4,
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
        name = "Door Lock",
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
                cc_id = class_ids.user_codes,
                channel_id = 0,
            },
            {
                name = item_names.daily_user_code_intervals,
                cc_id = class_ids.user_codes,
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
    product_id = 521,
    product_type = 4,
} )

-- check removing result
loadfile( test_suite_dir .. "check_zwave_node_removing_result" )( check_context )
