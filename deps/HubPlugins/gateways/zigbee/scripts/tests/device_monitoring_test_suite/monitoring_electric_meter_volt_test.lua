local tests = require( "tests" )
local core = require( "core" )

local cluster_on_off = {
    name = "on_off",
    attributes = {
        {
            name = "OnOff",
            formatted_value = "false"
        }
    }
}

local cluster_meter = {
    name = "electrical_measurement",
    attributes = {
        {
            name = "RMSVoltage",
            formatted_value = 0
        },
        {
            name = "ACVoltageDivisor",
            formatted_value = 1
        },
        {
            name = "ACVoltageMultiplier",
            formatted_value = 1
        }
    }
}

local endpoint_1 = {
    id = 1,
    clusters = { cluster_on_off, cluster_meter }
}

local node = {
    id = 42,
    manufacturer_id = "Ezlo",
    model_id = "Plug",
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
        item_name = "electric_meter_volt",
        node_id = 42,
        endpoint_id = 1,
        cluster_name = "electrical_measurement",
        attribute_name = "RMSVoltage"
    })

tests.assert_ne(item_id, nil, "Try to find item id")

-- check undefined
core.update_item_value = tests.create_mock()

events_handler( {
    event = "attribute_value_updated",
    node_id = 42,
    endpoint_id = 1,
    cluster_name = "electrical_measurement",
    attribute = {
        name = "RMSVoltage",
        formatted_value = "undefined"
    }
} )

tests.assert_called( core.update_item_value, "check if item values not updated", {} )

-- check simple
core.update_item_value = tests.create_mock()

events_handler( {
    event = "attribute_value_updated",
    node_id = 42,
    endpoint_id = 1,
    cluster_name = "electrical_measurement",
    attribute = {
        name = "RMSVoltage",
        formatted_value = 500
    }
} )

tests.assert_called( core.update_item_value, "check if item values updated", { { item_id, { value = 500.0, scale = "volt" } } } )

-- check with multiply
cluster_meter.attributes[3].formatted_value = 2
core.update_item_value = tests.create_mock()

events_handler( {
    event = "attribute_value_updated",
    node_id = 42,
    endpoint_id = 1,
    cluster_name = "electrical_measurement",
    attribute = {
        name = "RMSVoltage",
        formatted_value = 500
    }
} )

tests.assert_called( core.update_item_value, "check if item values updated", { { item_id, { value = 1000.0, scale = "volt" } } } )

-- check with divide
cluster_meter.attributes[3].formatted_value = 1
cluster_meter.attributes[2].formatted_value = 2
core.update_item_value = tests.create_mock()

events_handler( {
    event = "attribute_value_updated",
    node_id = 42,
    endpoint_id = 1,
    cluster_name = "electrical_measurement",
    attribute = {
        name = "RMSVoltage",
        formatted_value = 500
    }
} )

tests.assert_called( core.update_item_value, "check if item values updated", { { item_id, { value = 250.0, scale = "volt" } } } )

-- check with divide
cluster_meter.attributes[3].formatted_value = 4
cluster_meter.attributes[2].formatted_value = 2
core.update_item_value = tests.create_mock()

events_handler( {
    event = "attribute_value_updated",
    node_id = 42,
    endpoint_id = 1,
    cluster_name = "electrical_measurement",
    attribute = {
        name = "RMSVoltage",
        formatted_value = 500
    }
} )

tests.assert_called( core.update_item_value, "check if item values updated", { { item_id, { value = 1000.0, scale = "volt" } } } )
