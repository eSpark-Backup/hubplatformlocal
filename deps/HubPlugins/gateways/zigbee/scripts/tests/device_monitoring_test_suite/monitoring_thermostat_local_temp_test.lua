local tests = require( "tests" )
local core = require( "core" )

local cluster_meter = {
    name = "thermostat",
    attributes = {
        {
            name = "SystemMode",
            formatted_value = "off"
        },
        {
            name = "LocalTemperature",
            formatted_value = 0
        },
        {
            name = "OccupiedCoolingSetpoint",
            formatted_value = 0
        },
        {
            name = "OccupiedHeatingSetpoint",
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
    model_id = "Thermostat",
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
        cluster_name = "thermostat",
        attribute_name = "LocalTemperature"
    })

tests.assert_ne(item_id, nil, "Try to find item id")

-- check undefined
core.update_item_value = tests.create_mock()

events_handler( {
    event = "attribute_value_updated",
    node_id = 42,
    endpoint_id = 1,
    cluster_name = "thermostat",
    attribute = {
        name = "LocalTemperature",
        formatted_value = "undefined"
    }
} )

tests.assert_called( core.update_item_value, "check if item values not updated", {} )

-- check 0 degrees
core.update_item_value = tests.create_mock()

events_handler( {
    event = "attribute_value_updated",
    node_id = 42,
    endpoint_id = 1,
    cluster_name = "thermostat",
    attribute = {
        name = "LocalTemperature",
        formatted_value = 0
    }
} )

tests.assert_called( core.update_item_value, "check if item values updated", { { item_id, { value = 0, scale = "celsius" } } } )

-- check 15 degrees
core.update_item_value = tests.create_mock()

events_handler( {
    event = "attribute_value_updated",
    node_id = 42,
    endpoint_id = 1,
    cluster_name = "thermostat",
    attribute = {
        name = "LocalTemperature",
        formatted_value = 1500
    }
} )

tests.assert_called( core.update_item_value, "check if item values updated", { { item_id, { value = 15, scale = "celsius" } } } )

-- check -5 degrees
core.update_item_value = tests.create_mock()

events_handler( {
    event = "attribute_value_updated",
    node_id = 42,
    endpoint_id = 1,
    cluster_name = "thermostat",
    attribute = {
        name = "LocalTemperature",
        formatted_value = -500
    }
} )

tests.assert_called( core.update_item_value, "check if item values updated", { { item_id, { value = -5, scale = "celsius" } } } )
