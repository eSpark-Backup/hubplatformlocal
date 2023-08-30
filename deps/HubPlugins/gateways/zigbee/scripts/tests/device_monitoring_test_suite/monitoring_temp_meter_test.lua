local tests = require( "tests" )
local core = require( "core" )

local cluster_meter = {
    name = "temperature_measurement",
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
    model_id = "Temp Sensor",
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

local storage = hubplugins.object( "storage_helper", "HUB:zigbee/scripts/utils" )
local item_id = storage:call(
    "find_item_id",
    {
        item_name = "temp",
        node_id = 42,
        endpoint_id = 1,
        cluster_name = "temperature_measurement",
        attribute_name = "MeasuredValue"
    })

tests.assert_ne(item_id, nil, "Try to find item id")

-- check undefined
core.update_item_value_with_min_max = tests.create_mock()

events_handler( {
    event = "attribute_value_updated",
    node_id = 42,
    endpoint_id = 1,
    cluster_name = "temperature_measurement",
    attribute = {
        name = "MeasuredValue",
        formatted_value = "undefined"
    }
} )

tests.assert_called( core.update_item_value_with_min_max, "check if item values not updated", {} )

-- check 0 degrees
core.update_item_value_with_min_max = tests.create_mock()

events_handler( {
    event = "attribute_value_updated",
    node_id = 42,
    endpoint_id = 1,
    cluster_name = "temperature_measurement",
    attribute = {
        name = "MeasuredValue",
        formatted_value = 0
    }
} )

tests.assert_called(
    core.update_item_value_with_min_max,
    "check if item values updated",
    {
        {
            item_id,
            { value = 0, scale = "celsius" },
            { value = 0, scale = "celsius" },
            { value = 40, scale = "celsius" }
        }
    })

-- check 15 degrees
core.update_item_value_with_min_max = tests.create_mock()

events_handler( {
    event = "attribute_value_updated",
    node_id = 42,
    endpoint_id = 1,
    cluster_name = "temperature_measurement",
    attribute = {
        name = "MeasuredValue",
        formatted_value = 1500
    }
} )

tests.assert_called(
    core.update_item_value_with_min_max,
    "check if item values updated",
    {
        {
            item_id,
            { value = 15, scale = "celsius" },
            { value = 0, scale = "celsius" },
            { value = 40, scale = "celsius" }
        }
    })

-- check with correct borders
core.update_item_value_with_min_max = tests.create_mock()

cluster_meter.attributes[2].formatted_value = -1000
cluster_meter.attributes[3].formatted_value = 3000

events_handler( {
    event = "attribute_value_updated",
    node_id = 42,
    endpoint_id = 1,
    cluster_name = "temperature_measurement",
    attribute = {
        name = "MeasuredValue",
        formatted_value = 2000
    }
} )

tests.assert_called(
    core.update_item_value_with_min_max,
    "check if item values updated",
    {
        {
            item_id,
            { value = 20, scale = "celsius" },
            { value = -10, scale = "celsius" },
            { value = 30, scale = "celsius" }
        }
    })

-- check with undefined borders
core.update_item_value_with_min_max = tests.create_mock()

cluster_meter.attributes[2].formatted_value = "undefined"
cluster_meter.attributes[3].formatted_value = "undefined"

events_handler( {
    event = "attribute_value_updated",
    node_id = 42,
    endpoint_id = 1,
    cluster_name = "temperature_measurement",
    attribute = {
        name = "MeasuredValue",
        formatted_value = 2000
    }
} )

tests.assert_called(
    core.update_item_value_with_min_max,
    "check if item values updated",
    {
        {
            item_id,
            { value = 20, scale = "celsius" },
            { value = 0, scale = "celsius" },
            { value = 40, scale = "celsius" }
        }
    })

-- check with incorrect borders
core.update_item_value_with_min_max = tests.create_mock()

cluster_meter.attributes[2].formatted_value = 0
cluster_meter.attributes[3].formatted_value = 0

events_handler( {
    event = "attribute_value_updated",
    node_id = 42,
    endpoint_id = 1,
    cluster_name = "temperature_measurement",
    attribute = {
        name = "MeasuredValue",
        formatted_value = 2000
    }
} )

tests.assert_called(
    core.update_item_value_with_min_max,
    "check if item values updated",
    {
        {
            item_id,
            { value = 20, scale = "celsius" },
            { value = 0, scale = "celsius" },
            { value = 40, scale = "celsius" }
        }
    })
