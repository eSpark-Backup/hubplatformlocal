local tests = require( "tests" )

local cluster_ias_zone = { 
    name = "ias_zone",
    attributes = {
        {
            name = "ZoneType",
            formatted_value = {zone_type = "contact_switch"}
        }
    }
}

local endpoint_1 = { 
    id = 1,
    clusters = { cluster_ias_zone }
}

local node = {
    id = 42,
    manufacturer_id = "Ezlo",
    model_id = "Door Window Sensor",
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

local dw_state_enum = loadfile("HUB:zigbee/scripts/resources/enums/dw_state_event")()
local tampering_cover_enum = loadfile("HUB:zigbee/scripts/resources/enums/tampering_cover_events")()

-- check adding result
local expected_dw_items = {
    {
        name = "dw_state",
        endpoint_id = 1,
        cluster_name = "ias_zone",
        attribute_name = "ZoneStatus",
        value_type = "token",
        enum = {
            dw_state_enum.unknown,
            dw_state_enum.dw_is_closed,
            dw_state_enum.dw_is_opened },
        value = dw_state_enum.unknown
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
    },
    {
        name = "security_threat",
        endpoint_id = 1,
        cluster_name = "ias_zone",
        attribute_name = "ZoneStatus",
        tag = ".security_tag",
        main_item_name = "dw_state",
    }
}


local expected_devices = {
    {
        name = "Door/Window Sensor",
        category = "security_sensor",
        subcategory = "door",
        items = expected_dw_items,
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
    model_id = "Door Window Sensor"
} )

-- check node removing result
loadfile( test_suite_dir .. "check_zigbee_node_removing_result" )( check_context )
