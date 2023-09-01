local tests = require( "tests" )
local core = require("core")

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

-- check adding result
local expected_items = {
    {
        name = "switch",
        endpoint_id = 1,
        cluster_name = "on_off",
        attribute_name = "OnOff",
        value_type = "bool",
        value = false
    },
    {
        name = "electric_meter_volt",
        endpoint_id = 1,
        cluster_name = "electrical_measurement",
        attribute_name = "RMSVoltage",
        value_type = "electric_potential",
        value = {
            value = 0,
            scale = "volt",
        },
    }
}


local expected_devices = {
    {
        name = "Plug",
        category = "switch",
        subcategory = "interior_plugin",
        items = expected_items,
        node_id = 42,
        battery_powered = false
    }
}

local test_suite_dir = "HUB:zigbee/scripts/tests/device_adding_removing_test_suite/"
loadfile( test_suite_dir .. "check_zigbee_node_adding_result" )( expected_devices )

-- modify zigbee node
table.insert(node.endpoints[1].clusters[2].attributes, 
    {
        name = "RMSCurrent",
        formatted_value = 0
    }
)

-- modify expectations
table.insert(expected_items, 
    {
        name = "electric_meter_amper",
        endpoint_id = 1,
        cluster_name = "electrical_measurement",
        attribute_name = "RMSCurrent",
        value_type = "electric_current",
        value = {
            value = 0,
            scale = "ampere",
        },
    }
)

core.remove_device = tests.create_mock()

-- execute migration
loadfile("HUB:zigbee/scripts/tests/migration_test_suite/run_migration")()

-- check migration result
loadfile( test_suite_dir .. "check_zigbee_node_adding_result" )( expected_devices )

tests.assert_called( core.remove_device, "no devices deleted", nil, 0 )
