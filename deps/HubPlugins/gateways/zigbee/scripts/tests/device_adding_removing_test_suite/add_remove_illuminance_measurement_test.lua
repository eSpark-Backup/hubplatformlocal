local tests = require( "tests" )
local core = require( "core" )

local cluster_meter = { 
    name = "illuminance_measurement",
    attributes = {
        {
            name = "MeasuredValue",
            formatted_value = 0
        },
        {
            name = "MinMeasuredValue",
            formatted_value = "undefined"
        },
        {
            name = "MaxMeasuredValue",
            formatted_value = "undefined"
        }
    }
}

local endpoint_1 = { 
    id = 1,
    clusters = { cluster_meter }
}

local node = {
    id = 42,
    manufacturer_id = "Ezlo",
    model_id = "Lux Sensor",
    security = 0,
    is_reachable = 1,
    is_battery_powered = true,

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
        name = "lux",
        endpoint_id = 1,
        cluster_name = "illuminance_measurement",
        attribute_name = "MeasuredValue",
        value_type = "illuminance",
        value = { value = 0, scale = "lux" }
    }
}


local expected_devices = {
    {
        name = "Light Level Sensor",
        category = "light_sensor",
        subcategory = "",
        items = expected_items,
        node_id = 42,
        battery_powered = true
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
    model_id = "Lux Sensor"
} )

-- check node removing result
loadfile( test_suite_dir .. "check_zigbee_node_removing_result" )( check_context )
