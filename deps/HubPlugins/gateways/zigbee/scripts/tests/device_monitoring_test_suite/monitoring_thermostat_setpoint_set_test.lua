local tests = require( "tests" )
local core = require( "core" )
local zigbee = require( "zigbee" )
local timer = require( "timer" )

timer.set_timeout = tests.create_mock()
zigbee.set_attribute_value = tests.create_mock()

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

local cooling_setpoint_item_id = storage:call(
    "find_item_id",
    {
        item_name = "thermostat_setpoint_cooling",
        node_id = 42,
        endpoint_id = 1,
        cluster_name = "thermostat",
        attribute_name = "OccupiedCoolingSetpoint"
    })

local heating_setpoint_item_id = storage:call(
    "find_item_id",
    {
        item_name = "thermostat_setpoint_heating",
        node_id = 42,
        endpoint_id = 1,
        cluster_name = "thermostat",
        attribute_name = "OccupiedHeatingSetpoint"
    })

tests.assert_ne(cooling_setpoint_item_id, nil, "Try to find cooling setpoint item id")
tests.assert_ne(heating_setpoint_item_id, nil, "Try to find heating setpoint item id")

local function change_thermostat_mode(mode)
    -- switch to cooling mode
    events_handler( {
        event = "attribute_value_updated",
        node_id = 42,
        endpoint_id = 1,
        cluster_name = "thermostat",
        attribute = {
            name = "SystemMode",
            formatted_value = mode
        }
    } )
end

-- set heating mode
change_thermostat_mode("heat")

-- check set 0
loadfile( "HUB:zigbee/scripts/set_item_value" )( {item_id = heating_setpoint_item_id, value = {value = 0, scale = "celsius"}} )
tests.assert_called( zigbee.set_attribute_value, "check if attribute value seted", { { 42, 1, "thermostat", "OccupiedHeatingSetpoint", 0 } } )

-- check set 15
loadfile( "HUB:zigbee/scripts/set_item_value" )( {item_id = heating_setpoint_item_id, value = {value = 15, scale = "celsius"}} )
tests.assert_called( zigbee.set_attribute_value, "check if attribute value seted", { { 42, 1, "thermostat", "OccupiedHeatingSetpoint", 1500 } } )

-- check set -10
loadfile( "HUB:zigbee/scripts/set_item_value" )( {item_id = heating_setpoint_item_id, value = {value = -10, scale = "celsius"}} )
tests.assert_called( zigbee.set_attribute_value, "check if attribute value seted", { { 42, 1, "thermostat", "OccupiedHeatingSetpoint", -1000 } } )

-- set cooling mode
change_thermostat_mode("cool")

-- check set 0
loadfile( "HUB:zigbee/scripts/set_item_value" )( {item_id = cooling_setpoint_item_id, value = {value = 0, scale = "celsius"}} )
tests.assert_called( zigbee.set_attribute_value, "check if attribute value seted", { { 42, 1, "thermostat", "OccupiedCoolingSetpoint", 0 } } )

-- check set 15
loadfile( "HUB:zigbee/scripts/set_item_value" )( {item_id = cooling_setpoint_item_id, value = {value = 15, scale = "celsius"}} )
tests.assert_called( zigbee.set_attribute_value, "check if attribute value seted", { { 42, 1, "thermostat", "OccupiedCoolingSetpoint", 1500 } } )

-- check set -10
loadfile( "HUB:zigbee/scripts/set_item_value" )( {item_id = cooling_setpoint_item_id, value = {value = -10, scale = "celsius"}} )
tests.assert_called( zigbee.set_attribute_value, "check if attribute value seted", { { 42, 1, "thermostat", "OccupiedCoolingSetpoint", -1000 } } )

-- set heating in auto mode
change_thermostat_mode("auto")

-- check set 0
loadfile( "HUB:zigbee/scripts/set_item_value" )( {item_id = heating_setpoint_item_id, value = {value = 0, scale = "celsius"}} )
tests.assert_called(
    zigbee.set_attribute_value,
    "check if attribute value seted",
    {
        { 42, 1, "thermostat", "OccupiedHeatingSetpoint", 0 }
    }
)

-- check set 15
loadfile( "HUB:zigbee/scripts/set_item_value" )( {item_id = heating_setpoint_item_id, value = {value = 15, scale = "celsius"}} )
tests.assert_called(
    zigbee.set_attribute_value,
    "check if attribute value seted",
    {
        { 42, 1, "thermostat", "OccupiedHeatingSetpoint", 1500 }
    }
)

-- check set -10
loadfile( "HUB:zigbee/scripts/set_item_value" )( {item_id = heating_setpoint_item_id, value = {value = -10, scale = "celsius"}} )
tests.assert_called(
    zigbee.set_attribute_value,
    "check if attribute value seted",
    {
        { 42, 1, "thermostat", "OccupiedHeatingSetpoint", -1000 }
    }
)

-- set cooling in auto mode
change_thermostat_mode("auto")

-- check set 0
loadfile( "HUB:zigbee/scripts/set_item_value" )( {item_id = cooling_setpoint_item_id, value = {value = 0, scale = "celsius"}} )
tests.assert_called(
    zigbee.set_attribute_value,
    "check if attribute value seted",
    {
        { 42, 1, "thermostat", "OccupiedCoolingSetpoint", 0 }
    }
)

-- check set 15
loadfile( "HUB:zigbee/scripts/set_item_value" )( {item_id = cooling_setpoint_item_id, value = {value = 15, scale = "celsius"}} )
tests.assert_called(
    zigbee.set_attribute_value,
    "check if attribute value seted",
    {
        { 42, 1, "thermostat", "OccupiedCoolingSetpoint", 1500 }
    }
)

-- check set -10
loadfile( "HUB:zigbee/scripts/set_item_value" )( {item_id = cooling_setpoint_item_id, value = {value = -10, scale = "celsius"}} )
tests.assert_called(
    zigbee.set_attribute_value,
    "check if attribute value seted",
    {
        { 42, 1, "thermostat", "OccupiedCoolingSetpoint", -1000 }
    }
)

-- check with specified dead band
table.insert(cluster_thermostat.attributes, {name = "MinSetpointDeadBand", formatted_value = 15})

-- check set 15
loadfile( "HUB:zigbee/scripts/set_item_value" )( {item_id = heating_setpoint_item_id, value = {value = 15, scale = "celsius"}} )
tests.assert_called(
    zigbee.set_attribute_value,
    "check if attribute value seted",
    {
        { 42, 1, "thermostat", "OccupiedHeatingSetpoint", 1500 }
    }
)

-- check set 50 fahrenheit
loadfile( "HUB:zigbee/scripts/set_item_value" )( {item_id = cooling_setpoint_item_id, value = {value = 50, scale = "fahrenheit"}} )
tests.assert_called(
    zigbee.set_attribute_value,
    "check if attribute value seted",
    {
        { 42, 1, "thermostat", "OccupiedCoolingSetpoint", 1000 }
    }
)
