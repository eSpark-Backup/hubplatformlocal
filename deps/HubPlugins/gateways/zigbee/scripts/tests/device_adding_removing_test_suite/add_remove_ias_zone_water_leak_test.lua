local tests = require( "tests" )

local cluster_ias_zone = { 
    name = "ias_zone",
    attributes = {
        {
            name = "ZoneType",
            formatted_value = {zone_type = "water_sensor"}
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
    model_id = "Water Leak Sensor",
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
local water_leak_alarm_enum = loadfile("HUB:zigbee/scripts/resources/enums/water_leak_alarm_event")()
local tampering_cover_enum = loadfile("HUB:zigbee/scripts/resources/enums/tampering_cover_events")()

local expected_wl_items = {
    {
        name = "water_leak_alarm",
        endpoint_id = 1,
        cluster_name = "ias_zone",
        attribute_name = "ZoneStatus",
        tag = "alarm_1",
        value_type = "token",
        enum = {
            water_leak_alarm_enum.unknown,
            water_leak_alarm_enum.no_water_leak,
            water_leak_alarm_enum.water_leak_detected },
        value = water_leak_alarm_enum.unknown
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
        tag = "alarm_1.security_tag",
        main_item_name = "water_leak_alarm",
    }
}


local expected_devices = {
    {
        name = "Water Leak Sensor",
        category = "security_sensor",
        subcategory = "leak",
        items = expected_wl_items,
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
    model_id = "Water Leak Sensor"
} )

-- check node removing result
loadfile( test_suite_dir .. "check_zigbee_node_removing_result" )( check_context )
