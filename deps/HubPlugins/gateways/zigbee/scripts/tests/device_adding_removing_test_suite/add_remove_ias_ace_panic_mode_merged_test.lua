local tests = require( "tests" )
local emergency_state = loadfile("HUB:zigbee/scripts/resources/enums/emergency_state")()

local cluster_ias_zone = { 
    name = "ias_zone",
    attributes = {
        {
            name = "ZoneType",
            formatted_value = {zone_type = "keypad"}
        }
    }
}
    
local cluster_ias_ace = { 
    name = "ias_ace",
    data_holder = "client",
    attributes = {},
    commands = {
        {
            name = "Emergency"
        },
        {
            name = "Panic"
        }
    }
}

local endpoint_1 = { 
    id = 1,
    clusters = { cluster_ias_zone, cluster_ias_ace }
}

local node = {
    id = 42,
    manufacturer_id = "Ezlo",
    model_id = "Keypad",
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

local expected_security_items = {
    {
        name = "emergency_state",
        endpoint_id = 1,
        cluster_name = "ias_zone",
        attribute_name = "ZoneStatus",
        value_type = "token",
        enum = {
            emergency_state.idle,
            emergency_state.emergency,
            emergency_state.panic,
            emergency_state.emergency_and_panic },
        value = emergency_state.idle,
    },
    {
        name = "emergency_state",
        endpoint_id = 1,
        cluster_name = "ias_ace",
        attribute_name = "",
        value_type = "token",
        enum = {
            emergency_state.idle,
            emergency_state.emergency,
            emergency_state.panic,
            emergency_state.emergency_and_panic },
        value = emergency_state.idle,
        is_merged = true
    },
    {
        name = "security_threat",
        endpoint_id = 1,
        cluster_name = "ias_zone",
        attribute_name = "ZoneStatus",
        tag = ".security_tag",
        main_item_name = "emergency_state",
    },
    {
        name = "tampering_cover_alarm",
        endpoint_id = 1,
        cluster_name = "ias_zone",
        attribute_name = "ZoneStatus",
        value_type = "token",
        enum = {
            tampering_cover_enum.unknown,
            tampering_cover_enum.no_tampering_cover,
            tampering_cover_enum.tampering_cover },
        value = tampering_cover_enum.unknown
    }
}


local expected_devices = {
    {
        name = "Panic Button",
        category = "scene_controller",
        subcategory = "panic",
        items = expected_security_items,
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
    model_id = "Keypad"
} )

-- check node removing result
loadfile( test_suite_dir .. "check_zigbee_node_removing_result" )( check_context )
