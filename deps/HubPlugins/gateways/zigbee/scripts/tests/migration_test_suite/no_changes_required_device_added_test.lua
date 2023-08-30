local zigbee = require("zigbee")
local core = require("core")
local storage = require( "storage" )
local tests = require("tests")

local default_changes = loadfile("HUB:zigbee/scripts/migration/version_changes/default")()
loadfile = loadfile( "HUB:zigbee/scripts/tests/migration_test_suite/setup_versions")(
    storage,
    default_changes,
    1,
    2
)

--prepare zigbee node
local node = loadfile( "HUB:zigbee/scripts/tests/migration_test_suite/simple_zigbee_node_resource" )()
zigbee.add_node( node )

-- add node
local events_handler = assert( loadfile( "HUB:zigbee/scripts/events/zigbee/events_handling" ) )
events_handler( {
    event = "node_added",
    node_id = 42 
} )

core.add_device = tests.create_mock()
core.add_item = tests.create_mock()
core.remove_device_sync = tests.create_mock()
core.remove_device = tests.create_mock()
core.remove_item = tests.create_mock()
core.modify_device = tests.create_mock()
core.modify_item = tests.create_mock()

-- execute migration
loadfile("HUB:zigbee/scripts/tests/migration_test_suite/run_migration")()

tests.assert_called( core.add_device, "no new device added", nil, 0 )
tests.assert_called( core.add_item, "no new item added", nil, 0 )
tests.assert_called( core.remove_device_sync, "no any device deleted", nil, 0 )
tests.assert_called( core.remove_device, "no any device deleted", nil, 0 )
tests.assert_called( core.remove_item, "no any item deleted", nil, 0 )
tests.assert_called( core.modify_device, "root device modified", nil, 0 )
tests.assert_called( core.modify_item, "item modified", nil, 0 )
