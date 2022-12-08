
local class_ids = require( "HUB:zwave/scripts/resources/class_ids" )
local events_handler = assert( loadfile( "HUB:zwave/scripts/events/events_handling" ) )
local zwave = require( "zwave" )
local tests = require("tests")

zwave.request_node_values = tests.create_mock()


local channel_0 = {
    id = 0,
    classes = {
        { id = class_ids.user_codes, supported_users_count = 250 }
    }
}

local node = {
    id = "9000",
    manufacturer_id = 297,
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
        channel_id = 0
    },
    {
        name = "user_codes_scan_progress",
        cc_id = class_ids.user_codes,
        channel_id = 0,
        value = 0
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

local expected_devices = {
    {
        name = "Touchscreen Deadbolt",
        category = "door_lock",
        items = expected_door_lock_items,
        node_id = "9000",
        battery_powered = true,
    }
}

local test_suite_dir = "HUB:zwave/scripts/tests/device_adding_removing_test_suite/"

local check_zwave_node_adding_result = loadfile( test_suite_dir .. "check_zwave_node_adding_result" )

check_zwave_node_adding_result( expected_devices )

local request_event = {
    event = "user_code_updated",
    node_id = 9000,
    class_id = class_ids.user_codes,
    channel_id = 0,

    user_code_ranges = {
        {
            user_status = "available"
        }
    },
    user_polling_status = "polling",
    supported_users_count = 250
}

-- check 0%
request_event.user_code_ranges[1].user_id = 1
events_handler(request_event)
expected_devices[1].items[2].value = 0
check_zwave_node_adding_result( expected_devices )

-- check 2%
request_event.user_code_ranges[1].user_id = 4
events_handler(request_event)
expected_devices[1].items[2].value = 2
check_zwave_node_adding_result( expected_devices )

-- check 50%
request_event.user_code_ranges[1].user_id = 125
events_handler(request_event)
expected_devices[1].items[2].value = 50
check_zwave_node_adding_result( expected_devices )

-- check 99%
request_event.user_code_ranges[1].user_id = 248
events_handler(request_event)
expected_devices[1].items[2].value = 99
check_zwave_node_adding_result( expected_devices )

-- check 100%
request_event.user_code_ranges[1].user_id = 250
events_handler(request_event)
expected_devices[1].items[2].value = 100
check_zwave_node_adding_result( expected_devices )

