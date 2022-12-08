local zwave = require("zwave")
local events_handler = assert( loadfile( "HUB:zwave/scripts/events/events_handling" ) )
local class_ids = require( "HUB:zwave/scripts/resources/class_ids" )
local item_descriptor_tags = require("HUB:zwave/scripts/resources/item_descriptor_tags")
local tests = require("tests")

zwave.request_node_values = tests.create_mock()

-- TODO: add here full list of cc for fibaro 271_796_4097
local channel_0 = {
    id = 0,
    classes = { 
        { id = class_ids.dimmer },
        { 
            id = class_ids.multilevel_sensor,
            sensors = {{
                sensor_type = "power"
            }}
        }
    }
}

local node = {
    id = "9000",
    manufacturer_id = 271,
    product_type = 769,
    product_id = 4097,
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
                name = "dimmer_up",
                cc_id = class_ids.dimmer,
                channel_id = 0
            },
            {
                name = "dimmer_down",
                cc_id = class_ids.dimmer,
                channel_id = 0
            },
            {
                name = "dimmer_stop",
                cc_id = class_ids.dimmer,
                channel_id = 0
            },
            {
                name = "goto_favorite",
                cc_id = class_ids.dimmer,
                tag = "dimmer",
                channel_id = 0  
            },
            {
                name = "electric_meter_watt",
                cc_id = class_ids.multilevel_sensor,
                channel_id = 0
            }
        },
        node_id = "9000",
        battery_powered = true,
        security = "no",
    }
}

local test_suite_dir = "HUB:zwave/scripts/tests/device_adding_removing_test_suite/"
loadfile( test_suite_dir .. "check_zwave_node_adding_result" )( expected_devices )
