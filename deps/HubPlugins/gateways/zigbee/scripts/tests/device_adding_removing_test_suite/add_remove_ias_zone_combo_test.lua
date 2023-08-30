local tests = require( "tests" )

local function cluster_ias_zone_generator(zone_type)
    return { 
        name = "ias_zone",
        attributes = {
            {
                name = "ZoneType",
                formatted_value = {zone_type = zone_type}
            }
        }
    }
end

local endpoint_1 = { 
    id = 1,
    clusters = { cluster_ias_zone_generator("motion_sensor") }
}

local endpoint_2 = { 
    id = 2,
    clusters = { cluster_ias_zone_generator("fire_sensor") }
}

local endpoint_3 = { 
    id = 3,
    clusters = { cluster_ias_zone_generator("gas_sensor") }
}

local node = {
    id = 42,
    manufacturer_id = "Ezlo",
    model_id = "Combo Sensor",
    security = 0,
    is_reachable = 1,
    is_battery_powered = false,

    endpoints = { endpoint_1, endpoint_2, endpoint_3 }
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

local expected_motion_items = {
    {
        name = "motion",
        endpoint_id = 1,
        cluster_name = "ias_zone",
        attribute_name = "ZoneStatus",
        value_type = "bool",
        value = false
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
        main_item_name = "motion",
    }
}

local smoke_alarm_enum = loadfile("HUB:zigbee/scripts/resources/enums/smoke_alarm_event")()

local expected_smoke_items = {
    {
        name = "smoke_alarm",
        endpoint_id = 2,
        cluster_name = "ias_zone",
        attribute_name = "ZoneStatus",
        tag = "alarm_1",
        value_type = "token",
        enum = {
            smoke_alarm_enum.unknown,
            smoke_alarm_enum.no_smoke,
            smoke_alarm_enum.smoke_detected },
        value = smoke_alarm_enum.unknown
    },
    {
        name = "tampering_cover_alarm",
        endpoint_id = 2,
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
        endpoint_id = 2,
        cluster_name = "ias_zone",
        attribute_name = "ZoneStatus",
        tag = "alarm_1.security_tag",
        main_item_name = "smoke_alarm",
    }
}

local co_alarm_enum = loadfile("HUB:zigbee/scripts/resources/enums/co_alarm_event")()
local expected_co_items = {
    {
        name = "co_alarm",
        endpoint_id = 3,
        cluster_name = "ias_zone",
        attribute_name = "ZoneStatus",
        tag = "alarm_1",
        value_type = "token",
        enum = {
            co_alarm_enum.unknown,
            co_alarm_enum.no_co,
            co_alarm_enum.co_detected },
        value = co_alarm_enum.unknown
    },
    {
        name = "tampering_cover_alarm",
        endpoint_id = 3,
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
        endpoint_id = 3,
        cluster_name = "ias_zone",
        attribute_name = "ZoneStatus",
        tag = "alarm_1.security_tag",
        main_item_name = "co_alarm",
    }
}

local expected_devices = {
    {
        name = "Motion Sensor",
        category = "security_sensor",
        subcategory = "motion",
        items = expected_motion_items,
        node_id = 42,
        battery_powered = false,
        
        subdevices = {
            {
                name = "Smoke Sensor",
                category = "security_sensor",
                subcategory = "smoke",
                items = expected_smoke_items
            },
            {
                name = "CO Sensor",
                category = "security_sensor",
                subcategory = "co",
                items = expected_co_items
            }
        }
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
    model_id = "Combo Sensor"
} )

-- check node removing result
loadfile( test_suite_dir .. "check_zigbee_node_removing_result" )( check_context )
