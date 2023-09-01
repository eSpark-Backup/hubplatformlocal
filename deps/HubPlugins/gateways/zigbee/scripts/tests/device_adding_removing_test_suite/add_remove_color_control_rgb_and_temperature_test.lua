local tests = require( "tests" )

local cluster_color_control = { 
    name = "color_control",
    attributes = {
        {
            name = "CurrentX",
            formatted_value = 0
        },
        {
            name = "CurrentY",
            formatted_value = 0
        },
        {
            name = "ColorTemperatureMireds",
            formatted_value = 0
        },
        {
            name = "ColorMode",
            formatted_value = "color_xy"
        }
    }
}

local cluster_level_control = { 
    name = "level_control",
    attributes = {
        {
            name = "CurrentLevel",
            formatted_value = 0
        }
    }
}

local endpoint_1 = { 
    id = 1,
    clusters = { cluster_color_control, cluster_level_control }
}

local node = {
    id = 42,
    manufacturer_id = "Ezlo",
    model_id = "RGB Bulb",
    security = 0,
    is_reachable = 1,
    is_battery_powered = false,

    endpoints = { endpoint_1 }
}

local events_handler = assert( loadfile( "HUB:zigbee/scripts/events/zigbee/events_handling" ) )

local zigbee = require( "zigbee" )

-- prepare zigbee
zigbee.add_node( node )

-- add node
events_handler( {
    event = "node_added",
    node_id = 42 
} )

-- check adding result
local expected_items = {
    {
        name = "dimmer",
        endpoint_id = 1,
        cluster_name = "level_control",
        attribute_name = "CurrentLevel",
        value_type = "int",
        value = 0
    },
    {
        name = "dimmer_up",
        endpoint_id = 1,
        cluster_name = "level_control",
        attribute_name = "CurrentLevel",
        value_type = "int",
        value = 0
    },
    {
        name = "dimmer_down",
        endpoint_id = 1,
        cluster_name = "level_control",
        attribute_name = "CurrentLevel",
        value_type = "int",
        value = 0
    },
    {
        name = "dimmer_stop",
        endpoint_id = 1,
        cluster_name = "level_control",
        attribute_name = "CurrentLevel",
        value_type = "int",
        value = 0
    },
    {
        name = "rgbcolor",
        endpoint_id = 1,
        cluster_name = "color_control",
        attribute_name = "",
        value_type = "rgb",
        value = {
            red = 0,
            blue = 0,
            green = 0,
            cwhite = 0,
            wwhite = 0
        }
    }
}


local expected_devices = {
    {
        name = "RGB Light",
        category = "dimmable_light",
        subcategory = "dimmable_colored",
        items = expected_items,
        node_id = 42,
        battery_powered = false
    }
}

local check_context = {
    descriptors = {},
    item_ids = {},
    device_ids = {}
}

local test_suite_dir = "HUB:zigbee/scripts/tests/device_adding_removing_test_suite/"

loadfile( test_suite_dir .. "check_zigbee_node_adding_result" )( expected_devices, check_context )

-- remove node
events_handler( {
    event = "node_removed",
    node_id = 42,     
    manufacturer_id = "Ezlo",
    model_id = "RGB Bulb"
} )

-- check node removing result
loadfile( test_suite_dir .. "check_zigbee_node_removing_result" )( check_context )
