local tests = require( "tests" )

local class_ids = require( "HUB:zwave/scripts/resources/class_ids" )
local events_handler = hubplugins.weak_loadfile( "HUB:zwave/scripts/events/events_handling" )
local zwave = require( "zwave" )
local core = require( "core" )
local storage = hubplugins.object("storage_helper", "HUB:zwave/scripts/utils")

zwave.request_node_values = tests.create_mock()

local cc_basic = { 
    id = class_ids.basic,
    is_supported = false
}

local channel_0 = { 
    id = 0,
    classes = { cc_basic }
}

local node = {
    id = "9000",
    manufacturer_id = 1,
    product_type = 2,
    product_id = 3,
    is_battery_powered = true,

    specific_type = "SPECIFIC_TYPE_NOT_USED",
    generic_type = "GENERIC_TYPE_SWITCH_BINARY",

    security = { "none" },
    security_class_started = { "none" },

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
local expected_devices = {
    {
        name = "In Wall Switch",
        category = "switch",
        subcategory = "in_wall",
        items = {},
        node_id = "9000",
        battery_powered = true
    }
}

local check_context = {
    descriptors = {},
    item_ids = {},
    device_ids = {}
}

local test_suite_dir = "HUB:zwave/scripts/tests/device_adding_removing_test_suite/"

hubplugins.weak_loadfile( test_suite_dir .. "check_zwave_node_adding_result" )( expected_devices, check_context )

-- remove node
events_handler( {
    event = "node_removed",
    node_id = "9000",     
    manufacturer_id = 1,
    product_type = 2,
    product_id = 3
} )

-- check node removing result
hubplugins.weak_loadfile( test_suite_dir .. "check_zwave_node_removing_result" )( check_context )
