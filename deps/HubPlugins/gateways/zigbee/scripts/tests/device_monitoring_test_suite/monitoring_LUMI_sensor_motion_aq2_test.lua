local tests = require( "tests" )
local core = require( "core" )

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

local storage = hubplugins.object( "storage_helper", "HUB:zigbee/scripts/utils" )
local item_id = storage:call(
    "find_item_id",
    {
        item_name = "motion",
        node_id = 42,
        endpoint_id = 1,
        cluster_name = "occupancy_sensing",
        attribute_name = "Occupancy",
    })

tests.assert_ne(item_id, nil, "Try to find item id")

local security_item_id = storage:call(
    "find_item_id",
    {
        item_name = "motion",
        node_id = 42,
        endpoint_id = 1,
        cluster_name = "occupancy_sensing",
        attribute_name = "Occupancy",
        tag = ".security_tag"
    })

tests.assert_ne(security_item_id, nil, "Try to find seciruty item id")

local lumi_item_id = storage:call(
    "find_item_id",
    {
        item_name = "lux",
        node_id = 42,
        endpoint_id = 1,
        cluster_name = "illuminance_measurement",
        attribute_name = "MeasuredValue",
    })

tests.assert_ne(lumi_item_id, nil, "Try to find item id")

-- check motion detect
core.update_item_value = tests.create_mock()

events_handler( {
    event = "unknown_attribute_value_updated",
    node_id = 42,
    endpoint_id = 1,
    cluster_name = "occupancy_sensing",
    attribute_name = "Occupancy",
    value = "1"
} )

tests.assert_called(
    core.update_item_value,
    "check if item values updated",
    {
        { item_id, true },
        { security_item_id, true },
        { item_id, false },
        { security_item_id, false },
    }
)

-- check unsupported motion value
core.update_item_value = tests.create_mock()

events_handler( {
    event = "unknown_attribute_value_updated",
    node_id = 42,
    endpoint_id = 1,
    cluster_name = "occupancy_sensing",
    attribute_name = "Occupancy",
    value = "0"
} )

tests.assert_called(
    core.update_item_value,
    "check if item values not updated",
    {}
)

-- check lumi 1
core.update_item_value_with_min_max = tests.create_mock()

events_handler( {
    event = "attribute_value_updated",
    node_id = 42,
    endpoint_id = 1,
    cluster_name = "illuminance_measurement",
    attribute = {
        name = "MeasuredValue",
        formatted_value = 1
    }
} )

tests.assert_called(
    core.update_item_value_with_min_max,
    "check if item values updated",
    {
        {
            lumi_item_id,
            { value = 1, scale = "lux" },
            { value = 1, scale = "lux" },
            { value = 3576000, scale = "lux" }
        }
    }
)

-- check lumi 200
core.update_item_value_with_min_max = tests.create_mock()

events_handler( {
    event = "attribute_value_updated",
    node_id = 42,
    endpoint_id = 1,
    cluster_name = "illuminance_measurement",
    attribute = {
        name = "MeasuredValue",
        formatted_value = 200
    }
} )

tests.assert_called(
    core.update_item_value_with_min_max,
    "check if item values updated",
    {
        {
            lumi_item_id,
            { value = 200, scale = "lux" },
            { value = 1, scale = "lux" },
            { value = 3576000, scale = "lux" }
        }
    }
)
