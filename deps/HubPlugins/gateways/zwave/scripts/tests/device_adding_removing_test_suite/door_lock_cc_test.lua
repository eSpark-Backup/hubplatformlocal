local zwave = require("zwave")
local events_handler = assert( loadfile( "HUB:zwave/scripts/events/events_handling" ) )
local class_ids = require( "HUB:zwave/scripts/resources/class_ids" )
local item_value_type = require("HUB:zwave/scripts/resources/item_value_type")
local item_names = require("HUB:zwave/scripts/resources/item_names")
local tests = require("tests")
local storage = hubplugins.object("storage_helper", "HUB:zwave/scripts/utils")

zwave.request_node_values = tests.create_mock()

local channel_0 = {
    id = 0,
    classes = { 
        {
            id = class_ids.door_lock,
            version = 2
        }
    }
}

local node = {
    id = "9000",
    manufacturer_id = 0,
    product_type = 1,
    product_id = 2,
    is_battery_powered = true,

    specific_type = "",
    generic_type = "",

    security = { "none" },
    security_class_started = { "none" },

    channels = { channel_0 }
}

zwave.add_node( node )

-- add node
events_handler( {
    event = "node_added",
    node_id = 9000
} )


-- check adding result
local expected_items = {
    {
        name = item_names.door_lock,
        cc_id = class_ids.door_lock,
        channel_id = 0,
        name = item_names.door_lock,
        cc_id = class_ids.door_lock,
        channel_id = 0,
        value_type = "token",
        value = "unknown",
        enum = {
            "unsecured",
            "unknown",
            "secured"
        }
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
        name = "Door Lock",
        category = "door_lock",
        items = expected_items,
        settings = expected_settings,
        node_id = "9000",
        battery_powered = true
    }
}

local check_context = {
    descriptors = {},
    item_ids = {},
    device_ids = {},
}

local test_suite_dir = "HUB:zwave/scripts/tests/device_adding_removing_test_suite/"

loadfile( test_suite_dir .. "check_zwave_node_adding_result" )( expected_devices, check_context )


-- remove node
events_handler( {
    event = "node_removed",
    node_id = "9000",
    manufacturer_id = 0,
    product_type = 1,
    product_id = 2,
} )

-- check removing result
loadfile( test_suite_dir .. "check_zwave_node_removing_result" )( check_context )
