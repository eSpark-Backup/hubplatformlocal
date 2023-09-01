local tests = require( "tests" )

local cluster_door_lock = { 
    name = "door_lock",
    attributes = {}
}

local endpoint_1 = { 
    id = 1,
    clusters = { 
        cluster_door_lock
    }
}

local node = {
    id = 42,
    manufacturer_id = "LUMI",
    model_id = "lumi.vibration.aq1",
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
local tampering_cover_enum = loadfile("HUB:zigbee/scripts/resources/enums/tampering_cover_events")()

local expected_items = {
    {
        name = "activity",
        endpoint_id = 1,
        cluster_name = "door_lock",
        attribute_name = "",
        value_type = "token",
        enum = {
            "no_activity",
            "shake",
            "tilt",
            "drop"
        },
        value = "no_activity"
    },
    {
        name = "bed_activity",
        endpoint_id = 1,
        cluster_name = "door_lock",
        attribute_name = "",
        value_type = "int",
        value = 0
    }
}


local expected_devices = {
    {
        name = "Activity Sensor",
        category = "state_sensor",
        subcategory = "activity",
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
    manufacturer_id = "LUMI",
    model_id = "lumi.vibration.aq1"
} )

-- check node removing result
loadfile( test_suite_dir .. "check_zigbee_node_removing_result" )( check_context )
