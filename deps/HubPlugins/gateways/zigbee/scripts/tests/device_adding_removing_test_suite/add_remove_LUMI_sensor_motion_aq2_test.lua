local tests = require( "tests" )

local cluster_occupancy_sensing = { 
    name = "occupancy_sensing",
    attributes = {}
}

local cluster_ias_zone = { 
    name = "ias_zone",
    attributes = {}
}

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

local cluster_power_configuration = {
    name = "power_configuration",
    attributes = {
        {
            name = "BatteryVoltage",
        }
    }
}

local endpoint_1 = { 
    id = 1,
    clusters = { 
        cluster_occupancy_sensing, 
        cluster_ias_zone, 
        cluster_power_configuration,
        cluster_meter
    }
}

local node = {
    id = 42,
    manufacturer_id = "LUMI",
    model_id = "lumi.sensor_motion.aq2",
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
        name = "motion",
        endpoint_id = 1,
        cluster_name = "occupancy_sensing",
        attribute_name = "Occupancy",
        value_type = "bool",
        value = false
    },
    {
        name = "security_threat",
        endpoint_id = 1,
        cluster_name = "occupancy_sensing",
        attribute_name = "Occupancy",
        tag = ".security_tag",
        main_item_name = "motion",
    },
    {
        name = "battery",
        endpoint_id = 1,
        cluster_name = "power_configuration",
        attribute_name = "BatteryVoltage",
        value_type = "int",
        value = 100
    }
}

local expected_lumi_item = {
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
        name = "Motion Sensor",
        category = "security_sensor",
        subcategory = "motion",
        items = expected_items,
        node_id = 42,
        battery_powered = true,
        
        subdevices = {
            {
                name = "Light Level Sensor",
                category = "light_sensor",
                subcategory = "",
                items = expected_lumi_item,
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
    manufacturer_id = "LUMI",
    model_id = "lumi.sensor_motion.aq2"
} )

-- check node removing result
loadfile( test_suite_dir .. "check_zigbee_node_removing_result" )( check_context )
