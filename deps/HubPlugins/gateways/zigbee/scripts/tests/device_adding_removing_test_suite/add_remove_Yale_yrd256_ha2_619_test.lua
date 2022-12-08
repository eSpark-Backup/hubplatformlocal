local tests = require( "tests" )

local node = loadfile( "HUB:zigbee/scripts/tests/test_resources/yale_yrd256_ha2_619_node_structure" )()

local events_handler = assert( loadfile( "HUB:zigbee/scripts/events/zigbee/events_handling" ) )

local zigbee = require( "zigbee" )
zigbee.request_user_codes = tests.create_mock()

-- prepare zigbee
zigbee.add_node( node )

-- add node
events_handler( {
    event = "node_added",
    node_id = 9000
} )

local door_lock_state = require("HUB:zigbee/scripts/resources/enums/door_lock_state")

-- check adding result
local expected_items = {
    {
        name = "door_lock",
        endpoint_id = 1,
        cluster_name = "door_lock",
        attribute_name = "LockState",
        value_type = "token",
        enum = {
            door_lock_state.unsecured,
            door_lock_state.secured,
            door_lock_state.unknown
        },
        value = door_lock_state.unknown
    },
    {
        name = "battery",
        endpoint_id = 1,
        cluster_name = "power_configuration",
        attribute_name = "BatteryPercentageRemaining",
        value_type = "int",
        value = 100
    }
}

local expected_devices = {
    {
        name = "Door Lock",
        category = "door_lock",
        items = expected_items,
        node_id = 9000,
        battery_powered = true
    }
}

local check_context = {
    descriptors = {},
    item_ids = {},
    device_ids = {},
    setting_ids = {}
}

local test_suite_dir = "HUB:zigbee/scripts/tests/device_adding_removing_test_suite/"
loadfile( test_suite_dir .. "check_zigbee_node_adding_result" )( expected_devices, check_context )

tests.assert_called(
    zigbee.request_user_codes,
    "check if pin codes requested",
    { }
)

-- remove node
events_handler( {
    event = "node_removed",
    node_id = 9000,
    manufacturer_id = "Yale",
    model_id = "YRD256 TSDB",
} )

-- check node removing result
loadfile( test_suite_dir .. "check_zigbee_node_removing_result" )( check_context )
