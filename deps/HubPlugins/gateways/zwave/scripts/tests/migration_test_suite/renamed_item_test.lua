local zwave = require("zwave")
local core = require("core")
local storage = require( "storage" )
local tests = require("tests")
local events_handler = assert( loadfile( "HUB:zwave/scripts/events/events_handling" ) )
local item_names = require("HUB:zwave/scripts/resources/item_names")
local class_ids = require( "HUB:zwave/scripts/resources/class_ids" )

zwave.request_node_values = tests.create_mock()

local default_changes = loadfile("HUB:zwave/scripts/migration/version_changes/default")()
default_changes.cc_to_check = {[class_ids.switch_binary] = true}
default_changes.items_old_names = {["dimmer"] = "switch"}
loadfile = loadfile( "HUB:zwave/scripts/tests/migration_test_suite/setup_versions")(
    storage,
    default_changes,
    1,
    2
)

--prepare zwave node
local node = loadfile( "HUB:zwave/scripts/tests/migration_test_suite/simple_zwave_node_resource" )()
zwave.add_node( node )

-- add node
item_names.switch = "dimmer"
events_handler( {
    event = "node_added",
    node_id = 9000 
} )

-- check adding result
local expected_switch_items = {
    {
        name = "dimmer",
        cc_id = class_ids.switch_binary,
        channel_id = 0
    }
}

local expected_devices = {
    {
        name = "Water Valve",
        category = "switch",
        subcategory = "valve",
        items = expected_switch_items,
        node_id = "9000",
        battery_powered = true
    }
}

local test_suite_dir = "HUB:zwave/scripts/tests/device_adding_removing_test_suite/"
loadfile( test_suite_dir .. "check_zwave_node_adding_result" )( expected_devices )

-- modify item name
item_names.switch = "switch"

core.add_device = tests.create_mock()
core.add_item = tests.create_mock()
core.remove_device_sync = tests.create_mock()
core.remove_device = tests.create_mock()
core.remove_item = tests.create_mock()

-- execute migration
loadfile("HUB:zwave/scripts/tests/migration_test_suite/run_migration")()

-- check migration result
expected_devices[1].items[1].name = "switch"
loadfile( test_suite_dir .. "check_zwave_node_adding_result" )( expected_devices )

tests.assert_called( core.add_device, "no new device added", nil, 0 )
tests.assert_called( core.add_item, "no new item added", nil, 0 )
tests.assert_called( core.remove_device_sync, "no any device deleted", nil, 0 )
tests.assert_called( core.remove_device, "no any device deleted", nil, 0 )
tests.assert_called( core.remove_item, "no any item deleted", nil, 0 )
