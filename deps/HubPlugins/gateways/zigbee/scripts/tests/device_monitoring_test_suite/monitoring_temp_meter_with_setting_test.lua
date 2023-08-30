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

-- check setting 0
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

-- check setting 1
loadfile( "HUB:zigbee/scripts/set_setting_value" )( {setting_id = setting_id, value = 1} )
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
            { value = 16, scale = "celsius" },
            { value = 0, scale = "celsius" },
            { value = 40, scale = "celsius" }
        }
    })

-- check setting 2.5
loadfile( "HUB:zigbee/scripts/set_setting_value" )( {setting_id = setting_id, value = 2.5} )
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
            { value = 17.5, scale = "celsius" },
            { value = 0, scale = "celsius" },
            { value = 40, scale = "celsius" }
        }
    })

-- check setting -2.5
loadfile( "HUB:zigbee/scripts/set_setting_value" )( {setting_id = setting_id, value = -2.5} )
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
            { value = 12.5, scale = "celsius" },
            { value = 0, scale = "celsius" },
            { value = 40, scale = "celsius" }
        }
    })

-- check with setting lower border
loadfile( "HUB:zigbee/scripts/set_setting_value" )( {setting_id = setting_id, value = -2.5} )
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

-- check with setting higher border
loadfile( "HUB:zigbee/scripts/set_setting_value" )( {setting_id = setting_id, value = 2.5} )
core.update_item_value_with_min_max = tests.create_mock()

events_handler( {
    event = "attribute_value_updated",
    node_id = 42,
    endpoint_id = 1,
    cluster_name = "temperature_measurement",
    attribute = {
        name = "MeasuredValue",
        formatted_value = 4000
    }
} )

tests.assert_called(
    core.update_item_value_with_min_max,
    "check if item values updated",
    {
        {
            item_id,
            { value = 40, scale = "celsius" },
            { value = 0, scale = "celsius" },
            { value = 40, scale = "celsius" }
        }
    })
