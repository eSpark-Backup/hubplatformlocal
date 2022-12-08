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
        },
        {
            name = "MinCoolSetpointLimit",
            formatted_value = "undefined"
        },
        {
            name = "MaxCoolSetpointLimit",
            formatted_value = "undefined"
        },
        {
            name = "MinHeatSetpointLimit",
            formatted_value = "undefined"
        },
        {
            name = "MaxHeatSetpointLimit",
            formatted_value = "undefined"
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

-- switch to cooling mode
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

-- check heat setpoint update
core.update_item_value_with_min_max = tests.create_mock()
events_handler( {
    event = "attribute_value_updated",
    node_id = 42,
    endpoint_id = 1,
    cluster_name = "thermostat",
    attribute = {
        name = "OccupiedHeatingSetpoint",
        formatted_value = 1000
    }
} )

tests.assert_called(
    core.update_item_value_with_min_max,
    "check if item values updated",
    {
        {
            heating_setpoint_item_id,
            { value = 10, scale = "celsius" },
            { value = 0, scale = "celsius" },
            { value = 40, scale = "celsius" }
        }
    } )

-- check cool setpoint update
core.update_item_value_with_min_max = tests.create_mock()
events_handler( {
    event = "attribute_value_updated",
    node_id = 42,
    endpoint_id = 1,
    cluster_name = "thermostat",
    attribute = {
        name = "OccupiedCoolingSetpoint",
        formatted_value = 2000
    }
} )

tests.assert_called(
    core.update_item_value_with_min_max,
    "check if item values updated",
    {
        {
            cooling_setpoint_item_id,
            { value = 20, scale = "celsius" },
            { value = 0, scale = "celsius" },
            { value = 40, scale = "celsius" }
        }
    } )

-- check cool setpoint with borders update
core.update_item_value_with_min_max = tests.create_mock()

cluster_thermostat.attributes[5].formatted_value = -1000
cluster_thermostat.attributes[6].formatted_value = 3000

events_handler( {
    event = "attribute_value_updated",
    node_id = 42,
    endpoint_id = 1,
    cluster_name = "thermostat",
    attribute = {
        name = "OccupiedCoolingSetpoint",
        formatted_value = 2000
    }
} )

tests.assert_called(
    core.update_item_value_with_min_max,
    "check if item values updated",
    {
        {
            cooling_setpoint_item_id,
            { value = 20, scale = "celsius" },
            { value = -10, scale = "celsius" },
            { value = 30, scale = "celsius" }
        }
    } )
