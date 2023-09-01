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
zigbee.set_attribute_value = tests.create_mock()
zigbee.request_attribute_value = tests.create_mock()

loadfile( "HUB:zigbee/scripts/set_setting_value" )( {setting_id = setting_id, value = 0} )

tests.assert_called(
    zigbee.set_attribute_value,
    "check if attribute values updated",
    {
        {
            42, 1, "thermostat", "LocalTemperatureCalibration", 0
        }
    }
)

tests.assert_called(
    zigbee.request_attribute_value,
    "check if attribute values requested",
    {
        {
            42, 1, "thermostat", "LocalTemperatureCalibration"
        }
    }
)

-- check 1
zigbee.set_attribute_value = tests.create_mock()
zigbee.request_attribute_value = tests.create_mock()

loadfile( "HUB:zigbee/scripts/set_setting_value" )( {setting_id = setting_id, value = 1} )

tests.assert_called(
    zigbee.set_attribute_value,
    "check if attribute values updated",
    {
        {
            42, 1, "thermostat", "LocalTemperatureCalibration", 10
        }
    }
)

tests.assert_called(
    zigbee.request_attribute_value,
    "check if attribute values requested",
    {
        {
            42, 1, "thermostat", "LocalTemperatureCalibration"
        }
    }
)

-- check 2.5
zigbee.set_attribute_value = tests.create_mock()
zigbee.request_attribute_value = tests.create_mock()

loadfile( "HUB:zigbee/scripts/set_setting_value" )( {setting_id = setting_id, value = 2.5} )

tests.assert_called(
    zigbee.set_attribute_value,
    "check if attribute values updated",
    {
        {
            42, 1, "thermostat", "LocalTemperatureCalibration", 25
        }
    }
)

tests.assert_called(
    zigbee.request_attribute_value,
    "check if attribute values requested",
    {
        {
            42, 1, "thermostat", "LocalTemperatureCalibration"
        }
    }
)

-- check -2.5
zigbee.set_attribute_value = tests.create_mock()
zigbee.request_attribute_value = tests.create_mock()

loadfile( "HUB:zigbee/scripts/set_setting_value" )( {setting_id = setting_id, value = -2.5} )

tests.assert_called(
    zigbee.set_attribute_value,
    "check if attribute values updated",
    {
        {
            42, 1, "thermostat", "LocalTemperatureCalibration", -25
        }
    }
)

tests.assert_called(
    zigbee.request_attribute_value,
    "check if attribute values requested",
    {
        {
            42, 1, "thermostat", "LocalTemperatureCalibration"
        }
    }
)

-- check reset
zigbee.set_attribute_value = tests.create_mock()
zigbee.request_attribute_value = tests.create_mock()

loadfile( "HUB:zigbee/scripts/reset_setting" )( {setting_id = setting_id} )

tests.assert_called(
    zigbee.set_attribute_value,
    "check if attribute values updated",
    {
        {
            42, 1, "thermostat", "LocalTemperatureCalibration", 0
        }
    }
)

tests.assert_called(
    zigbee.request_attribute_value,
    "check if attribute values requested",
    {
        {
            42, 1, "thermostat", "LocalTemperatureCalibration"
        }
    }
)
