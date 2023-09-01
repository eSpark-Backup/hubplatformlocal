local tests = require( "tests" )

local class_ids = require( "HUB:zwave/scripts/resources/class_ids" )
local item_names = require("HUB:zwave/scripts/resources/item_names")

local cc_switch_binary = {
    id = class_ids.switch_binary,
}

local cc_switch_color = {
    id = class_ids.switch_color,
    component_ids ={},
}

local channel_0 = {
    id = 0,
    classes = { cc_switch_color, cc_switch_binary }
}

local node = {
    id = "100500",
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

local events_handler = assert( loadfile( "HUB:zwave/scripts/events/events_handling" ) )

local zwave = require( "zwave" )
zwave.request_node_values = tests.create_mock()

-- prepare zwave
zwave.add_node( node )


-- add node
events_handler( {
    event = "node_added",
    node_id = 100500
} )


local expected_devices = {
    {
        name = "RGB Light",
        category = "dimmable_light",
        subcategory = "dimmable_colored",
        node_id = "100500",
        battery_powered = true,
        items = {
            {
                name = item_names.switch,
                cc_id = class_ids.switch_binary,
                channel_id = 0
            },
            {
                name = item_names.rgb_color,
                cc_id = class_ids.switch_color,
                channel_id = 0
            },
            {
                name = item_names.rgb_color_up,
                cc_id = class_ids.switch_color,
                channel_id = 0
            },
            {
                name = item_names.rgb_color_down,
                cc_id = class_ids.switch_color,
                channel_id = 0
            },
            {
                name = item_names.rgb_color_stop,
                cc_id = class_ids.switch_color,
                channel_id = 0
            }
        },
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
    node_id = "100500",
    manufacturer_id = 0,
    product_type = 1,
    product_id = 2,
} )

-- check removing result
loadfile( test_suite_dir .. "check_zwave_node_removing_result" )( check_context )
