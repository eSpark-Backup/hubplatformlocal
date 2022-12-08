local tests = require( "tests" )

local cluster_meter = { 
    name = "relative_humidity",
    attributes = {
        {
            name = "MeasuredValue",
            formatted_value = 0
        },
        {
            name = "MinMeasuredValue",
            formatted_value = 0
        },
        {
            name = "MaxMeasuredValue",
            formatted_value = 0
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
    model_id = "Humidity Sensor",
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
        name = "humidity",
        endpoint_id = 1,
        cluster_name = "relative_humidity",
        attribute_name = "MeasuredValue",
        value_type = "humidity",
        value = { value = 0, scale = "percent" }
    }
}


local expected_devices = {
    {
        name = "Humidity Sensor",
        category = "humidity",
        subcategory = "",
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
    model_id = "Humidity Sensor"
} )

-- check node removing result
loadfile( test_suite_dir .. "check_zigbee_node_removing_result" )( check_context )
