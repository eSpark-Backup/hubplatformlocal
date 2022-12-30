local zigbee = require("zigbee")
local core = require("core")
local storage = require("storage")
local tests = require("tests")

local default_changes = loadfile("HUB:zigbee/scripts/migration/version_changes/default")()
default_changes.check_all_nodes = true
loadfile = loadfile( "HUB:zigbee/scripts/tests/migration_test_suite/setup_versions")(
    storage,
    default_changes,
    1,
    1
)

--prepare zigbee node
local node = loadfile( "HUB:zigbee/scripts/tests/migration_test_suite/simple_zigbee_node_resource" )()
zigbee.add_node( node )

local events_handler = assert( loadfile( "HUB:zigbee/scripts/events/zigbee/events_handling" ) )
events_handler( {
    event = "node_added",
    node_id = 42
} )

core.devices = {}
core.items = {}

local restrictions_helper = hubplugins.object("include_restrictions_helper", "HUB:zigbee/scripts/helpers")
restrictions_helper:call("storage/insert_restricted_node_id", 42)

-- execute migration
loadfile("HUB:zigbee/scripts/tests/migration_test_suite/run_migration")()

tests.assert_eq(core.get_devices(), nil, "Check if no devices added")
tests.assert_eq(#core.get_items(), 0, "Check if no items added")
