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
            name = "LocalTemperatureCalibration",
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
local setting_id = storage:call(
    "find_setting_id",
    {
        setting_name = "thermostat_temperature_calibration",
        node_id = 42,
        endpoint_id = 1,
        cluster_name = "thermostat",
        attribute_name = "LocalTemperatureCalibration"
    })

tests.assert_ne(setting_id, nil, "Try to find setting id")

-- check 0
core.set_setting_value = tests.create_mock()
zigbee.request_attribute_value = tests.create_mock()

events_handler( {
    event = "attribute_value_updated",
    node_id = 42,
    endpoint_id = 1,
    cluster_name = "thermostat",
    attribute = {
        name = "LocalTemperatureCalibration",
        formatted_value = 0
    }
} )

tests.assert_called(
    core.set_setting_value,
    "check if setting values updated",
    {
        {
            setting_id, { value = 0, scale = "celsius" }, "synced"
        }
    }
)

tests.assert_called(
    zigbee.request_attribute_value,
    "check if attribute values requested",
    {
        {
            42, 1, "thermostat", "LocalTemperature"
        }
    }
)

-- check 1
core.set_setting_value = tests.create_mock()
zigbee.request_attribute_value = tests.create_mock()

events_handler( {
    event = "attribute_value_updated",
    node_id = 42,
    endpoint_id = 1,
    cluster_name = "thermostat",
    attribute = {
        name = "LocalTemperatureCalibration",
        formatted_value = 10
    }
} )

tests.assert_called(
    core.set_setting_value,
    "check if setting values updated",
    {
        {
            setting_id, { value = 1, scale = "celsius" }, "synced"
        }
    }
)

tests.assert_called(
    zigbee.request_attribute_value,
    "check if attribute values requested",
    {
        {
            42, 1, "thermostat", "LocalTemperature"
        }
    }
)

-- check 2.5
core.set_setting_value = tests.create_mock()
zigbee.request_attribute_value = tests.create_mock()

events_handler( {
    event = "attribute_value_updated",
    node_id = 42,
    endpoint_id = 1,
    cluster_name = "thermostat",
    attribute = {
        name = "LocalTemperatureCalibration",
        formatted_value = 25
    }
} )

tests.assert_called(
    core.set_setting_value,
    "check if setting values updated",
    {
        {
            setting_id, { value = 2.5, scale = "celsius" }, "synced"
        }
    }
)

tests.assert_called(
    zigbee.request_attribute_value,
    "check if attribute values requested",
    {
        {
            42, 1, "thermostat", "LocalTemperature"
        }
    }
)

-- check -2.5
core.set_setting_value = tests.create_mock()
zigbee.request_attribute_value = tests.create_mock()

events_handler( {
    event = "attribute_value_updated",
    node_id = 42,
    endpoint_id = 1,
    cluster_name = "thermostat",
    attribute = {
        name = "LocalTemperatureCalibration",
        formatted_value = -25
    }
} )

tests.assert_called(
    core.set_setting_value,
    "check if setting values updated",
    {
        {
            setting_id, { value = -2.5, scale = "celsius" }, "synced"
        }
    }
)

tests.assert_called(
    zigbee.request_attribute_value,
    "check if attribute values requested",
    {
        {
            42, 1, "thermostat", "LocalTemperature"
        }
    }
)
