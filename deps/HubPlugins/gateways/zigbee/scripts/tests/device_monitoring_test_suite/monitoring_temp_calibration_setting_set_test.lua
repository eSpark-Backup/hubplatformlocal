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
local setting_id = storage:call(
    "find_setting_id",
    {
        setting_name = "temperature_calibration",
        node_id = 42,
        endpoint_id = 1,
        cluster_name = "temperature_measurement",
        attribute_name = "MeasuredValue"
    })

tests.assert_ne(setting_id, nil, "Try to find setting id")

-- check 0
core.set_setting_value = tests.create_mock()
zigbee.request_attribute_value = tests.create_mock()

loadfile( "HUB:zigbee/scripts/set_setting_value" )( {setting_id = setting_id, value = 0} )

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
            42, 1, "temperature_measurement", "MeasuredValue"
        }
    }
)

-- check 1
core.set_setting_value = tests.create_mock()
zigbee.request_attribute_value = tests.create_mock()

loadfile( "HUB:zigbee/scripts/set_setting_value" )( {setting_id = setting_id, value = 1} )

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
            42, 1, "temperature_measurement", "MeasuredValue"
        }
    }
)

-- check 2.5
core.set_setting_value = tests.create_mock()
zigbee.request_attribute_value = tests.create_mock()

loadfile( "HUB:zigbee/scripts/set_setting_value" )( {setting_id = setting_id, value = 2.5} )

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
            42, 1, "temperature_measurement", "MeasuredValue"
        }
    }
)

-- check -2.5
core.set_setting_value = tests.create_mock()
zigbee.request_attribute_value = tests.create_mock()

loadfile( "HUB:zigbee/scripts/set_setting_value" )( {setting_id = setting_id, value = -2.5} )

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
            42, 1, "temperature_measurement", "MeasuredValue"
        }
    }
)

-- check reset
core.set_setting_value = tests.create_mock()
zigbee.request_attribute_value = tests.create_mock()

loadfile( "HUB:zigbee/scripts/reset_setting" )( {setting_id = setting_id} )

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
            42, 1, "temperature_measurement", "MeasuredValue"
        }
    }
)
