local tests = require( "tests" )
local core = require( "core" )

local cluster_thermostat = {
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
    clusters = { cluster_thermostat }
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
        item_name = "thermostat_mode",
        node_id = 42,
        endpoint_id = 1,
        cluster_name = "thermostat",
        attribute_name = "SystemMode"
    })

tests.assert_ne(item_id, nil, "Try to find item id")

-- check off
core.update_item_value = tests.create_mock()

events_handler( {
    event = "attribute_value_updated",
    node_id = 42,
    endpoint_id = 1,
    cluster_name = "thermostat",
    attribute = {
        name = "SystemMode",
        formatted_value = "off"
    }
} )

tests.assert_called( core.update_item_value, "check if item values updated", {{ item_id, "off" }} )

-- check heating
core.update_item_value = tests.create_mock()
zigbee.request_attribute_value = tests.create_mock()

events_handler( {
    event = "attribute_value_updated",
    node_id = 42,
    endpoint_id = 1,
    cluster_name = "thermostat",
    attribute = {
        name = "SystemMode",
        formatted_value = "heat"
    }
} )

tests.assert_called( core.update_item_value, "check if item values updated", {{ item_id, "heat" }} )
tests.assert_called(
    zigbee.request_attribute_value,
    "check if setpoint value requested",
    {{ 42, 1, "thermostat", "OccupiedHeatingSetpoint" }}
)

-- check cooling
core.update_item_value = tests.create_mock()
zigbee.request_attribute_value = tests.create_mock()

events_handler( {
    event = "attribute_value_updated",
    node_id = 42,
    endpoint_id = 1,
    cluster_name = "thermostat",
    attribute = {
        name = "SystemMode",
        formatted_value = "cool"
    }
} )

tests.assert_called( core.update_item_value, "check if item values updated", {{ item_id, "cool" }} )
tests.assert_called(
    zigbee.request_attribute_value,
    "check if setpoint value requested",
    {{ 42, 1, "thermostat", "OccupiedCoolingSetpoint" }}
)

-- check auto
core.update_item_value = tests.create_mock()
zigbee.request_attribute_value = tests.create_mock()

events_handler( {
    event = "attribute_value_updated",
    node_id = 42,
    endpoint_id = 1,
    cluster_name = "thermostat",
    attribute = {
        name = "SystemMode",
        formatted_value = "auto"
    }
} )

tests.assert_called( core.update_item_value, "check if item values updated", {{ item_id, "auto" }} )
tests.assert_called(
    zigbee.request_attribute_value,
    "check if setpoints value requested",
    {{ 42, 1, "thermostat", "OccupiedCoolingSetpoint" }, { 42, 1, "thermostat", "OccupiedHeatingSetpoint" }}
)

--TODO test other modes
