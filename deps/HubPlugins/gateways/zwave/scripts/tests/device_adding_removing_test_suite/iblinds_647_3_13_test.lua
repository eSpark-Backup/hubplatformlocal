local tests = require( "tests" )

local class_ids = require( "HUB:zwave/scripts/resources/class_ids" )
local events_handler = assert( loadfile( "HUB:zwave/scripts/events/events_handling" ) )
local zwave = require( "zwave" )
local core = require( "core" )
local storage = hubplugins.object("storage_helper", "HUB:zwave/scripts/utils")

zwave.request_node_values = tests.create_mock()

local channel_0 = {
    id = 0,
    classes = { 
        { id = class_ids.battery },
        { id = class_ids.switch_binary },
        { id = class_ids.dimmer }
    }
}

local node = {
    id = "9000",
    manufacturer_id = 647,
    product_type = 3,
    product_id = 13,
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
local expected_devices = {
    {
        name = "Roller Shutter",
        category = "window_cov",
        subcategory = "window_cov",
        items = { 
            {
                name = "dimmer",
                cc_id = class_ids.dimmer,
                channel_id = 0
            },
            {
                name = "switch",
                cc_id = class_ids.switch_binary,
                channel_id = 0
            },
            {
                name = "battery",
                cc_id = class_ids.battery,
                channel_id = 0
            }
        },
        node_id = "9000",
        battery_powered = true,
        security = "no",
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
    manufacturer_id = 647,
    product_type = 3,
    product_id = 13,
} )

-- check node removing result
loadfile( test_suite_dir .. "check_zwave_node_removing_result" )( check_context )
