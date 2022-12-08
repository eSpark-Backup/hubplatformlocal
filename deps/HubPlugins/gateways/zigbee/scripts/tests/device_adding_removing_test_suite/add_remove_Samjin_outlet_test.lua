local tests = require( "tests" )

local cluster_on_off = { 
    name = "on_off",
    attributes = {
        {
            name = "OnOff",
            formatted_value = "false"
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
    clusters = { cluster_on_off, cluster_level_control }
}

local node = {
    id = 42,
    manufacturer_id = "Samjin",
    model_id = "outlet",
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
local expected_switch_items = {
    {
        name = "switch",
        endpoint_id = 1,
        cluster_name = "on_off",
        attribute_name = "OnOff",
        value_type = "bool",
        value = false
    },
    {
        name = "switch",
        endpoint_id = 1,
        cluster_name = "level_control",
        attribute_name = "CurrentLevel",
        value_type = "bool",
        value = false,
        is_merged = true
    }
}

local expected_devices = {
    {
        name = "Switch Outlet IM66001-OTP01",
        category = "switch",
        subcategory = "interior_plugin",
        items = expected_switch_items,
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
    manufacturer_id = "Samjin",
    model_id = "outlet"
} )

-- check node removing result
loadfile( test_suite_dir .. "check_zigbee_node_removing_result" )( check_context )
