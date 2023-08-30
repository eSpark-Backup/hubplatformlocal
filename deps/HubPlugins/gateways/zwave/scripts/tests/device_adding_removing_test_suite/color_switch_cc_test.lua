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
            id = class_ids.switch_color,
            version = 2,
            component_ids = { "red", "green", "blue", "wwhite" }
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
        name = item_names.rgb_color,
        cc_id = class_ids.switch_color,
        channel_id = 0,
        value_type = "rgb",
        value = {
            red = 0,
            green = 0,
            blue = 0,
            wwhite = 0
        }
    },
    {
        name = item_names.rgb_color_up,
        cc_id = class_ids.switch_color,
        channel_id = 0,
        value_type = "token",
        value = "",
        enum = { "red", "green", "blue", "wwhite" }
    },
    {
        name = item_names.rgb_color_down,
        cc_id = class_ids.switch_color,
        channel_id = 0,
        value_type = "token",
        value = "",
        enum = { "red", "green", "blue", "wwhite" }
    },
    {
        name = item_names.rgb_color_stop,
        cc_id = class_ids.switch_color,
        channel_id = 0,
        value_type = "token",
        value = "all",
        enum = { "all", "red", "green", "blue", "wwhite" }
    }
}

local expected_settings = {
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
        name = "Unknown",
        category = "unknown",
        items = expected_items,
        settings = expected_settings,
        node_id = "9000",
        battery_powered = true
    }
}

local check_context = { }

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
