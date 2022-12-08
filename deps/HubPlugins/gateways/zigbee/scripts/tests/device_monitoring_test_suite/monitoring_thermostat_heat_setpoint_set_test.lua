local tests = require( "tests" )
local core = require( "core" )
local zigbee = require( "zigbee" )
local timer = require( "timer" )

timer.set_timeout = tests.create_mock()

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
        item_name = "thermostat_setpoint_heating",
        node_id = 42,
        endpoint_id = 1,
        cluster_name = "thermostat",
        attribute_name = "OccupiedHeatingSetpoint"
    })

tests.assert_ne(item_id, nil, "Try to find item id")

-- check set 0
zigbee.set_attribute_value = tests.create_mock()

loadfile( "HUB:zigbee/scripts/set_item_value" )( {item_id = item_id, value = {value = 0, scale = "celsius"}} )

tests.assert_called( zigbee.set_attribute_value, "check if attribute value seted", { { 42, 1, "thermostat", "OccupiedHeatingSetpoint", 0 } } )

-- check set 15
zigbee.set_attribute_value = tests.create_mock()

loadfile( "HUB:zigbee/scripts/set_item_value" )( {item_id = item_id, value = {value = 15, scale = "celsius"}} )

tests.assert_called( zigbee.set_attribute_value, "check if attribute value seted", { { 42, 1, "thermostat", "OccupiedHeatingSetpoint", 1500 } } )

-- check set -10
zigbee.set_attribute_value = tests.create_mock()

loadfile( "HUB:zigbee/scripts/set_item_value" )( {item_id = item_id, value = {value = -10, scale = "celsius"}} )

tests.assert_called( zigbee.set_attribute_value, "check if attribute value seted", { { 42, 1, "thermostat", "OccupiedHeatingSetpoint", -1000 } } )

-- check set 50 fahrenheit
zigbee.set_attribute_value = tests.create_mock()

loadfile( "HUB:zigbee/scripts/set_item_value" )( {item_id = item_id, value = {value = 50, scale = "fahrenheit"}} )

tests.assert_called( zigbee.set_attribute_value, "check if attribute value seted", { { 42, 1, "thermostat", "OccupiedHeatingSetpoint", 1000 } } )
